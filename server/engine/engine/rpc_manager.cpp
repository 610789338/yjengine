#include "rpc_manager.h"

RpcManager g_rpc_manager;

uint16_t read_uint16(const char *buf) {
    return (uint16_t(buf[0]) << 8) | uint16_t(buf[1]);
}

string&& read_string(const char *buf) {
    auto str_len = read_uint16(buf);
    return string(buf + 2, buf + 2 + str_len);
}

uint16_t RpcManager::rpc_imp_generate(const char *buf, uint16_t length) {

    uint16_t ret = 0;

    if (length <= 2) {
        return ret;
    }

    while (true) {
        uint16_t pkg_len = read_uint16(buf);
        if (ret + 2 + pkg_len > length) {
            break;
        }

        // TODO - 生成RPC实例，传给主线程调用
        RpcImp &&imp = rpc_decode(buf + ret + 2);

        ret += 2;        // pkg head len
        ret += pkg_len;  // pkg len
    }

    return ret;
}

RpcImp&& RpcManager::rpc_decode(const char* buf) {
    string&& rpc_name = read_string(buf);
    return RpcImp(rpc_name);
}