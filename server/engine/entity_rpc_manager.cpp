#include "entity_rpc_manager.h"

GArray* g_local_entity_rpc_names = nullptr;
unordered_map<GString, uint16_t> g_entity_rpc_name_l2s;
unordered_map<uint16_t, GString> g_entity_rpc_name_s2l;
