#pragma once

#include "tree_visitor.h"
#include "binary_search_tree.h"
#include <algorithm>
#include <cmath>
#include <vector>

template <typename T>
class ScapegoatTree final : public BinarySearchTree<T> {
public:
    struct Node {
        T key;
        Node *left, *right;
        size_t size;

        explicit Node(const T& key) : key(key), left(nullptr), right(nullptr), size(1) {}
    };

    ScapegoatTree() : root_(nullptr), size_(0), max_size_(0) {}
    
    ~ScapegoatTree() {
        destroyTree(root_);
    }

    bool search(const T& value) override {
        Node* current = root_;
        while (current != nullptr) {
            if (value == current->key) {
                return true;
            } else if (value < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return false;
    }

    void insert(const T &value) override {
        if (root_ == nullptr) {
            root_ = new Node(value);
            size_ = 1;
            max_size_ = 1;
            return;
        }

        size_t depth = 0;
        bool inserted = false;
        std::vector<Node*> path;
        
        root_ = insertUtility(root_, value, depth, path, inserted);
        
        if (inserted) {
            size_++;
            max_size_ = std::max(max_size_, size_);
            
            double height_limit = std::log(size_) / std::log(1.0 / alpha);
            if (depth > static_cast<size_t>(height_limit)) {
                Node* scapegoat = nullptr;
                Node* scapegoat_parent = nullptr;
                
                for (size_t i = 0; i < path.size() - 1; i++) {
                    Node* child = path[i+1];
                    Node* parent = path[i];
                    
                    if (child->size > alpha * parent->size) {
                        scapegoat = child;
                        scapegoat_parent = parent;
                        break;
                    }
                }
                
                if (scapegoat == nullptr) {
                    root_ = rebuildSubtree(root_);
                } else {
                    if (scapegoat_parent->left == scapegoat) {
                        scapegoat_parent->left = rebuildSubtree(scapegoat);
                    } else {
                        scapegoat_parent->right = rebuildSubtree(scapegoat);
                    }
                    
                    for (int i = path.size() - 2; i >= 0; i--) {
                        updateSize(path[i]);
                    }
                }
            }
        }
    }
    
    void remove(const T &value) override {
        bool removed = false;
        root_ = removeUtility(root_, value, removed);
        
        if (removed) {
            size_--;
            if (size_ < alpha * max_size_) {
                root_ = rebuildSubtree(root_);
                max_size_ = size_;
            }
        }
    }

    Node* getRoot() const {
        return root_;
    }

    static std::string name() {
        return "Scapegoat Tree";
    }

    void accept(TreeVisitor<T>& visitor) const override {
        visitor.visit(*this);
    }

private:
    Node* root_ = nullptr;
    size_t size_;
    size_t max_size_;
    static constexpr double alpha = 2.0 / 3.0;

    size_t getSize(Node* node) const {
        return node ? node->size : 0;
    }

    void updateSize(Node* node) {
        if (node) {
            node->size = 1 + getSize(node->left) + getSize(node->right);
        }
    }

    Node* rebuildSubtree(Node* root) {
        std::vector<Node*> nodes;
        nodes.reserve(getSize(root));
        
        flattenTree(root, nodes);
        
        return buildBalancedTree(nodes, 0, nodes.size() - 1);
    }

    void flattenTree(Node* root, std::vector<Node*>& nodes) {
        if (!root) return;
        
        flattenTree(root->left, nodes);
        nodes.push_back(root);
        flattenTree(root->right, nodes);
    }

    Node* buildBalancedTree(std::vector<Node*>& nodes, int start, int end) {
        if (start > end) return nullptr;
        
        int mid = start + (end - start) / 2;
        Node* root = nodes[mid];
        
        root->left = buildBalancedTree(nodes, start, mid - 1);
        root->right = buildBalancedTree(nodes, mid + 1, end);
        
        updateSize(root);
        
        return root;
    }

    Node* insertUtility(Node* current, const T& value, size_t& depth, std::vector<Node*>& path, bool& inserted) {
        if (current == nullptr) {
            inserted = true;
            return new Node(value);
        }
        
        path.push_back(current);
        depth++;
        
        if (value < current->key) {
            current->left = insertUtility(current->left, value, depth, path, inserted);
        } else if (value > current->key) {
            current->right = insertUtility(current->right, value, depth, path, inserted);
        }
        
        if (inserted) {
            current->size++;
        }
        
        return current;
    }

    Node* removeUtility(Node* current, const T& value, bool& removed) {
        if (current == nullptr) return nullptr;
        
        if (value < current->key) {
            current->left = removeUtility(current->left, value, removed);
        } else if (value > current->key) {
            current->right = removeUtility(current->right, value, removed);
        } else {
            removed = true;
            
            if (current->left == nullptr) {
                Node* temp = current->right;
                delete current;
                return temp;
            } else if (current->right == nullptr) {
                Node* temp = current->left;
                delete current;
                return temp;
            } else {
                Node* successor = current->right;
                while (successor->left) successor = successor->left;
                
                current->key = successor->key;
                current->right = removeUtility(current->right, successor->key, removed);
            }
        }
        
        if (removed) {
            current->size--;
        }
        
        return current;
    }

    void destroyTree(Node* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
};
