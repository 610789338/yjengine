#include "engine/utils.h"

#include "gate_instance.h"

extern shared_ptr<RpcImp> g_cur_imp;

GENERATE_ENTITY_OUT(BaseGateInstance);

void BaseGateInstance::on_ready() {
    REGIST_TIMER(0, 1, true, heartbeat_check_timer);
}

void BaseGateInstance::heartbeat_check_timer() {

    auto nowms = nowms_timestamp(true);
    vector<GString> remote_tobe_remove;
    g_remote_mgr.foreach_remote([nowms, &remote_tobe_remove](const GString& remote_name, shared_ptr<Remote> remote) {
        if (remote->get_last_active_time() != 0 && remote->get_last_active_time() + 6000 < nowms) {
            // 6s³¬Ê±
            remote->close();
            remote_tobe_remove.push_back(remote_name);
        }
    });

    for (auto iter = remote_tobe_remove.begin(); iter != remote_tobe_remove.end(); ++iter) {
        g_remote_mgr.on_remote_disconnected(*iter);
    }
}

void heartbeat_req_from_game() {
    // remote's last active time is update
    // ack to game
    auto remote = g_cur_imp->get_remote();
    REMOTE_RPC_CALL(remote, "heartbeat_ack_from_gate");
}

void create_gate_instance() {
    g_gate_instance = create_local_base_entity("GateInstance", gen_uuid());
    g_gate_instance->on_create(GDict());
}
