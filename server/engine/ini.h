#pragma once

#include "boost/property_tree/ini_parser.hpp"
#include "boost/property_tree/ptree.hpp"

#include "gvalue.h"

using namespace std;

extern GString ini_file;

class GIni {
public:
    GIni() { 
        parser_ini(ini_file.c_str()); 
    }
    ~GIni() {}

    void parser_ini(const char* ini_file);
    int32_t get_int(const char* node_name, const char* child_name);
    float get_float(const char* node_name, const char* child_name);
    GString get_string(const char* node_name, const char* child_name);

private:
    boost::property_tree::ptree m_root_node;
};
