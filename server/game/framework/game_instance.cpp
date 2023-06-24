#include <vector>
#include <string>

#include "boost/regex.hpp"

#include "engine/utils.h"
#include "engine/log.h"

#include "game_instance.h"

using namespace std;

GENERATE_ENTITY_OUT(GameInstance);

extern void create_stub(const GString& stub_name);

void GameInstance::on_ready() {
    // 延时1s等待集群互连后广播通知
    // TODO - 等做了服务器发现再做动态通知
    REGIST_TIMER(1, 0, false, create_stubs);
}

void GameInstance::create_stubs() {
    auto stub_names_ini = ini_get_string("Stub", "stub_names", "");
    if (stub_names_ini == "") {
        return;
    }

    vector<string> stub_names;
    split(stub_names_ini, "|", stub_names);
    for (auto iter1 = stub_names.begin(); iter1 != stub_names.end(); ++iter1) {
        create_stub(*iter1);
    }
}

void GameInstance::on_gate_disappear(const GString& gate_addr) {
}

GameInstance* g_game_instance = nullptr;

void create_game_instance() {
    g_game_instance = (GameInstance*)create_local_base_entity("GameInstance", gen_uuid());
    g_game_instance->on_create(GDict());
    g_game_instance->ready();
}

void heartbeat_from_gate() {
    // session's last active time is update
    // here do nothing
}

void heart_beat_tick() {
    auto nowms = nowms_timestamp();
    vector<shared_ptr<Session>> session_tobe_remove;
    g_session_mgr.foreach_session([nowms, &session_tobe_remove](const GString& session_name, shared_ptr<Session> session) {
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
        //INFO_LOG("heatbeat check gate(%s) disappear!!!\n", (*iter)->get_remote_addr().c_str());
        LOCAL_RPC_CALL((*iter)->shared_from_this(), "disconnect_from_client", (*iter)->get_remote_addr());
    }
}

void assist_thread() {
    while (true) {
        // TODO - open heart beat check
        // heart_beat_tick();
        log_queue_tick();
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

void assist_thread_start() {
    boost::thread t(assist_thread);
}
