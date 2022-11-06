#include "engine/engine.h"

#include "create_avatar_component.h"

void CreateAvatarComponent::component_rpc_test(GString msg) {

    INFO_LOG("[base] component_rpc_test.%s\n", msg.c_str());
}
