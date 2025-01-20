#include "AVLTree.h"

#include <algorithm>

template <typename T>
AVLTree<T>::Node::Node(const T &key) : key(key), left(nullptr), right(nullptr), height(1) {}

template <typename T>
using NodePtr = typename AVLTree<T>::Node*;

template <typename T>
size_t GetHeight(NodePtr<T> node) {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

template <typename T>
void UpdateHeight(NodePtr<T> node) {
    if (node == nullptr) {
        return;
    }
    node->height = std::max(GetHeight<T>(node->left), GetHeight<T>(node->right)) + 1;
}

template <typename T>
int GetBalance(NodePtr<T> node) {
    if (node == nullptr) {
        return 0;
    }
    return GetHeight<T>(node->left) - GetHeight<T>(node->right);
}

template <typename T>
NodePtr<T> RotateRight(NodePtr<T> node) {
    NodePtr<T> left_child = node->left;
    NodePtr<T> left_right_child = left_child->right;

    node->left = left_right_child;
    left_child->right = node;

    UpdateHeight<T>(left_child);
    UpdateHeight<T>(node);

    return left_child;
}

template <typename T>
NodePtr<T> RotateLeft(NodePtr<T> node) {
    NodePtr<T> right_child = node->right;
    NodePtr<T> right_left_child = right_child->left;

    node->right = right_left_child;
    right_child->left = node;

    UpdateHeight<T>(right_child);
    UpdateHeight<T>(node);

    return right_child;
}

template <typename T>
NodePtr<T> InsertUtility(NodePtr<T> &current, const T& value) {
    if (current == nullptr) {
        const NodePtr<T> new_node = new typename AVLTree<T>::Node(value);
        return new_node;
    }

    if (value < current->key) {
        current->left = InsertUtility<T>(current->left, value);
    } else {
        current->right = InsertUtility<T>(current->right, value);
    }

    UpdateHeight<T>(current);
    auto balance = GetBalance<T>(current);

    if (balance > 1 && value < current->left->key) {
        return RotateRight<T>(current);
    }
    if (balance < -1 && value > current->right->key) {
        return RotateLeft<T>(current);
    }
    if (balance > 1 && value > current->left->key) {
        current->left = RotateLeft<T>(current->left);
        return RotateRight<T>(current);
    }
    if (balance < -1 && value < current->right->key) {
        current->right = RotateRight<T>(current->right);
        return RotateLeft<T>(current);
    }
    return current;
}

template <typename T>
void AVLTree<T>::Insert(const T &value) {
    if (root_ == nullptr) {
        root_ = new Node(value);
        return;
    }
    root_ = InsertUtility<T>(root_, value);
}

template <typename T>
NodePtr<T> FindMinValueNode(NodePtr<T> node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

template <typename T>
NodePtr<T> RemoveUtility(NodePtr<T> current, const T& value) {
    if (current == nullptr) {
        return nullptr;
    }
    if (value < current->key) {
        current->left = RemoveUtility(current->left, value);
    } else if (value > current->key) {
        current->right = RemoveUtility(current->right, value);
    } else {
        if (current->right == nullptr || current->left == nullptr) {
            NodePtr<T> temp;
            if (current->right == nullptr) {
                temp = current->left;
            } else {
                temp = current->right;
            }
            if (temp == nullptr) {
                temp = current;
                current = nullptr;
            } else {
                *current = std::move(*temp);
            }
            delete temp;
        } else {
            NodePtr<T> temp = FindMinValueNode<T>(current->right);
            current->key = temp->key;
            current->right = RemoveUtility(current->right, temp->key);
        }
    }
    if (current == nullptr) {
        return nullptr;
    }

    UpdateHeight<T>(current);
    int balance = GetBalance<T>(current);
    if (balance > 1 && GetBalance<T>(current->left) >= 0) {
        return RotateRight<T>(current);
    }
    if (balance < -1 && GetBalance<T>(current->right) <= 0) {
        return RotateLeft<T>(current);
    }
    if (balance > 1 && GetBalance<T>(current->left) <= 0) {
        current->left = RotateLeft<T>(current->left);
        return RotateRight<T>(current);
    }
    if (balance < -1 && GetBalance<T>(current->right) >= 0) {
        current->right = RotateRight<T>(current->right);
        return RotateLeft<T>(current);
    }
    return current;
}

template <typename T>
void AVLTree<T>::Remove(const T &value) {
    root_ = RemoveUtility(root_, value);
}
