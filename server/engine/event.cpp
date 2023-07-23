#include <vector>

#include "event.h"
#include "utils.h"
#include "encode.h"
#include "decode.h"
#include "entity.h"


vector<EventBase*>* EventManagerBase::get_event_cb_set(Entity* owner, const GString& event_name) {
    auto iter = owner->events.find(event_name);
    if (iter == owner->events.end()) {
        owner->events.insert(make_pair(event_name, vector<EventBase*>()));
        iter = owner->events.find(event_name);
    }

    return &(iter->second);
}

void EventManagerBase::restore_event(Entity* entity, const GBin& event_bin) {
    Decoder decoder(event_bin.buf, event_bin.size);
    decoder.skip_head_len();
    GString event_name = decoder.read_string();
    GString component_name = decoder.read_string();

    auto iter = event_cbs_store.find(event_name + component_name);
    if (iter == event_cbs_store.end()) {
        return;
    }

    EventBase* event = iter->second->create_self();

    if (component_name != "") {
        auto component = entity->get_component(component_name);
        event->set_owner(component);
    }
    else {
        event->set_owner(entity);
    }
    event->m_event_name = event_name;
    event->m_component_name = component_name;

    entity->add_event(event);
}
