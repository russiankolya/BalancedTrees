#pragma once

#include "binary_search_tree.h"
#include <algorithm>
#include <cmath>

template <typename T>
class ScapegoatTree final : public BinarySearchTree<T> {
public:
    struct Node {
        T key;
        Node *left, *right;

        explicit Node(const T& key) : key(key), left(nullptr), right(nullptr) {}
    };

    ScapegoatTree(double alpha = 0.6) : root_(nullptr), size_(0), max_size_(0), alpha_(alpha) {}
    
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

        Node* current = root_;
        Node* parent = nullptr;
        std::vector<Node*> path;
        
        while (current != nullptr) {
            path.push_back(current);
            parent = current;
            if (value < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        
        Node* new_node = new Node(value);
        if (value < parent->key) {
            parent->left = new_node;
        } else {
            parent->right = new_node;
        }
        
        size_++;
        max_size_ = std::max(max_size_, size_);
        
        if (height(path.size()) > log_alpha(size_)) {
            Node* scapegoat = findScapegoat(path);
            if (scapegoat != nullptr) {
                rebuildSubtree(scapegoat, path);
            }
        }
    }
    
    void remove(const T &value) override {
        root_ = removeNode(root_, value);
        
        if (size_ < alpha_ * max_size_) {
            if (root_ != nullptr) {
                root_ = rebuildEntireTree();
            }
            max_size_ = size_;
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
    Node* root_;
    size_t size_;
    size_t max_size_;
    double alpha_;

    size_t subtreeSize(Node* node) const {
        if (node == nullptr) return 0;
        return 1 + subtreeSize(node->left) + subtreeSize(node->right);
    }

    double log_alpha(double n) const {
        return std::log(n) / std::log(1.0 / alpha_);
    }

    size_t height(size_t path_length) const {
        return path_length;
    }

    Node* findScapegoat(const std::vector<Node*>& path) {
        size_t depth = path.size() - 1;
        for (int i = depth - 1; i >= 0; i--) {
            Node* current = path[i];
            size_t size = subtreeSize(current);
            size_t child_size = subtreeSize(path[i+1]);
            
            if (child_size > alpha_ * size) {
                return current;
            }
        }
        return root_;
    }

    Node* flattenTree(Node* root, Node* tail) {
        if (root == nullptr) {
            return tail;
        }
        root->right = flattenTree(root->right, tail);
        return flattenTree(root->left, root);
    }

    Node* buildBalancedFromLinkedList(Node*& head, size_t size) {
        if (size == 0) return nullptr;
        
        size_t leftSize = size / 2;
        Node* leftSubtree = buildBalancedFromLinkedList(head, leftSize);
        
        Node* root = head;
        head = head->right;
        
        root->left = leftSubtree;
        
        root->right = buildBalancedFromLinkedList(head, size - leftSize - 1);
        
        return root;
    }

    void rebuildSubtree(Node* scapegoat, const std::vector<Node*>& path) {
        Node* parent = nullptr;
        bool is_left_child = false;
        
        for (size_t i = 0; i < path.size(); i++) {
            if (path[i] == scapegoat && i > 0) {
                parent = path[i-1];
                is_left_child = (parent->left == scapegoat);
                break;
            }
        }
        
        size_t subtree_size = subtreeSize(scapegoat);
        
        Node* list_head = flattenTree(scapegoat, nullptr);
        
        Node* new_subtree_root = buildBalancedFromLinkedList(list_head, subtree_size);
        
        if (parent == nullptr) {
            root_ = new_subtree_root;
        } else if (is_left_child) {
            parent->left = new_subtree_root;
        } else {
            parent->right = new_subtree_root;
        }
    }

    Node* rebuildEntireTree() {
        size_t tree_size = size_;
        Node* list_head = flattenTree(root_, nullptr);
        return buildBalancedFromLinkedList(list_head, tree_size);
    }

    Node* removeNode(Node* node, const T& value) {
        if (node == nullptr) return nullptr;
        
        if (value < node->key) {
            node->left = removeNode(node->left, value);
        } else if (value > node->key) {
            node->right = removeNode(node->right, value);
        } else {            
            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                size_--;
                return temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                size_--;
                return temp;
            }
            
            Node* temp = node->right;
            while (temp->left != nullptr) {
                temp = temp->left;
            }
            
            node->key = temp->key;
            node->right = removeNode(node->right, temp->key);
        }
        
        return node;
    }

    void destroyTree(Node* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
};
