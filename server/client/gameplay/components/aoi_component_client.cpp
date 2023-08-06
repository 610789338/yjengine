#include "aoi_component_client.h"

void AoiComponentClient::on_other_enter_aoi(const GString& entity_uuid, const GString& entity_name, const GBin& v) {
    Entity* entity = create_other_entity(entity_name, entity_uuid);

    Decoder decoder(v.buf, v.size);
    decoder.skip_head_len();
    entity->propertys_unserialize(decoder);

    others.insert(make_pair(entity_uuid, entity));
}

void AoiComponentClient::on_other_leave_aoi(const GString& entity_uuid) {
    auto iter = others.find(entity_uuid);
    if (iter != others.end()) {
        delete iter->second;
        others.erase(iter);
    }
}

void AoiComponentClient::other_prop_sync_from_cell(const GString& entity_uuid, const GBin& v) {
    auto iter = others.find(entity_uuid);
    if (iter == others.end()) {
        ERROR_LOG("other prop sync error: entity.%s not exist\n", entity_uuid.c_str());
        return;
    }

    Decoder decoder(v.buf, v.size);
    decoder.skip_head_len();
    iter->second->propertys_unserialize(decoder);
}
