#pragma once

#include "engine/engine.h"


class StubBase : public BaseEntity {

public:
    StubBase() {}
    ~StubBase() {}

    virtual void on_create(const GDict& create_data);

    virtual GString get_entity_name() { return ""; };

};


extern GDict g_stub_proxys;

template<class... T>
void call_stub(const GString& stub_name, const GString& rpc_name, T... args) {
    auto iter = g_stub_proxys.find(stub_name);
    if (iter == g_stub_proxys.end()) {
        ERROR_LOG("call stub(%s) not exist!!!", stub_name.c_str());
        return;
    }

    iter->second.as_mailbox().to_proxy().call(rpc_name, args...);
}
