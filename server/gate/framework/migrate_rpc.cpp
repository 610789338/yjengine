#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/entity.h"

void entity_property_migrate_from_oldcell(const GValue& new_cell_addr, const GValue& old_cell_addr, const GValue& entity_class_name, const GValue& create_data, const GValue& v) {
    auto remote = g_remote_mgr.get_remote(new_cell_addr.as_string());
    if (nullptr == remote) {
        return;
    }

    REMOTE_RPC_CALL(remote, "entity_property_migrate_from_oldcell",
        /*old cell addr*/old_cell_addr.as_string(),
        /*entity class name*/entity_class_name.as_string(),
        /*create data*/ create_data.as_dict(),
        /*entity property*/ v.as_bin());
}

void migrate_rpc_handle_regist() {
    RPC_REGISTER(entity_property_migrate_from_oldcell, GString(), GString(), GString(), GDict(), GBin());
}
