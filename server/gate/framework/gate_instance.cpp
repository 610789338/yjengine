#include "engine/utils.h"

#include "gate_instance.h"

GENERATE_ENTITY_OUT(BaseGateInstance);


void BaseGateInstance::on_game_disappear(const GString& game_addr) {
    g_remote_mgr.foreach_remote([this, &game_addr](const GString& remote_name, shared_ptr<Remote> remote) {
        REMOTE_RPC_CALL(remote, "on_game_disappear", game_addr);
    });
}

BaseGateInstance* g_gate_instance = nullptr;

void create_gate_instance() {
    g_gate_instance = (BaseGateInstance*)create_local_base_entity("GateInstance", gen_uuid());
    g_gate_instance->on_create(GDict());
}

void heartbeat_from_game() {
    // remote's last active time is update
    // here do nothing
}

void HeatbeatThreadObj::operator()() {
    while (true) {
        heart_beat_check();
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

void HeatbeatThreadObj::heart_beat_check() {
    auto nowms = nowms_timestamp(true);
    vector<shared_ptr<Remote>> remote_tobe_remove;
    g_remote_mgr.foreach_remote([this, nowms, &remote_tobe_remove](const GString& remote_name, shared_ptr<Remote> remote) {
        if (remote->get_last_active_time() != 0 && remote->get_last_active_time() + 3000 < nowms) {
            // 3s³¬Ê±
            remote->close();
            remote_tobe_remove.push_back(remote);
        }

        auto next_heartbeat_time = remote->get_next_heartbeat_time();
        if (next_heartbeat_time < nowms) {
            REMOTE_RPC_CALL(remote, "heartbeat_from_gate");
            remote->set_next_heartbeat_time(nowms + 1000);
        }
    });

    for (auto iter = remote_tobe_remove.begin(); iter != remote_tobe_remove.end(); ++iter) {
        //g_remote_mgr.on_remote_disconnected(*iter);
        INFO_LOG("heatbeat check game(%s) disappear!!!\n", (*iter)->get_remote_addr().c_str());
        LOCAL_RPC_CALL((*iter)->shared_from_this(), "on_remote_disconnected", (*iter)->get_remote_addr());
    }
}

void assist_thread_start() {
    HeatbeatThreadObj thread_obj;
    boost::thread t(thread_obj);
}
