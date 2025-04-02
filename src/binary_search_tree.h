#pragma once

#include "tree_visitor.h"
#include <string>

template <typename T>
class BinarySearchTree {
public:
    virtual ~BinarySearchTree() = default;

    virtual bool search(const T& value) = 0;
    virtual void insert(const T &value) = 0;
    virtual void remove(const T& value) = 0;

    virtual void accept(TreeVisitor<T>& visitor) const = 0;
};
