#include "mailbox.h"
#include "entity.h"

MailBox::MailBox() {

}

MailBox::~MailBox() {

}

bool MailBox::is_valid() {
    return !m_entity_uuid.empty() && !m_addr.empty();
}

void MailBox::set_entity_and_addr(const GString& entity_uuid, const GString& addr) {
    m_entity_uuid = entity_uuid;
    m_addr = addr;
}

void MailBox::set_flag(int8_t flag) {
    m_flag = flag;
}

GString MailBox::get_entity_uuid() const { 
    return m_entity_uuid; 
}

GString MailBox::get_addr() const {
    return m_addr; 
}

int8_t MailBox::get_flag() const {
    return m_flag;
}

Entity* MailBox::get_owner() {
    if (!m_owner) {
        if (is_base()) {
            m_owner = thread_safe_get_base_entity(get_entity_uuid());
        }
        else if (is_cell()) {
            m_owner = thread_safe_get_cell_entity(get_entity_uuid());
        }
        else {
            m_owner = thread_safe_get_client_entity(get_entity_uuid());
        }
    }

    return m_owner;
}

MailBoxProxy& MailBox::to_proxy() const {
    static MailBoxProxy proxy;
    proxy.set_entity_and_addr(m_entity_uuid, m_addr);
    proxy.set_flag(m_flag);
    proxy.m_session_cache = m_session_cache;
    proxy.m_remote_cache = m_remote_cache;
    return proxy;
}
