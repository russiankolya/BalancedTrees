#include <iostream>
#include <httplib.h>
#include "tree_visitor.h"
#include "trees/splay_tree.hpp"
#include "trees/avl_tree.hpp"
#include "trees/red_black_tree.hpp"
#include "json_serializer.hpp"
#include "tree_service.h"

int main() {
    TreeManager treeManager;
    httplib::Server server;

    setupTreeServer(server, treeManager);

    std::cout << "Server started on port 8080..." << std::endl;
    server.listen("0.0.0.0", 8080);
    
    return 0;
}
