#pragma once

#include <nlohmann/json.hpp>
#include "tree_visitor.h"

using json = nlohmann::json;

template <typename T>
class JsonSerializer : public TreeVisitor<T> {
public:
    JsonSerializer() = default;

    void visit(const AVLTree<T>& tree) override {
        json_ = {{"type", "avl_tree"}};
        json nodes = json::array();
        
        if (tree.getRoot()) {
            serializeNode(nodes, tree.getRoot(), [](auto* node, json& node_obj) {
                node_obj["height"] = node->height;
            });
        }
        json_["nodes"] = nodes;
    }

    void visit(const RedBlackTree<T>& tree) override {
        json_ = {{"type", "red_black_tree"}};
        json nodes = json::array();
        
        if (tree.getRoot()) {
            serializeNode(nodes, tree.getRoot(), [](auto* node, json& node_obj) {
                node_obj["color"] = node->color == RedBlackTree<T>::RED ? "red" : "black";
            });
        }
        json_["nodes"] = nodes;
    }

    void visit(const SplayTree<T>& tree) override {
        json_ = {{"type", "splay_tree"}};
        json nodes = json::array();
        
        if (tree.getRoot()) {
            serializeNode(nodes, tree.getRoot(), [](auto* node, json& node_obj) {});
        }
        json_["nodes"] = nodes;
    }

    void visit(const ScapegoatTree<T>& tree) override {
        json_ = {{"type", "scapegoat"}};
        json nodes = json::array();
        
        if (tree.getRoot()) {
            serializeNode(nodes, tree.getRoot(), [](auto* node, json& node_obj) {});
        }
        json_["nodes"] = nodes;
    }

    void visit(const BBAlphaTree<T>& tree) override {
        json_ = {{"type", "bb_alpha"}};
        json nodes = json::array();
        
        if (tree.getRoot()) {
            serializeNode(nodes, tree.getRoot(), [](auto* node, json& node_obj) {});
        }
        json_["nodes"] = nodes;
    }

    json getJson() const {
        return json_;
    }

private:
    json json_;

    template <typename NodeType, typename ProcessNodeFunc>
    int serializeNode(json& nodes, NodeType* node, ProcessNodeFunc processNode) {
        json node_obj;
        node_obj["key"] = node->key;
        
        processNode(node, node_obj);
        
        int current_id = nodes.size();

        nodes.push_back(node_obj);

        nodes[current_id]["left"] = node->left ? serializeNode(nodes, node->left, processNode) : -1;
        nodes[current_id]["right"] = node->right ? serializeNode(nodes, node->right, processNode) : -1;

        return current_id;
    }
};
