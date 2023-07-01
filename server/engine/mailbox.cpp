#include "mailbox.h"

MailBox::MailBox() {

}

MailBox::~MailBox() {

}

void MailBox::set_entity_and_addr(const GString& entity_uuid, const GString& addr) {
    m_entity_uuid = entity_uuid;
    m_addr = addr;
}

GString MailBox::get_entity_uuid() const { 
    return m_entity_uuid; 
}

GString MailBox::get_addr() const {
    return m_addr; 
}

BaseMailBox& MailBox::to_base() {
    static BaseMailBox base;
    base.set_entity_and_addr(m_entity_uuid, m_addr);
    base.m_session_cache = m_session_cache;
    base.m_remote_cache = m_remote_cache;
    return base;
}

CellMailBox& MailBox::to_cell() {
    static CellMailBox cell;
    cell.set_entity_and_addr(m_entity_uuid, m_addr);
    cell.m_session_cache = m_session_cache;
    cell.m_remote_cache = m_remote_cache;
    return cell;
}
