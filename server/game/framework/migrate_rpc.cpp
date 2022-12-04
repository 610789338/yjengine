#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/entity.h"

void entity_property_migrate_from_oldcell(GString old_cell_addr, GString entity_class_name, GDict _create_data, GBin v) {
    GDict& create_data = _create_data;
    GString entity_uuid = create_data.at("entity_uuid").as_string();
    create_data.erase("entity_uuid");

    Entity* entity = create_local_cell_entity(entity_class_name, entity_uuid);
    Decoder decoder(v.buf, v.size);
    decoder.skip_head_len();
    entity->propertys_unserialize(decoder);
    entity->on_migrate_in(create_data);

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
