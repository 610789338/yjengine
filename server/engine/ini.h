#pragma once

#include "boost/property_tree/ini_parser.hpp"
#include "boost/property_tree/ptree.hpp"

#include "log.h"
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
    int get_int(const char* node_name, const char* child_name);
    GString get_string(const char* node_name, const char* child_name);

private:
    boost::property_tree::ptree m_root_node;
};

extern int ini_get_int(const char* node_name, const char* child_name);
extern GString ini_get_string(const char* node_name, const char* child_name);
