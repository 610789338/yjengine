#pragma once

#include "engine/engine.h"


class StubBase : public BaseEntity {

public:
    StubBase() {}
    ~StubBase() {}

    virtual GString get_entity_name() { return ""; };

    void on_ready();
};


extern GDict g_stub_proxys;

template<class... T>
void call_stub(const GString& stub_name, const GString& rpc_name, T... args) {
    auto iter = g_stub_proxys.find(stub_name);
    if (iter == g_stub_proxys.end()) {
        ERROR_LOG("call stub(%s) not exist!!!", stub_name.c_str());
        return;
    }

    iter->as_mailbox().to_base().call(rpc_name, args...);
}
