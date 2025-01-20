#pragma once

#include "IBinarySearchTree.h"

template <typename T>
class AVLTree final : public IBinarySearchTree<T> {
public:
    void Insert(const T &value) override;
    void Remove(const T &value) override;

    struct Node {
        T key;
        Node *left, *right;
        size_t height;

        explicit Node(const T& key);
    };
private:
    Node* root_ = nullptr;
};

template class AVLTree<int>;