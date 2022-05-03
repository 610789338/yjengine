#pragma once

#include <string>

using namespace std;

class RpcImp {
public:
    RpcImp(string rpc_name) : m_rpc_name(std::move(rpc_name)) {}
    ~RpcImp() {}

private:
    string m_rpc_name;
    // m_rpc_param;
};

class RpcManager {
public:
    uint16_t rpc_imp_generate(const char *buf, uint16_t length);
    RpcImp&& rpc_decode(const char* buf);
    //char* rcp_encode(RpcImp&& rpc_imp);
};

extern RpcManager g_rpc_manager;
