#pragma once

#include "binary_search_tree.h"

template <typename T>
class SplayTree final : public BinarySearchTree<T> {
public:
    struct Node {
        T key;
        Node *left, *right, *parent;

        explicit Node(const T& key) 
            : key(key), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    SplayTree() : root_(nullptr) {}
    
    ~SplayTree() {
        destroyTree(root_);
    }

    bool search(const T& value) override {
        Node* node = findNode(value);
        if (node) {
            splay(node);
            return true;
        }
        return false;
    }

    void insert(const T &value) override {
        Node* newNode = new Node(value);
        if (!root_) {
            root_ = newNode;
            return;
        }

        Node* current = root_;
        Node* parent = nullptr;
        
        while (current) {
            parent = current;
            if (value < current->key) {
                current = current->left;
            } else if (value > current->key) {
                current = current->right;
            } else {
                delete newNode;
                splay(current);
                return;
            }
        }

        newNode->parent = parent;
        if (value < parent->key) {
            parent->left = newNode;
        } else {
            parent->right = newNode;
        }
        
        splay(newNode);
    }
    
    void remove(const T &value) override {
        Node* node = findNode(value);
        if (!node) return;
    
        splay(node);
    
        Node* toDelete = root_;
        Node* leftSubtree = root_->left;
        Node* rightSubtree = root_->right;
    
        if (leftSubtree) leftSubtree->parent = nullptr;
        if (rightSubtree) rightSubtree->parent = nullptr;
    
        delete toDelete;
        root_ = nullptr;
    
        if (leftSubtree) {
            root_ = leftSubtree;
            Node* maxNode = maximum(root_);
            splay(maxNode);
            root_->right = rightSubtree;
            if (rightSubtree) {
                rightSubtree->parent = root_;
            }
        } else {
            root_ = rightSubtree;
        }
    }

    Node* getRoot() const {
        return root_;
    }

    static std::string name() {
        return "Splay Tree";
    }

    void accept(TreeVisitor<T>& visitor) const override {
        visitor.visit(*this);
    }

private:
    Node* root_ = nullptr;

    Node* findNode(const T& value) {
        Node* current = root_;
        while (current) {
            if (value < current->key) {
                current = current->left;
            } else if (value > current->key) {
                current = current->right;
            } else {
                return current;
            }
        }
        return nullptr;
    }

    Node* maximum(Node* node) {
        while (node->right) {
            node = node->right;
        }
        return node;
    }

    void rotateLeft(Node* x) {
        Node* y = x->right;
        if (!y) return;
        
        x->right = y->left;
        if (y->left) {
            y->left->parent = x;
        }
        
        y->parent = x->parent;
        if (!x->parent) {
            root_ = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node* x) {
        Node* y = x->left;
        if (!y) return;
        
        x->left = y->right;
        if (y->right) {
            y->right->parent = x;
        }
        
        y->parent = x->parent;
        if (!x->parent) {
            root_ = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }
        
        y->right = x;
        x->parent = y;
    }

    void splay(Node* x) {
        while (x->parent) {
            Node* parent = x->parent;
            Node* grandparent = parent->parent;

            if (!grandparent) {
                if (x == parent->left) {
                    rotateRight(parent);
                } else {
                    rotateLeft(parent);
                }
            } else {
                bool isParentLeft = (parent == grandparent->left);
                bool isXLeft = (x == parent->left);

                if (isParentLeft == isXLeft) {
                    if (isXLeft) {
                        rotateRight(grandparent);
                        rotateRight(parent);
                    } else {
                        rotateLeft(grandparent);
                        rotateLeft(parent);
                    }
                } else {
                    if (isXLeft) {
                        rotateRight(parent);
                        rotateLeft(grandparent);
                    } else {
                        rotateLeft(parent);
                        rotateRight(grandparent);
                    }
                }
            }
        }
    }

    void destroyTree(Node* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
};
