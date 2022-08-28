#include "entity_property_tree.h"
#include "entity_property_manager.h"

void PropertyTree::generate_property_tree(unordered_map<GString, EntityPropertyBase*>& propertys) {
    for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {
        root.child_node.push_back(PropertyTreeNode(iter->first));
        generate_node(iter->second, root.child_node[root.child_node.size() - 1]);
    }
}

void PropertyTree::generate_node(EntityPropertyBase* property, PropertyTreeNode& tree_node) {
    if (property->is_complex()) {
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        unordered_map<GString, uint8_t>& prop_idxs = all_prop_idxs.at(property->get_pclass_name());
        for (auto iter = prop_idxs.begin(); iter != prop_idxs.end(); ++iter) {
            tree_node.child_node.push_back(PropertyTreeNode(iter->first));
            generate_node(property->get_propertys()[iter->second], tree_node.child_node[tree_node.child_node.size() - 1]);
        }
    }

    if (property->is_array() && property->is_value_complex()) {
        tree_node.child_node.push_back(PropertyTreeNode("array"));
        EntityPropertyBase* value_prop = property->create_array_value();
        generate_node(value_prop, tree_node.child_node[tree_node.child_node.size() - 1]);
        delete value_prop;
    }

    if (property->is_map() && property->is_value_complex()) {
        tree_node.child_node.push_back(PropertyTreeNode("map"));
        EntityPropertyBase* value_prop = property->create_map_value();
        generate_node(value_prop, tree_node.child_node[tree_node.child_node.size() - 1]);
        delete value_prop;
    }
}
