#include "engine/utils.h"

#include "game_instance.h"


//GENERATE_ENTITY_OUT(BaseGameInstance);
//
//void BaseGameInstance::on_ready() {
//    REGIST_TIMER(0, 1, true, heartbeat_check_timer);
//}
//
//void BaseGameInstance::heartbeat_check_timer() {
//
//    auto nowms = nowms_timestamp(true);
//    vector<GString> session_tobe_remove;
//    g_session_mgr.foreach_session([nowms, &session_tobe_remove](const GString& session_name, shared_ptr<Session> session) {
//        if (session->get_last_active_time() != 0 && session->get_last_active_time() + 6000 < nowms) {
//            // 6s超时
//            session->close();
//            session_tobe_remove.push_back(session_name);
//        }
//        else {
//            // TODO - 还是要放到子线程里面去，避免主线程太忙导致误判
//            REMOTE_RPC_CALL(session, "heartbeat_from_game");
//        }
//    });
//
//    for (auto iter = session_tobe_remove.begin(); iter != session_tobe_remove.end(); ++iter) {
//        g_session_mgr.on_session_disconnected(*iter);
//    }
//}

void create_game_instance() {
    //g_game_instance = create_local_base_entity("GameInstance", gen_uuid());
    //g_game_instance->on_create(GDict());
}

void heartbeat_from_gate() {
    // session's last active time is update
    // here do nothing
}

void HeatbeatThreadObj::operator()() {
    while (true) {
        heart_beat_check();
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

void HeatbeatThreadObj::heart_beat_check() {
    auto nowms = nowms_timestamp();
    vector<shared_ptr<Session>> session_tobe_remove;
    g_session_mgr.foreach_session([this, nowms, &session_tobe_remove](const GString& session_name, shared_ptr<Session> session) {
        if (session->get_last_active_time() != 0 && session->get_last_active_time() + 3000 < nowms) {
            // 3s超时，断开连接
            session->close();
            session_tobe_remove.push_back(session);
        }

        auto next_heartbeat_time = session->get_next_heartbeat_time();
        if (next_heartbeat_time < nowms) {
            // REMOTE/LOCAL RPC CALL应该是线程安全的，因为m_s2l/m_l2s没有写操作
            REMOTE_RPC_CALL(session, "heartbeat_from_game");
            session->set_next_heartbeat_time(nowms + 1000);
        }
    });

    for (auto iter = session_tobe_remove.begin(); iter != session_tobe_remove.end(); ++iter) {
        // LOCAL_RPC_CALL是异步本地调用，所以要shared_from_this
        LOCAL_RPC_CALL((*iter)->shared_from_this(), "disconnect_from_client", (*iter)->get_remote_addr());
    }
}

void assist_thread_start() {
    HeatbeatThreadObj thread_obj;
    boost::thread t(thread_obj);
}
