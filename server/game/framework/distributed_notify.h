#pragma once

#include "engine/engine.h"


// 广播
template<class... T>
void game_broadcast(const GString& rpc_name, T... args) {
    auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);
    auto gate = g_session_mgr.get_rand_session();
    if (gate == nullptr) {
        ERROR_LOG("game_broadcast(%s) failed, gate empty!!!\n", rpc_name.c_str());
        return;
    }
    REMOTE_RPC_CALL(gate, "game_broadcast", inner_rpc);
}

// 单播
template<class... T>
void game_unicast(const GString& addr, const GString& rpc_name, T... args) {
    auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);
    auto gate = g_session_mgr.get_fixed_session(addr);
    if (gate == nullptr) {
        ERROR_LOG("game_unicast(%s) failed, gate empty!!!\n", rpc_name.c_str());
        return;
    }
    REMOTE_RPC_CALL(gate, "game_unicast", addr, inner_rpc);
}

// 随机单播
template<class... T>
void game_unicast_random(const GString& rpc_name, T... args) {
    auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);
    auto gate = g_session_mgr.get_rand_session();
    if (gate == nullptr) {
        ERROR_LOG("game_unicast_random(%s) failed, gate empty!!!\n", rpc_name.c_str());
        return;
    }
    REMOTE_RPC_CALL(gate, "game_unicast_random", inner_rpc);
}

#define GAME_BROADCAST(rpc_name, ...) game_broadcast(#rpc_name, __VA_ARGS__)
#define GAME_UNICAST(addr, rpc_name, ...) game_unicast(addr, #rpc_name, __VA_ARGS__)
#define GAME_UNICAST_RANDOM(rpc_name, ...) game_unicast_random(#rpc_name, __VA_ARGS__)
