#include "ini.h"

using namespace boost::property_tree;

GIni* g_ini = nullptr;

void GIni::parser_ini(const char* file_name) {
    ini_parser::read_ini(file_name, m_root_node);
}

int GIni::get_int(const char* node_name, const char* child_name) {
    ptree child_node = m_root_node.get_child(node_name);
    return child_node.get<int>(child_name);
}

GString GIni::get_string(const char* node_name, const char* child_name) {
    ptree child_node = m_root_node.get_child(node_name);
    return child_node.get<GString>(child_name);
}

int ini_get_int(const char* node_name, const char* child_name) {
    if (g_ini == nullptr) {
        g_ini = new GIni;
    }

    return g_ini->get_int(node_name, child_name);
}

GString ini_get_string(const char* node_name, const char* child_name) {
    if (g_ini == nullptr) {
        g_ini = new GIni;
    }

    return g_ini->get_string(node_name, child_name);
}

