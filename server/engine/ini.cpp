#include "ini.h"

using namespace boost::property_tree;

void GIni::parser_ini(const char* file_name) {
    ini_parser::read_ini(file_name, m_root_node);
}

int GIni::get_int(const char* node_name, const char* child_name)
{
    ptree child_node = m_root_node.get_child(node_name);
    return child_node.get<int>(child_name);
}

GString GIni::get_string(const char* node_name, const char* child_name)
{
    ptree child_node = m_root_node.get_child(node_name);
    return child_node.get<GString>(child_name);
}

GIni g_ini;