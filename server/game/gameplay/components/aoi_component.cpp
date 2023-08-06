#include "aoi_component.h"

void AoiComponent::tick() {
    for (auto iter = others.begin(); iter != others.end(); ++iter) {
        Encoder encoder;
        iter->second->serialize_otherclient(encoder);
        encoder.write_end();
        if (encoder.anything()) {
            Client().call("other_prop_sync_from_cell", iter->first, GBin(encoder.get_buf(), encoder.get_offset()));
        }
    }
}

void AoiComponent::on_other_enter_aoi(const GString& entity_uuid, int64_t p) {
    Entity* other = (Entity*)p;
    others.insert(make_pair(entity_uuid, other));
    Encoder encoder;
    other->serialize_otherclient_all(encoder);
    encoder.write_end();
    Client().call("on_other_enter_aoi", entity_uuid, other->entity_name, GBin(encoder.get_buf(), encoder.get_offset()));
}

void AoiComponent::on_other_leave_aoi(const GString& entity_uuid, int64_t p) {
    Entity* other = (Entity*)p;
    others.erase(entity_uuid);
    Client().call("on_other_leave_aoi", entity_uuid);
}

void AoiComponent::clear_aoi() {
    others.clear();
}
