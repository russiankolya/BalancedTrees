#pragma once
#include <memory>

template <typename T>
class IBinarySearchTree {
public:
    virtual ~IBinarySearchTree() = default;

    // virtual bool Find(const T& value) const = 0;
    virtual void Insert(const T &value) = 0;
    // virtual void Remove(const T& value) = 0;
};
