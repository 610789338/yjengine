#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/entity.h"

void entity_property_migrate_from_oldcell(const GString& new_cell_addr, const GString& old_cell_addr, const GString& entity_class_name, const GDict& migrate_data, const GBin& property_bin) {
    auto remote = g_remote_mgr.get_remote(new_cell_addr);
    if (nullptr == remote) {
        return;
    }

    REMOTE_RPC_CALL(remote, "entity_property_migrate_from_oldcell",
        /*old cell addr*/old_cell_addr,
        /*entity class name*/entity_class_name,
        /*create data*/ migrate_data,
        /*entity property*/ property_bin);
}

void migrate_rpc_handle_regist() {
    RPC_REGISTER(entity_property_migrate_from_oldcell);
}
