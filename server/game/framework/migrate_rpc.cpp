#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/entity.h"

void entity_property_migrate_from_oldcell(const GString& old_cell_addr, const GString& entity_class_name, const GDict& _migrate_data, const GBin& property_bin) {
    GDict& migrate_data = (GDict&)_migrate_data;
    GString entity_uuid = migrate_data.at("entity_uuid").as_string();
    migrate_data.erase("entity_uuid");

    Entity* entity = create_local_cell_entity(entity_class_name, entity_uuid);
    Decoder decoder(property_bin.buf, property_bin.size);
    decoder.skip_head_len();
    entity->propertys_unserialize(decoder);
    entity->on_migrate_in(migrate_data);

    // notify old cell destroy
    CellMailBox old_cell;
    old_cell.set_side("server");
    old_cell.set_entity_and_addr(entity_uuid, old_cell_addr);
    old_cell.set_owner(entity);
    old_cell.call("on_new_cell_migrate_finish");
}

void migrate_rpc_handle_regist() {
    RPC_REGISTER(entity_property_migrate_from_oldcell);
}
