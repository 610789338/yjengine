#include "create_avatar_component.h"


void CreateAvatarComponent::component_rpc_test(const GString& msg) {
    INFO_LOG("[base] component_rpc_test.%s\n", msg.c_str());
}

void CreateAvatarComponent::component_timer_test(const GString& msg) {
    INFO_LOG("[base] component_timer_test.%s\n", msg.c_str());
}

void CreateAvatarComponent::component_event_test(const GString& msg) {
    INFO_LOG("[base] component_event_test.%s\n", msg.c_str());
}
