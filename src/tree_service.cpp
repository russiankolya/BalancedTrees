#include "tree_service.h"
#include "tree_visitor.h"
#include <filesystem>
#include <string>

std::unique_ptr<TreeWrapper> TreeFactory::createTree(const std::string& treeType) {
    if (treeType == "avl") {
        return std::make_unique<ConcreteTreeWrapper<AVLTree<int>>>("avl");
    } else if (treeType == "splay") {
        return std::make_unique<ConcreteTreeWrapper<SplayTree<int>>>("splay");
    } else if (treeType == "red_black") {
        return std::make_unique<ConcreteTreeWrapper<RedBlackTree<int>>>("red_black");
    } else if (treeType == "scapegoat") {
        return std::make_unique<ConcreteTreeWrapper<ScapegoatTree<int>>>("scapegoat");
    } else if (treeType == "bb_alpha") {
        return std::make_unique<ConcreteTreeWrapper<BBAlphaTree<int>>>("bb_alpha");
    }
    
    throw std::invalid_argument("Unsupported tree type: " + treeType);
}

std::string TreeManager::generateId() {
    return std::to_string(++current_id);
}

std::string TreeManager::createTree(const std::string& treeType) {
    std::string id = generateId();
    trees_[id] = TreeFactory::createTree(treeType);
    return id;
}

TreeWrapper* TreeManager::getTree(const std::string& id) {
    auto it = trees_.find(id);
    if (it == trees_.end()) {
        return nullptr;
    }
    return it->second.get();
}

bool TreeManager::removeTree(const std::string& id) {
    return trees_.erase(id) > 0;
}

json TreeManager::listTrees() {
    json result = json::array();
    for (const auto& [id, tree] : trees_) {
        result.push_back({
            {"id", id},
            {"type", tree->getType()}
        });
    }
    return result;
}

void setupTreeServer(httplib::Server& server, TreeManager& treeManager) {
    server.set_mount_point("/", "./static");
    
    server.Post("/trees", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto reqJson = json::parse(req.body);
            
            if (!reqJson.contains("type")) {
                res.status = 400;
                res.set_content(json{{"error", "Tree type is required"}}.dump(), "application/json");
                return;
            }
            
            std::string treeType = reqJson["type"];
            std::string treeId = treeManager.createTree(treeType);
            
            res.set_content(json{{"id", treeId}, {"type", treeType}}.dump(), "application/json");
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });
    
    server.Get(R"(/trees/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        TreeWrapper* tree = treeManager.getTree(id);
        
        if (!tree) {
            res.status = 404;
            res.set_content(json{{"error", "Tree not found"}}.dump(), "application/json");
            return;
        }
        
        json treeJson = tree->getJson();
        res.set_content(treeJson.dump(), "application/json");
    });
    
    server.Post(R"(/trees/([^/]+)/insert)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string id = req.matches[1];
            TreeWrapper* tree = treeManager.getTree(id);
            
            if (!tree) {
                res.status = 404;
                res.set_content(json{{"error", "Tree not found"}}.dump(), "application/json");
                return;
            }
            
            auto reqJson = json::parse(req.body);
            
            if (!reqJson.contains("value")) {
                res.status = 400;
                res.set_content(json{{"error", "Value is required"}}.dump(), "application/json");
                return;
            }
            
            int value = reqJson["value"];
            tree->insert(value);
            
            res.set_content(json{{"success", true}}.dump(), "application/json");
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });
    
    server.Post(R"(/trees/([^/]+)/remove)", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string id = req.matches[1];
            TreeWrapper* tree = treeManager.getTree(id);
            
            if (!tree) {
                res.status = 404;
                res.set_content(json{{"error", "Tree not found"}}.dump(), "application/json");
                return;
            }
            
            auto reqJson = json::parse(req.body);
            
            if (!reqJson.contains("value")) {
                res.status = 400;
                res.set_content(json{{"error", "Value is required"}}.dump(), "application/json");
                return;
            }
            
            int value = reqJson["value"];
            tree->remove(value);
            
            res.set_content(json{{"success", true}}.dump(), "application/json");
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });
    
    server.Get("/trees", [&](const httplib::Request& req, httplib::Response& res) {
        json treesList = treeManager.listTrees();
        res.set_content(treesList.dump(), "application/json");
    });
    
    server.Delete(R"(/trees/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string id = req.matches[1];
        
        if (treeManager.removeTree(id)) {
            res.set_content(json{{"success", true}}.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content(json{{"error", "Tree not found"}}.dump(), "application/json");
        }
    });

    std::cout << "Serving static files from: " << std::filesystem::absolute("./static").string() << std::endl;
}
