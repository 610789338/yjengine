#include "entity_rpc_manager.h"

unordered_map<GString, uint16_t> all_rpc_names_l2s;
unordered_map<uint16_t, GString> all_rpc_names_s2l;

GArray* get_local_entity_rpc_names() {
    // 全局变量的内存分配和构造不具备原子性，在构造之前进行的操作可能导致问题
    // 比如一个全局vector变量，构造的时候会进行置空操作，如果构造之前就添加元素那么就会有问题（shared_ptr也会有同样的问题）
    // 可以用全局指针来解决这个问题，new是同时分配内存且调用构造函数
    // 但是new出来的对象要手动delete，否则会有内存泄露
    // 所以比较完美的方法是创建static局部变量，返回其引用或者指针
    static GArray _local_entity_rpc_names;
    return &_local_entity_rpc_names;
}

