#include "engine/engine.h"

#include "create_avatar_component.h"

void CreateAvatarComponent::component_rpc_test(const GValue& msg) {

    INFO_LOG("[base] component_rpc_test.%s\n", msg.as_string().c_str());

    property_test();
}

void CreateAvatarComponent::property_test() {
    owner->get_prop("avatar_datas")->get("avatar_idx")->push_back(int32_t(10));
    owner->get_prop("avatar_datas")->get("avatar_idx")->push_back(int32_t(11));
    owner->get_prop("avatar_datas")->get("avatar_idx")->push_back(int32_t(12));

    auto ele0 = owner->get_prop("avatar_datas")->get("avatar_idx")->get(0);
    auto ele1 = owner->get_prop("avatar_datas")->get("avatar_idx")->get(1);
    auto ele2 = owner->get_prop("avatar_datas")->get("avatar_idx")->get(2);

    INFO_LOG("ele0.%d - ele1.%d - ele2.%d\n", ele0->get().as_int32(), ele1->get().as_int32(), ele2->get().as_int32());
}
