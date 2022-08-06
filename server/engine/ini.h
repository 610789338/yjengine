#pragma once

#include "boost/property_tree/ini_parser.hpp"
#include "boost/property_tree/ptree.hpp"

#include "log.h"
#include "gvalue.h"

using namespace std;

class GIni {
public:
    GIni() {}
    ~GIni() {}

    void parser_ini(const char* ini_file);
    int get_int(const char* node_name, const char* child_name);
    GString get_string(const char* node_name, const char* child_name);

private:
    boost::property_tree::ptree m_root_node;
};

extern GIni g_ini;
