#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/entity.h"

void entity_property_migrate_from_oldcell(GString new_cell_addr, GString old_cell_addr, GString entity_class_name, GDict create_data, GBin v) {
    auto remote = g_remote_mgr.get_remote(new_cell_addr);
    if (nullptr == remote) {
        return;
    }

    REMOTE_RPC_CALL(remote, "entity_property_migrate_from_oldcell",
        /*old cell addr*/old_cell_addr,
        /*entity class name*/entity_class_name,
        /*create data*/ create_data,
        /*entity property*/ v);
}

void migrate_rpc_handle_regist() {
    RPC_REGISTER(entity_property_migrate_from_oldcell);
}
