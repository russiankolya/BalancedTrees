#pragma once

#include "binary_search_tree.h"
#include <algorithm>

template <typename T>
class AVLTree final : public BinarySearchTree<T> {
public:
    struct Node {
        T key;
        Node *left, *right;
        size_t height;

        explicit Node(const T& key) : key(key), left(nullptr), right(nullptr), height(1) {}
    };

    AVLTree() : root_(nullptr) {}
    
    ~AVLTree() {
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
        return "AVL Tree";
    }

    void accept(TreeVisitor<T>& visitor) const override {
        visitor.visit(*this);
    }

private:
    Node* root_ = nullptr;

    size_t getHeight(Node* node) const {
        if (node == nullptr) {
            return 0;
        }
        return node->height;
    }

    void updateHeight(Node* node) {
        if (node == nullptr) {
            return;
        }
        node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
    }

    int getBalance(Node* node) const {
        if (node == nullptr) {
            return 0;
        }
        return getHeight(node->left) - getHeight(node->right);
    }

    Node* rotateRight(Node* node) {
        if (node == nullptr || node->left == nullptr) {
            return node;
        }

        Node* left_child = node->left;
        Node* left_right_child = left_child->right;

        node->left = left_right_child;
        left_child->right = node;

        updateHeight(node);
        updateHeight(left_child);

        return left_child;
    }

    Node* rotateLeft(Node* node) {
        Node* right_child = node->right;
        Node* right_left_child = right_child->left;

        node->right = right_left_child;
        right_child->left = node;

        updateHeight(node);
        updateHeight(right_child);

        return right_child;
    }

    Node* insertUtility(Node* current, const T& value) {
        if (current == nullptr) {
            return new Node(value);
        }

        if (value < current->key) {
            current->left = insertUtility(current->left, value);
        } else {
            current->right = insertUtility(current->right, value);
        }

        updateHeight(current);
        int balance = getBalance(current);

        if (balance > 1 && value < current->left->key) {
            return rotateRight(current);
        }
        if (balance < -1 && value > current->right->key) {
            return rotateLeft(current);
        }
        if (balance > 1 && value > current->left->key) {
            current->left = rotateLeft(current->left);
            return rotateRight(current);
        }
        if (balance < -1 && value < current->right->key) {
            current->right = rotateRight(current->right);
            return rotateLeft(current);
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
            if (current->right == nullptr || current->left == nullptr) {
                Node* temp = current->left ? current->left : current->right;
                
                if (temp == nullptr) {
                    temp = current;
                    current = nullptr;
                } else {
                    *current = *temp;
                }
                delete temp;
            } else {
                Node* temp = findMinValueNode(current->right);
                current->key = temp->key;
                current->right = removeUtility(current->right, temp->key);
            }
        }
        
        if (current == nullptr) {
            return nullptr;
        }

        updateHeight(current);
        int balance = getBalance(current);

        if (balance > 1 && getBalance(current->left) >= 0) {
            return rotateRight(current);
        }
        if (balance < -1 && getBalance(current->right) <= 0) {
            return rotateLeft(current);
        }
        if (balance > 1 && getBalance(current->left) < 0) {
            current->left = rotateLeft(current->left);
            return rotateRight(current);
        }
        if (balance < -1 && getBalance(current->right) > 0) {
            current->right = rotateRight(current->right);
            return rotateLeft(current);
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
