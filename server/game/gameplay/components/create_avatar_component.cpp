#include "engine/engine.h"

#include "create_avatar_component.h"

void CreateAvatarComponent::component_rpc_test(const GValue& msg) {
    INFO_LOG("[base] component_rpc_test.%s\n", msg.as_string().c_str());
}
