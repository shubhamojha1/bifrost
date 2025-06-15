#include "BenchmarkSuite.h"
#include "DataLoader.h"
#include "JoinEngine.h"
#include <iomanip>

Profiler::ProfileData BenchmarkSuite::benchmarkJoinWithStrategy(size_t tableSize, CollisionStrategy strategy) {
        DataLoader loader;
        auto leftTable = loader.generateTestTable("Left", tableSize, 42);
        auto rightTable = loader.generateTestTable("Right", tableSize / 2, 123);
        
        JoinEngine engine;
        auto result = engine.hashJoin(*leftTable, "value", *rightTable, "value", 
                                     JoinType::INNER, strategy);
        
        return engine.getProfiler().getData();
    }

void BenchmarkSuite::runHashTableBenchmark() {
        std::cout << "\n=== Hash Table Strategy Comparison ===\n";
        
        const std::vector<size_t> sizes = {1000, 10000, 100000};
        const std::vector<CollisionStrategy> strategies = {
            CollisionStrategy::CHAINING,
            CollisionStrategy::LINEAR_PROBING
        };
        
        for (size_t tableSize : sizes) {
            std::cout << "\nTable Size: " << tableSize << " rows\n";
            std::cout << std::setw(20) << "Strategy" 
                      << std::setw(15) << "Build (ms)"
                      << std::setw(15) << "Probe (ms)"
                      << std::setw(15) << "Total (ms)"
                      << std::setw(15) << "Load Factor"
                      << std::setw(15) << "Collisions\n";
            std::cout << std::string(95, '-') << "\n";
            
            for (auto strategy : strategies) {
                auto result = benchmarkJoinWithStrategy(tableSize, strategy);
                
                std::string strategyName = (strategy == CollisionStrategy::CHAINING) ? 
                    "Chaining" : "Linear Probing";
                
                std::cout << std::setw(20) << strategyName
                          << std::setw(15) << std::fixed << std::setprecision(2)
                          << result.buildTime.count() / 1e6
                          << std::setw(15) << result.probeTime.count() / 1e6
                          << std::setw(15) << result.totalTime.count() / 1e6
                          << std::setw(15) << std::setprecision(3) << result.hashStats.loadFactor
                          << std::setw(15) << result.hashStats.collisions << "\n";
            }
        }
    }

void BenchmarkSuite::runJoinTypeBenchmark() {
        std::cout << "\n=== Join Type Performance Comparison ===\n";
        
        DataLoader loader;
        auto leftTable = loader.generateTestTable("Left", 10000, 42);
        auto rightTable = loader.generateTestTable("Right", 5000, 123);
        
        const std::vector<std::pair<JoinType, std::string>> joinTypes = {
            {JoinType::INNER, "INNER"},
            {JoinType::LEFT_OUTER, "LEFT OUTER"},
            {JoinType::RIGHT_OUTER, "RIGHT OUTER"},
            {JoinType::FULL_OUTER, "FULL OUTER"}
        };
        
        std::cout << std::setw(15) << "Join Type"
                  << std::setw(15) << "Build (ms)"
                  << std::setw(15) << "Probe (ms)"
                  << std::setw(15) << "Total (ms)"
                  << std::setw(15) << "Result Rows"
                  << std::setw(15) << "Selectivity\n";
        std::cout << std::string(90, '-') << "\n";
        
        JoinEngine engine;
        for (const auto& [joinType, joinName] : joinTypes) {
            auto result = engine.hashJoin(*leftTable, "value", *rightTable, "value", joinType);
            const auto& profilerData = engine.getProfiler().getData();
            
            std::cout << std::setw(15) << joinName
                      << std::setw(15) << std::fixed << std::setprecision(2)
                      << profilerData.buildTime.count() / 1e6
                      << std::setw(15) << profilerData.probeTime.count() / 1e6
                      << std::setw(15) << profilerData.totalTime.count() / 1e6
                      << std::setw(15) << profilerData.resultRows
                      << std::setw(15) << std::setprecision(3) 
                      << profilerData.selectivity * 100 << "%\n";
        }
    }

void BenchmarkSuite::runScalabilityBenchmark() {
        std::cout << "\n=== Scalability Analysis ===\n";
        
        const std::vector<size_t> leftSizes = {1000, 5000, 10000, 50000};
        const std::vector<size_t> rightSizes = {1000, 5000, 10000, 50000};
        
        std::cout << std::setw(12) << "Left Size"
                  << std::setw(12) << "Right Size"
                  << std::setw(15) << "Total (ms)"
                  << std::setw(15) << "Memory (KB)"
                  << std::setw(15) << "Result Rows"
                  << std::setw(15) << "Throughput\n";
        std::cout << std::string(84, '-') << "\n";
        
        DataLoader loader;
        JoinEngine engine;
        
        for (size_t leftSize : leftSizes) {
            for (size_t rightSize : rightSizes) {
                auto leftTable = loader.generateTestTable("Left", leftSize, 42);
                auto rightTable = loader.generateTestTable("Right", rightSize, 123);
                
                auto result = engine.hashJoin(*leftTable, "value", *rightTable, "value");
                const auto& profilerData = engine.getProfiler().getData();
                
                double throughputRowsPerMs = static_cast<double>(leftSize + rightSize) / 
                    (profilerData.totalTime.count() / 1e6);
                
                std::cout << std::setw(12) << leftSize
                          << std::setw(12) << rightSize
                          << std::setw(15) << std::fixed << std::setprecision(2)
                          << profilerData.totalTime.count() / 1e6
                          << std::setw(15) << profilerData.memoryUsage / 1024
                          << std::setw(15) << profilerData.resultRows
                          << std::setw(15) << std::setprecision(0) << throughputRowsPerMs << "\n";
            }
        }
    }

