//#include "boost/exception/detail/exception_ptr.hpp"
#include "ini.h"

using namespace boost::property_tree;


GIni* get_ini() {
    static GIni _ini;
    return &_ini;
}

void GIni::parser_ini(const char* file_name) {
    ini_parser::read_ini(file_name, m_root_node);
}

int32_t GIni::get_int(const char* node_name, const char* child_name) {
    ptree child_node = m_root_node.get_child(node_name);
    return child_node.get<int32_t>(child_name);
}

float GIni::get_float(const char* node_name, const char* child_name) {
    ptree child_node = m_root_node.get_child(node_name);
    return child_node.get<float>(child_name);
}

GString GIni::get_string(const char* node_name, const char* child_name) {
    ptree child_node = m_root_node.get_child(node_name);
    return child_node.get<GString>(child_name);
}

int32_t ini_get_int(const char* node_name, const char* child_name, int32_t _default/* = 0xF0F00F0F */) {
    try {
        return get_ini()->get_int(node_name, child_name);
    }
    catch (std::exception& e) {
        if (_default == 0xF0F00F0F) {
            ASSERT_LOG(false, "%s\n", e.what());
        }
        return _default;
    }
}

float ini_get_float(const char* node_name, const char* child_name, float _default/* = 0xF0F00F0F */) {
    try {
        return get_ini()->get_float(node_name, child_name);
    }
    catch (std::exception& e) {
        if (_default == 0xF0F00F0F) {
            ASSERT_LOG(false, "%s\n", e.what());
        }
        return _default;
    }
}

GString ini_get_string(const char* node_name, const char* child_name, GString _default/* = "0xF0F00F0F" */) {
    try {
        return get_ini()->get_string(node_name, child_name);
    }
    catch (std::exception& e) {
        if (_default == "0xF0F00F0F") {
            ASSERT_LOG(false, "%s\n", e.what());
        }
        return _default;
    }
}
