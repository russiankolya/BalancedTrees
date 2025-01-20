#include "AVLTree.h"

int main() {
    AVLTree<int> tree;
    tree.Insert(4);
    tree.Insert(3);
    tree.Insert(2);
    tree.Remove(3);
    tree.Remove(2);
    tree.Remove(4);
    return 0;
}
