#include "entity_rpc_manager.h"

unordered_map<GString, uint16_t> all_rpc_names_l2s;
unordered_map<uint16_t, GString> all_rpc_names_s2l;

GArray* get_local_entity_rpc_names() {
    // ȫ�ֱ������ڴ����͹��첻�߱�ԭ���ԣ��ڹ���֮ǰ���еĲ������ܵ�������
    // ����һ��ȫ��vector�����������ʱ�������ÿղ������������֮ǰ�����Ԫ����ô�ͻ������⣨shared_ptrҲ����ͬ�������⣩
    // ������ȫ��ָ�������������⣬new��ͬʱ�����ڴ��ҵ��ù��캯��
    // ����new�����Ķ���Ҫ�ֶ�delete����������ڴ�й¶
    // ���ԱȽ������ķ����Ǵ���static�ֲ����������������û���ָ��
    static GArray _local_entity_rpc_names;
    return &_local_entity_rpc_names;
}

