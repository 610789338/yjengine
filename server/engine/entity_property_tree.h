#pragma once

#include <vector>

#include "gvalue.h"

using namespace std;

struct EntityPropertyBase;

struct PropertyTreeNode {
    PropertyTreeNode() = delete;
    PropertyTreeNode(GString _name) : name(_name) {}
    ~PropertyTreeNode() {}

    GString name;
    vector<PropertyTreeNode> child_node;
};

struct PropertyTree {
    PropertyTree() = delete;
    PropertyTree(unordered_map<GString, EntityPropertyBase*>& propertys) : root("root") {
        generate_property_tree(propertys);
    }
    ~PropertyTree() {}

    void generate_property_tree(unordered_map<GString, EntityPropertyBase*>& propertys);
    void generate_node(EntityPropertyBase* property, PropertyTreeNode& tree_node);

    PropertyTreeNode root;
};
