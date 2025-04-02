#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <memory>
#include <unordered_set>
#include <string>
#include <iomanip>
#include <set>

#include "trees/avl_tree.hpp"
#include "trees/bb_alpha_tree.hpp"
#include "trees/red_black_tree.hpp"
#include "trees/scapegoat_tree.hpp"
#include "trees/splay_tree.hpp"

class DataGenerator {
public:
    static std::mt19937 gen;

    static void seed(unsigned value) { gen.seed(value); }

    static std::vector<int> generateUniqueData(size_t size, int min, int max) {
        std::unordered_set<int> uniqueValues;
        std::uniform_int_distribution<int> dist(min, max);
        
        while (uniqueValues.size() < size) {
            int value = dist(gen);
            if (uniqueValues.insert(value).second) {
                if (uniqueValues.size() >= size) break;
            }
        }
        
        std::vector<int> result(uniqueValues.begin(), uniqueValues.end());
        std::shuffle(result.begin(), result.end(), gen);
        return result;
    }

    static std::vector<int> generateHotKeysQueries(const std::vector<int>& data, size_t queriesCount) {
        std::vector<int> queries;
        queries.reserve(queriesCount);
        
        const size_t hotKeysCount = std::max(size_t(1), data.size()/100);
        std::vector<int> hotKeys;
        hotKeys.reserve(hotKeysCount);
        
        std::sample(data.begin(), data.end(), std::back_inserter(hotKeys), 
                   hotKeysCount, gen);
        
        std::bernoulli_distribution hotDist(0.99);
        std::uniform_int_distribution<size_t> hotIndex(0, hotKeysCount-1);
        std::uniform_int_distribution<size_t> dataIndex(0, data.size()-1);

        for (size_t i = 0; i < queriesCount; ++i) {
            queries.push_back(hotDist(gen) ? hotKeys[hotIndex(gen)] : data[dataIndex(gen)]);
        }
        
        return queries;
    }

    static std::vector<int> generateUniformQueries(const std::vector<int>& data, size_t queriesCount) {
        std::vector<int> queries(queriesCount);
        std::uniform_int_distribution<size_t> dist(0, data.size()-1);
        std::generate(queries.begin(), queries.end(), [&]() { return data[dist(gen)]; });
        return queries;
    }
};

std::mt19937 DataGenerator::gen(std::random_device{}());

struct Operation {
    enum Type { INSERT, SEARCH, REMOVE };
    Type type;
    int value;
};

class Benchmark {
private:
    struct Result {
        std::string treeName;
        std::string scenarioName;
        size_t dataSize;
        long long executionTime;
    };

    std::vector<Result> results;
    
    enum ScenarioType {
        SEARCH_UNIFORM,
        SEARCH_HOTKEYS,
        BALANCED_OPS,
        SEARCH_HEAVY
    };

    template<typename Tree>
    void testScenario(const std::vector<Operation>& operations, 
                     const std::string& scenarioName, 
                     size_t dataSize) {
        Tree tree;
        std::unordered_set<int> presentValues;
        
        if constexpr (std::is_same_v<Tree, SplayTree<int>>) {
            const int WARMUP_ITERATIONS = 3;
            for (int i = 0; i < WARMUP_ITERATIONS; ++i) {
                for (const auto& op : operations) {
                    if (op.type == Operation::SEARCH) {
                        tree.search(op.value);
                    }
                }
            }
        }

        auto start = std::chrono::high_resolution_clock::now();
        
        for (const auto& op : operations) {
            switch(op.type) {
                case Operation::INSERT:
                    if (presentValues.insert(op.value).second) {
                        tree.insert(op.value);
                    }
                    break;
                case Operation::SEARCH: {
                    volatile bool found = tree.search(op.value);
                    (void)found;
                    break;
                }
                case Operation::REMOVE:
                    if (presentValues.erase(op.value)) {
                        tree.remove(op.value);
                    }
                    break;
            }
        }
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
        
        results.emplace_back(Result{Tree::name(), scenarioName, dataSize, duration});
    }

    std::vector<Operation> generateOperations(ScenarioType type, 
                                             const std::vector<int>& data,
                                             size_t opsCount) {
        std::vector<Operation> operations;
        operations.reserve(opsCount);
        
        std::unordered_set<int> available(data.begin(), data.end());
        std::vector<int> hotKeys;
        std::mt19937& gen = DataGenerator::gen;
        
        if (type == SEARCH_HOTKEYS) {
            size_t hotCount = std::max(1ul, data.size()/100);
            std::sample(data.begin(), data.end(), std::back_inserter(hotKeys),
                       hotCount, gen);
        }

        std::uniform_int_distribution<size_t> dist(0, data.size()-1);
        std::uniform_real_distribution<double> prob(0.0, 1.0);

        for (size_t i = 0; i < opsCount; ++i) {
            Operation op;
            
            switch(type) {
                case SEARCH_UNIFORM:
                    op = {Operation::SEARCH, data[dist(gen)]};
                    break;
                    
                case SEARCH_HOTKEYS: {
                    std::bernoulli_distribution hot(0.99);
                    op.value = hot(gen) ? hotKeys[dist(gen) % hotKeys.size()] 
                                      : data[dist(gen)];
                    op.type = Operation::SEARCH;
                    break;
                }
                
                case BALANCED_OPS: {
                    double p = prob(gen);
                    if (p < 0.33 && !available.empty()) {
                        auto it = available.begin();
                        std::advance(it, dist(gen) % available.size());
                        op = {Operation::REMOVE, *it};
                        available.erase(it);
                    } else if (p < 0.66) {
                        int val = *std::max_element(data.begin(), data.end()) + i + 1;
                        available.insert(val);
                        op = {Operation::INSERT, val};
                    } else {
                        op = {Operation::SEARCH, data[dist(gen)]};
                    }
                    break;
                }
                
                case SEARCH_HEAVY: {
                    double p = prob(gen);
                    if (p < 0.8) {
                        op = {Operation::SEARCH, data[dist(gen)]};
                    } else if (p < 0.95 && !available.empty()) {
                        auto it = available.begin();
                        std::advance(it, dist(gen) % available.size());
                        op = {Operation::REMOVE, *it};
                        available.erase(it);
                    } else {
                        int val = *std::max_element(data.begin(), data.end()) + i + 1;
                        available.insert(val);
                        op = {Operation::INSERT, val};
                    }
                    break;
                }
            }
            
            operations.push_back(op);
        }
        
        return operations;
    }

public:
    void runBenchmarks() {
        const std::vector<size_t> sizes = {1000, 10000, 100000};
        const std::vector<std::pair<ScenarioType, std::string>> scenarios = {
            {SEARCH_UNIFORM,    "Search Only - Uniform"},
            {SEARCH_HOTKEYS,    "Search Only - Hot Keys"},
            {BALANCED_OPS,      "Balanced Operations"},
            {SEARCH_HEAVY,      "Search Heavy (80/15/5)"}
        };

        DataGenerator::seed(42);

        for (size_t size : sizes) {
            std::cout << "Run for size: " << size << "\n";
            auto data = DataGenerator::generateUniqueData(size, 0, size*2);
            
            for (const auto& [scenario, name] : scenarios) {
                auto operations = generateOperations(scenario, data, size);
                
                testScenario<AVLTree<int>>(operations, name, size);
                testScenario<BBAlphaTree<int>>(operations, name, size);
                testScenario<RedBlackTree<int>>(operations, name, size);
                testScenario<ScapegoatTree<int>>(operations, name, size);
                testScenario<SplayTree<int>>(operations, name, size);
            }
        }
    }

    void printResults() const {
        std::cout << "\nResults:\n";
        std::cout << std::string(90, '-') << "\n";
        std::cout << std::left 
                  << std::setw(20) << "Tree Type"
                  << std::setw(30) << "Scenario"
                  << std::setw(15) << "Data Size"
                  << std::setw(15) << "Time (ms)\n";
        std::cout << std::string(90, '-') << "\n";
         
        for (const auto& res : results) {
            std::cout << std::left
                      << std::setw(20) << res.treeName
                      << std::setw(30) << res.scenarioName
                      << std::setw(15) << res.dataSize
                      << std::setw(15) << res.executionTime << "\n";
        }
        std::cout << std::string(90, '-') << std::endl;
    }
};

int main() {
    Benchmark benchmark;
    benchmark.runBenchmarks();
    benchmark.printResults();
    return 0;
}
