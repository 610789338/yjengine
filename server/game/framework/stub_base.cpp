#include "stub_base.h"

#include "distributed_notify.h"


void StubBase::on_create(const GDict& create_data) {
    INFO_LOG("stub(%s) create\n", get_class_name().c_str());
    GAME_BROADCAST(on_stub_create, get_class_name(), get_self_mailbox());

    BaseEntity::on_create(create_data);
}


GDict g_stub_proxys;
void on_stub_create(const GString& stub_name, MailBox& mailbox) {
    INFO_LOG("stub(%s) create on %s\n", stub_name.c_str(), mailbox.get_addr().c_str());
    ASSERT_LOG(g_stub_proxys.find(stub_name) == g_stub_proxys.end(), "stub(%s) repeat!!!", stub_name.c_str());
    g_stub_proxys.insert(make_pair(stub_name, mailbox));
}
