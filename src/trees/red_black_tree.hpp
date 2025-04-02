#pragma once

#include "binary_search_tree.h"
#include <algorithm>

template <typename T>
class RedBlackTree final : public BinarySearchTree<T> {
public:
    enum Color { RED, BLACK };
    
    struct Node {
        T key;
        Node *left, *right, *parent;
        Color color;

        explicit Node(const T& key) 
            : key(key), left(nullptr), right(nullptr), parent(nullptr), color(RED) {}
    };

    RedBlackTree() : root_(nullptr) {}
    
    ~RedBlackTree() {
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
        Node* z = new Node(value);
        Node* y = nullptr;
        Node* x = root_;

        while (x != nullptr) {
            y = x;
            if (z->key < x->key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        z->parent = y;
        if (y == nullptr) {
            root_ = z;
        } else if (z->key < y->key) {
            y->left = z;
        } else {
            y->right = z;
        }

        insertFixup(z);
    }
    
    void remove(const T &value) override {
        Node* z = root_;
        while (z != nullptr) {
            if (value == z->key) {
                break;
            } else if (value < z->key) {
                z = z->left;
            } else {
                z = z->right;
            }
        }

        if (z == nullptr) return;

        Node* y = z;
        Node* x = nullptr;
        Node* x_parent = nullptr;
        Color y_original_color = y->color;

        if (z->left == nullptr) {
            x = z->right;
            x_parent = z->parent;
            transplant(z, z->right);
        } else if (z->right == nullptr) {
            x = z->left;
            x_parent = z->parent;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            
            if (y->parent == z) {
                if (x != nullptr) {
                    x->parent = y;
                }
                x_parent = y;
            } else {
                x_parent = y->parent;
                transplant(y, y->right);
                y->right = z->right;
                if (y->right != nullptr) {
                    y->right->parent = y;
                }
            }
            
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;
        
        if (y_original_color == BLACK) {
            deleteFixup(x, x_parent);
        }
    }

    Node* getRoot() const {
        return root_;
    }

    static std::string name() {
        return "Red-Black Tree";
    }

    void accept(TreeVisitor<T>& visitor) const override {
        visitor.visit(*this);
    }

private:
    Node* root_ = nullptr;
    
    void leftRotate(Node* x) {
        if (x == nullptr || x->right == nullptr) return;
        
        Node* y = x->right;
        x->right = y->left;
        
        if (y->left != nullptr) {
            y->left->parent = x;
        }
        
        y->parent = x->parent;
        
        if (x->parent == nullptr) {
            root_ = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
    }
    
    void rightRotate(Node* y) {
        if (y == nullptr || y->left == nullptr) return;
        
        Node* x = y->left;
        y->left = x->right;
        
        if (x->right != nullptr) {
            x->right->parent = y;
        }
        
        x->parent = y->parent;
        
        if (y->parent == nullptr) {
            root_ = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
        
        x->right = y;
        y->parent = x;
    }
    
    void insertFixup(Node* z) {
        while (z != root_ && z->parent != nullptr && z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                
                if (y != nullptr && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                
                if (y != nullptr && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        
        root_->color = BLACK;
    }
    
    void deleteFixup(Node* x, Node* x_parent) {
        while (x != root_ && (x == nullptr || x->color == BLACK)) {
            if (x_parent == nullptr) break;
            
            if (x == x_parent->left) {
                Node* w = x_parent->right;
                
                if (w != nullptr && w->color == RED) {
                    w->color = BLACK;
                    x_parent->color = RED;
                    leftRotate(x_parent);
                    w = x_parent->right;
                }
                
                if (w == nullptr || 
                    ((w->left == nullptr || w->left->color == BLACK) && 
                     (w->right == nullptr || w->right->color == BLACK))) {
                    if (w != nullptr) w->color = RED;
                    x = x_parent;
                    x_parent = x->parent;
                } else {
                    if (w->right == nullptr || w->right->color == BLACK) {
                        if (w->left != nullptr) w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x_parent->right;
                    }
                    
                    if (w != nullptr) {
                        w->color = x_parent->color;
                        if (w->right != nullptr) w->right->color = BLACK;
                    }
                    x_parent->color = BLACK;
                    leftRotate(x_parent);
                    x = root_;
                }
            } else {
                Node* w = x_parent->left;
                
                if (w != nullptr && w->color == RED) {
                    w->color = BLACK;
                    x_parent->color = RED;
                    rightRotate(x_parent);
                    w = x_parent->left;
                }
                
                if (w == nullptr || 
                    ((w->right == nullptr || w->right->color == BLACK) && 
                     (w->left == nullptr || w->left->color == BLACK))) {
                    if (w != nullptr) w->color = RED;
                    x = x_parent;
                    x_parent = x->parent;
                } else {
                    if (w->left == nullptr || w->left->color == BLACK) {
                        if (w->right != nullptr) w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x_parent->left;
                    }
                    
                    if (w != nullptr) {
                        w->color = x_parent->color;
                        if (w->left != nullptr) w->left->color = BLACK;
                    }
                    x_parent->color = BLACK;
                    rightRotate(x_parent);
                    x = root_;
                }
            }
        }
        
        if (x != nullptr) x->color = BLACK;
    }
    
    void transplant(Node* u, Node* v) {
        if (u->parent == nullptr) {
            root_ = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        
        if (v != nullptr) {
            v->parent = u->parent;
        }
    }
    
    Node* minimum(Node* node) const {
        while (node->left != nullptr) {
            node = node->left;
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
