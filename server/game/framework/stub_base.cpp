#include "stub_base.h"

#include "distributed_notify.h"

void StubBase::on_ready() {
    INFO_LOG("stub(%s) ready\n", get_entity_name().c_str());
    GAME_BROADCAST(on_stub_create, get_entity_name(), get_self_mailbox());
}


GDict g_stub_proxys;
void on_stub_create(const GString& stub_name, const MailBox& mailbox) {
    INFO_LOG("on stub(%s) create\n", stub_name.c_str());
    ASSERT_LOG(g_stub_proxys.find(stub_name) == g_stub_proxys.end(), "stub(%s) repeat!!!", stub_name.c_str());
    g_stub_proxys.insert(make_pair(stub_name, mailbox));
}
