#include "engine/utils.h"

#include "gate_instance.h"

//extern shared_ptr<RpcImp> g_cur_imp;
//
//GENERATE_ENTITY_OUT(BaseGateInstance);
//
//void BaseGateInstance::on_ready() {
//    REGIST_TIMER(0, 1, true, heartbeat_check_timer);
//}
//
//void BaseGateInstance::heartbeat_check_timer() {
//
//    auto nowms = nowms_timestamp(true);
//    vector<GString> remote_tobe_remove;
//    g_remote_mgr.foreach_remote([nowms, &remote_tobe_remove](const GString& remote_name, shared_ptr<Remote> remote) {
//        if (remote->get_last_active_time() != 0 && remote->get_last_active_time() + 6000 < nowms) {
//            // 6s³¬Ê±
//            remote->close();
//            remote_tobe_remove.push_back(remote_name);
//        }
//    });
//
//    for (auto iter = remote_tobe_remove.begin(); iter != remote_tobe_remove.end(); ++iter) {
//        g_remote_mgr.on_remote_disconnected(*iter);
//    }
//}

void create_gate_instance() {
    //g_gate_instance = create_local_base_entity("GateInstance", gen_uuid());
    //g_gate_instance->on_create(GDict());
}

void heartbeat_from_game() {
    // remote's last active time is update
    // here do nothing
}

HeatbeatThreadObj::HeatbeatThreadObj() {
    headbeat_buff = g_rpc_manager.rpc_encode("heartbeat_from_gate");
}

void HeatbeatThreadObj::operator()() {
    while (true) {
        heart_beat_check();
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

void HeatbeatThreadObj::heart_beat_check() {
    auto nowms = nowms_timestamp(true);
    vector<GString> remote_tobe_remove;
    g_remote_mgr.foreach_remote([this, nowms, &remote_tobe_remove](const GString& remote_name, shared_ptr<Remote> remote) {
        if (remote->get_last_active_time() != 0 && remote->get_last_active_time() + 3000 < nowms) {
            // 3s³¬Ê±
            remote->close();
            remote_tobe_remove.push_back(remote_name);
        }

        auto next_heartbeat_time = remote->get_next_heartbeat_time();
        if (next_heartbeat_time < nowms) {
            remote->send_buff_thread_safe(headbeat_buff);
            if (next_heartbeat_time == 0) {
                // É¢ÁÐ
                remote->set_next_heartbeat_time(nowms + rand() % 1000);
            }
            else {
                remote->set_next_heartbeat_time(nowms + 1000);
            }
        }
    });

    for (auto iter = remote_tobe_remove.begin(); iter != remote_tobe_remove.end(); ++iter) {
        g_remote_mgr.on_remote_disconnected(*iter);
    }
}

void assist_thread_start() {
    HeatbeatThreadObj thread_obj;
    boost::thread t(thread_obj);
}
