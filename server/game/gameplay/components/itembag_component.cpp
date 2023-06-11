#include "itembag_component.h"

#define ITEMBAG_1 101
#define ITEMBAG_2 102
#define IS_UNIQUE_ITEM(item_id) item_id < 100000
#define MAX_PILE_CNT 99

void ItemBagComponent::on_ready() {
    init_itembag();
}

void ItemBagComponent::init_itembag() {
    auto& item_bags = GET_PROP(itembags);
    if (!item_bags.empty()) {
        return;
    }

    ItemBag itembag1;
    itembag1.MEM(itembag_id).update(ITEMBAG_1);
    item_bags.insert(std::to_string(ITEMBAG_1), itembag1);

    ItemBag itembag2;
    itembag2.MEM(itembag_id).update(ITEMBAG_2);
    item_bags.insert(std::to_string(ITEMBAG_2), itembag2);

    add_item(ITEMBAG_1, 10010, 10);
    add_item(ITEMBAG_1, 100010, 10);
    add_item(ITEMBAG_1, 100011, 199);
    add_item(ITEMBAG_1, 100012, 199); 

    add_item(ITEMBAG_2, 10010, 10);
    add_item(ITEMBAG_2, 100010, 10);
    add_item(ITEMBAG_2, 100011, 199);
    add_item(ITEMBAG_2, 100012, 199);
}

void ItemBagComponent::add_item(int8_t itembag_id, int32_t item_id, int16_t num) {
    if (IS_UNIQUE_ITEM(item_id)) {
        add_uuid_item(itembag_id, item_id, num);
        return;
    }

    if (!GET_PROP(itembags).has(std::to_string(itembag_id))) {
        ERROR_LOG("itembag(%d) does not exist\n", itembag_id);
        return;
    }

    auto& item_bag = GET_PROP(itembags).GET(std::to_string(itembag_id));
    auto& item_list = item_bag.MEM(item_list);

    if (item_list.size() >= item_bag.MEM(item_limit).as_int16()) {
        ERROR_LOG("add item(%d) fail itembag(%d) full\n", item_id, itembag_id);
        return;
    }

    Item* item = nullptr;
    if (get_holdn_pile_item(itembag_id, item_id, &item)) {
        auto& pile_cnt = item->MEM(pile_cnt);
        auto new_num = pile_cnt.as_int16() + num;
        if (new_num > MAX_PILE_CNT) {
            add_new_pile_item(itembag_id, item_id, new_num - MAX_PILE_CNT);
            new_num = MAX_PILE_CNT;
        }
        pile_cnt.update(new_num);
    }
    else {
        add_new_pile_item(itembag_id, item_id, num);
    }
}

void ItemBagComponent::del_item(int8_t itembag_id, int32_t item_id, int16_t num, const GString& uuid) {
    if (IS_UNIQUE_ITEM(item_id)) {
        del_uuid_item(itembag_id, uuid);
        return;
    }
}

void ItemBagComponent::add_uuid_item(int8_t itembag_id, int32_t item_id, int16_t num) {
    auto& item_bag = GET_PROP(itembags).GET(std::to_string(itembag_id));
    auto& item_list = item_bag.MEM(item_list);

    for (int16_t idx = 0; idx < num; ++idx) {
        auto item_uuid = gen_uuid();

        Item item;
        item.MEM(uuid).update(item_uuid);
        item.MEM(item_id).update(item_id);
        item.MEM(pile_cnt).update(1);
        item_list.insert(item_uuid, item);
    }
}

void ItemBagComponent::del_uuid_item(int8_t itembag_id, const GString& uuid) {

}

bool ItemBagComponent::get_holdn_pile_item(int8_t itembag_id, int32_t item_id, Item** out_item) {
    auto& item_bag = GET_PROP(itembags).GET(std::to_string(itembag_id));
    auto& item_list = item_bag.MEM(item_list);

    auto item_uuids = item_list.keys();
    for (auto iter = item_uuids.begin(); iter != item_uuids.end(); ++iter) {
        auto& item = item_list.GET(*iter);
        if (item.MEM(item_id).as_int32() == item_id && item.MEM(pile_cnt).as_int16() < MAX_PILE_CNT) {
            *out_item = (Item*)&item;
            return true;
        }
    }

    return false;
}

void ItemBagComponent::add_new_pile_item(int8_t itembag_id, int32_t item_id, int16_t num) {
    int16_t extra_num = 0;
    if (num > MAX_PILE_CNT) {
        extra_num = num - MAX_PILE_CNT;
        num = MAX_PILE_CNT;
    }

    auto& item_bag = GET_PROP(itembags).GET(std::to_string(itembag_id));
    auto& item_list = item_bag.MEM(item_list);

    auto item_uuid = gen_uuid();

    Item item;
    item.MEM(uuid).update(item_uuid);
    item.MEM(item_id).update(item_id);
    item.MEM(pile_cnt).update(num);
    item_list.insert(item_uuid, item);

    if (extra_num > 0) {
        add_new_pile_item(itembag_id, item_id, extra_num);
    }
}
