#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/entity.h"

void entity_property_migrate_from_oldcell(const GValue& old_cell_addr, const GValue& entity_class_name, const GValue& _create_data, const GValue& v) {
    GDict& create_data = _create_data.as_dict();
    GString cell_entity_uuid = create_data.at("cell_entity_uuid").as_string();
    create_data.erase("cell_entity_uuid");

    Entity* entity = create_entity(entity_class_name.as_string(), cell_entity_uuid);
    Decoder decoder(v.as_bin().buf, v.as_bin().size);
    decoder.read_int16(); // skip pkg len offset
    entity->propertys_unserialize(decoder);
    entity->on_migrate_in(create_data);

    // notify old cell destroy
    CellMailBox old_cell;
    old_cell.set_side("server");
    old_cell.set_entity_and_addr(cell_entity_uuid, old_cell_addr.as_string());
    old_cell.set_owner(entity);
    old_cell.call("on_new_cell_migrate_finish");
}

void migrate_rpc_handle_regist() {
    RPC_REGISTER(entity_property_migrate_from_oldcell, GString(), GString(), GDict(), GBin());
}
