#include <iostream>
#include <numeric>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

#include "trees/avl_tree.hpp"
#include "trees/bb_alpha_tree.hpp"
#include "trees/red_black_tree.hpp"
#include "trees/scapegoat_tree.hpp"
#include "trees/splay_tree.hpp"

enum EType {
    INSERT,
    REMOVE,
    SEARCH
};

struct Operation {
    int value;
    EType type;
};

template<template<typename> class TTreeType>
uint64_t measureOperationsTime(const std::vector<int>& preliminaryValues, const std::vector<Operation>& operations) {
    TTreeType<int> tree;

    for (const auto value : preliminaryValues) {
        tree.insert(value);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& op : operations) {
        int value = op.value;
        EType type = op.type;
        
        switch (type) {
            case EType::INSERT:
                tree.insert(value);
                break;
            case EType::REMOVE:
                tree.remove(value);
                break;
            case EType::SEARCH:
                volatile bool found = tree.search(op.value);
                (void)found;
                break;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count();
}

void runOperations(const std::vector<int>& preliminaryValues, const std::vector<Operation>& operations) {
    std::cout << "AVL Tree: " << measureOperationsTime<AVLTree>(preliminaryValues, operations) << "\n";
    std::cout << "BB-alpha Tree: " << measureOperationsTime<BBAlphaTree>(preliminaryValues, operations) << "\n";
    std::cout << "Red Black Tree: " << measureOperationsTime<RedBlackTree>(preliminaryValues, operations) << "\n";
    std::cout << "Scapegoat Tree: " << measureOperationsTime<ScapegoatTree>(preliminaryValues, operations) << "\n";
    std::cout << "Splay Tree: " << measureOperationsTime<SplayTree>(preliminaryValues, operations) << "\n\n";
}

int main() {
    std::mt19937 gen;


    {
        const int N = 1'000'000, Q = 10'000'000;
        std::vector<int> data(2 * N);
        std::iota(data.begin(), data.end(), 1);
        std::shuffle(data.begin(), data.end(), gen);

        std::vector<int> preliminaryValues;
        std::vector<Operation> operations;
        for (int i = 0; i < N; i++) {
            preliminaryValues.push_back(data[i]);
        }
        std::uniform_int_distribution<int> dis(1, 2 * N);
        for (int i = 0; i < Q; i++) {
            operations.push_back({dis(gen), SEARCH});
        }

        std::cout << "Search only with uniformly distributed queries\n";
        runOperations(preliminaryValues, operations);
    }

    {
        const int N = 1'000'000, Q = 10'000'000;
        std::vector<int> data(2 * N);
        std::iota(data.begin(), data.end(), 1);
        std::shuffle(data.begin(), data.end(), gen);

        std::vector<int> preliminaryValues(data.begin(), data.begin() + N);

        const int HOT_KEYS_COUNT = 0.01 * N;
        std::vector<int> hotKeys(preliminaryValues.begin(), preliminaryValues.begin() + HOT_KEYS_COUNT);
        
        std::vector<Operation> operations;
        
        std::uniform_int_distribution<int> hotKeysDis(0, HOT_KEYS_COUNT - 1);
        for (int i = 0; i < Q * 0.99; i++) {
            int hotKeyIndex = hotKeysDis(gen);
            operations.push_back({hotKeys[hotKeyIndex], SEARCH});
        }
        
        std::uniform_int_distribution<int> anyKeyDis(1, 2 * N);
        for (int i = 0; i < Q * 0.01; i++) {
            operations.push_back({anyKeyDis(gen), SEARCH});
        }
                
        std::cout << "Search only with hot keys:\n";
        runOperations(preliminaryValues, operations);
    }

    {
        const int N = 1'000'000, Q = 10'000'000;
        std::vector<int> data(2 * N);
        std::iota(data.begin(), data.end(), 1);
        std::shuffle(data.begin(), data.end(), gen);

        std::vector<int> preliminaryValues(data.begin(), data.begin() + N);
        
        const int HOT_KEYS_COUNT = 0.01 * N;
        std::vector<int> hotKeys(preliminaryValues.begin(), preliminaryValues.begin() + HOT_KEYS_COUNT);
        
        std::vector<Operation> operations;
        
        std::uniform_int_distribution<int> hotKeysDis(0, HOT_KEYS_COUNT - 1);
        for (int i = 0; i < Q * 0.99; i++) {
            int hotKeyIndex = hotKeysDis(gen);
            operations.push_back({hotKeys[hotKeyIndex], SEARCH});
        }
        
        std::uniform_int_distribution<int> anyKeyDis(1, 2 * N);
        for (int i = 0; i < Q * 0.01; i++) {
            operations.push_back({anyKeyDis(gen), SEARCH});
        }
        
        std::sort(operations.begin(), operations.end(), [](Operation l, Operation r) { return l.value < r.value; });
        
        std::cout << "Search only with hot keys and sorted queries:\n";
        runOperations(preliminaryValues, operations);
    }

    {
        const int N = 1'000'000, Q = 10'000'000;
        std::vector<int> data(2 * N);
        std::iota(data.begin(), data.end(), 1);
        std::shuffle(data.begin(), data.end(), gen);
    
        std::vector<int> preliminaryValues(data.begin(), data.begin() + N);
        
        std::vector<int> availableForInsert(data.begin() + N, data.end());
        std::vector<int> availableForDelete(preliminaryValues);
        
        std::vector<Operation> operations;
        
        std::uniform_int_distribution<int> operationDis(0, 2);
                std::uniform_int_distribution<int> valueDis(1, 2 * N);
        
        for (int i = 0; i < Q; i++) {
            int op = operationDis(gen);
            
            if (op == 0) {
                if (!availableForInsert.empty()) {
                    std::uniform_int_distribution<int> insertDis(0, availableForInsert.size() - 1);
                    int index = insertDis(gen);
                    int value = availableForInsert[index];
                    
                    std::swap(availableForInsert[index], availableForInsert.back());
                    availableForInsert.pop_back();
                    availableForDelete.push_back(value);
                    
                    operations.push_back({value, INSERT});
                }
                else {
                    operations.push_back({valueDis(gen), SEARCH});
                }
            }
            else if (op == 1) {
                int value = valueDis(gen);
                operations.push_back({value, SEARCH});
            }
            else {
                if (!availableForDelete.empty()) {
                    std::uniform_int_distribution<int> deleteDis(0, availableForDelete.size() - 1);
                    int index = deleteDis(gen);
                    int value = availableForDelete[index];
                    
                    std::swap(availableForDelete[index], availableForDelete.back());
                    availableForDelete.pop_back();
                    availableForInsert.push_back(value);
                    
                    operations.push_back({value, REMOVE});
                }
                else {
                    operations.push_back({valueDis(gen), SEARCH});
                }
            }
        }
        
        std::cout << "Mixed operations with uniform distribution:\n";
        runOperations(preliminaryValues, operations);
    }
    

    return 0;
}
