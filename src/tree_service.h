#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <random>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include "trees/avl_tree.hpp"
#include "trees/bb_alpha_tree.hpp"
#include "trees/red_black_tree.hpp"
#include "trees/scapegoat_tree.hpp"
#include "trees/splay_tree.hpp"

#include "tree_visitor.h"
#include "json_serializer.hpp"

using json = nlohmann::json;

class TreeWrapper {
public:
    virtual ~TreeWrapper() = default;
    virtual void insert(int value) = 0;
    virtual void remove(int value) = 0;
    virtual json getJson() = 0;
    virtual std::string getType() const = 0;
};

template <typename TreeType>
class ConcreteTreeWrapper : public TreeWrapper {
private:
    TreeType tree_;
    std::string type_;
    
public:
    ConcreteTreeWrapper(const std::string& type) : type_(type) {}
    
    void insert(int value) override {
        tree_.insert(value);
    }
    
    void remove(int value) override {
        tree_.remove(value);
    }
    
    json getJson() override {
        JsonSerializer<int> serializer;
        tree_.accept(serializer);
        return serializer.getJson();
    }
    
    std::string getType() const override {
        return type_;
    }
};

class TreeFactory {
public:
    static std::unique_ptr<TreeWrapper> createTree(const std::string& treeType);
};

class TreeManager {
private:
    std::unordered_map<std::string, std::unique_ptr<TreeWrapper>> trees_;
    
    size_t current_id = 0;
    std::string generateId();
    
public:
    std::string createTree(const std::string& treeType);
    TreeWrapper* getTree(const std::string& id);
    bool removeTree(const std::string& id);
    json listTrees();
};

void setupTreeServer(httplib::Server& server, TreeManager& treeManager);
