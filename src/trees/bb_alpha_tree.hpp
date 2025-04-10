#pragma once

#include "tree_visitor.h"
#include "binary_search_tree.h"
#include <algorithm>
#include <vector>

template <typename T>
class BBAlphaTree final : public BinarySearchTree<T> {
public:
    struct Node {
        T key;
        Node *left, *right;
        size_t size;

        explicit Node(const T& key) : key(key), left(nullptr), right(nullptr), size(1) {}
    };

    BBAlphaTree(double alpha = 0.25) : root_(nullptr), alpha_(alpha) {}
    
    ~BBAlphaTree() {
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
            return;
        }
        root_ = insertUtility(root_, value);
    }
    
    void remove(const T &value) override {
        root_ = removeUtility(root_, value);
    }

    Node* getRoot() const {
        return root_;
    }

    static std::string name() {
        return "BB-alpha Tree";
    }

    void accept(TreeVisitor<T>& visitor) const override {
        visitor.visit(*this);
    }

private:
    Node* root_ = nullptr;
    double alpha_;

    size_t getSize(Node* node) const {
        if (node == nullptr) {
            return 0;
        }
        return node->size;
    }

    void updateSize(Node* node) {
        if (node == nullptr) {
            return;
        }
        node->size = 1 + getSize(node->left) + getSize(node->right);
    }

    bool isBalanced(Node* node) const {
        if (node == nullptr) {
            return true;
        }
        
        size_t leftSize = getSize(node->left);
        size_t rightSize = getSize(node->right);
        size_t totalSize = leftSize + rightSize + 1;
        
        if (leftSize < alpha_ * totalSize || leftSize > (1 - alpha_) * totalSize) {
            return false;
        }
        
        if (rightSize < alpha_ * totalSize || rightSize > (1 - alpha_) * totalSize) {
            return false;
        }
        
        return true;
    }

    Node* rebuildSubtree(Node* root) {
        std::vector<Node*> nodes;
        flattenTree(root, nodes);
        return buildBalancedTree(nodes, 0, nodes.size() - 1);
    }

    void flattenTree(Node* root, std::vector<Node*>& nodes) {
        if (root == nullptr) {
            return;
        }
        flattenTree(root->left, nodes);
        nodes.push_back(root);
        flattenTree(root->right, nodes);
        
        root->left = nullptr;
        root->right = nullptr;
    }

    Node* buildBalancedTree(std::vector<Node*>& nodes, int start, int end) {
        if (start > end) {
            return nullptr;
        }
        
        int mid = (start + end) / 2;
        Node* root = nodes[mid];
        
        root->left = buildBalancedTree(nodes, start, mid - 1);
        root->right = buildBalancedTree(nodes, mid + 1, end);
        
        updateSize(root);
        return root;
    }

    Node* insertUtility(Node* current, const T& value) {
        if (current == nullptr) {
            return new Node(value);
        }
        
        if (value < current->key) {
            current->left = insertUtility(current->left, value);
        } else if (value > current->key) {
            current->right = insertUtility(current->right, value);
        } else {
            return current;
        }
        
        updateSize(current);
        
        if (!isBalanced(current)) {
            return rebuildSubtree(current);
        }
        
        return current;
    }

    Node* findMinValueNode(Node* node) const {
        Node* current = node;
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    Node* removeUtility(Node* current, const T& value) {
        if (current == nullptr) {
            return nullptr;
        }
        
        if (value < current->key) {
            current->left = removeUtility(current->left, value);
        } else if (value > current->key) {
            current->right = removeUtility(current->right, value);
        } else {
            
            if (current->left == nullptr) {
                Node* temp = current->right;
                delete current;
                return temp;
            } else if (current->right == nullptr) {
                Node* temp = current->left;
                delete current;
                return temp;
            }
            
            Node* temp = findMinValueNode(current->right);
            current->key = temp->key;
            current->right = removeUtility(current->right, temp->key);
        }
        
        updateSize(current);
        
        if (!isBalanced(current)) {
            return rebuildSubtree(current);
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
