#pragma once

template <typename T> class AVLTree;
template<typename T> class RedBlackTree;
template <typename T> class SplayTree;
template <typename T> class ScapegoatTree;
template <typename T> class BBAlphaTree;

template <typename T>
class TreeVisitor {
public:
    virtual void visit(const AVLTree<T>& tree) = 0;
    virtual void visit(const BBAlphaTree<T>& tree) = 0;
    virtual void visit(const RedBlackTree<T>& tree) = 0;
    virtual void visit(const ScapegoatTree<T>& tree) = 0;
    virtual void visit(const SplayTree<T>& tree) = 0;

    virtual ~TreeVisitor() = default;
};