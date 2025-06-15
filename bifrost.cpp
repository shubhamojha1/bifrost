#include <cstdio>    // for remove
#include <iostream>  // for cout, cerr
#include <string>   // for string operations
#include <fstream>  // for ofstream (CSV file operations)
#include "DataLoader.h"  // includes Value.h, Row.h, and Table.h internally
#include "BenchmarkSuite.h"
#include "JoinEngine.h"
#include "Table.h"

void testWithCSVFiles() {
    std::cout << "=== CSV File Test ===\n";
      try {
        DataLoader loader;
        auto students = loader.loadFromCSV("assets/csv/students.csv", "Students");
        auto grades = loader.loadFromCSV("assets/csv/grades.csv", "Grades");
        std::cout << "Loaded tables from CSV:\n";
        students->printSample();
        grades->printSample();
        
        // JoinEngine engine;
        // auto result = engine.hashJoin(*students, "student_id", *grades, "student_id");
        
        // std::cout << "Join result:\n";
        // result->printSample();
        
        // engine.getProfiler().printReport();
        
    } catch (const std::exception& e) {
        std::cerr << "CSV loading failed: " << e.what() << std::endl;
    }
}

void demonstrateBasicUsage() {
    std::cout << "=== Hash Join Engine Demo ===\n\n";
    
    // Create sample data
    DataLoader loader;
    
    std::cout << "Creating sample tables...\n";
    auto employees = std::make_unique<Table>("Employees");
    employees->addColumn("emp_id", DataType::INTEGER);
    employees->addColumn("name", DataType::STRING);
    employees->addColumn("dept_id", DataType::INTEGER);
    
    auto departments = std::make_unique<Table>("Departments");
    departments->addColumn("dept_id", DataType::INTEGER);
    departments->addColumn("dept_name", DataType::STRING);
    departments->addColumn("budget", DataType::DOUBLE);
    
    // Add sample data
    std::vector<std::tuple<int, std::string, int>> empData = {
        {1, "Alice", 10}, {2, "Bob", 20}, {3, "Charlie", 10},
        {4, "Diana", 30}, {5, "Eve", 20}, {6, "Frank", 40}
    };
    
    for (const auto& [id, name, deptId] : empData) {
        Row row;
        row.addValue(Value(static_cast<std::int64_t>(id)));
        row.addValue(Value(name));
        row.addValue(Value(static_cast<std::int64_t>(deptId)));
        employees->addRow(row);
    }
    
    std::vector<std::tuple<int, std::string, double>> deptData = {
        {10, "Engineering", 500000.0}, {20, "Marketing", 200000.0},
        {30, "Sales", 300000.0}
    };
    
    for (const auto& [id, name, budget] : deptData) {
        Row row;
        row.addValue(Value(static_cast<std::int64_t>(id)));
        row.addValue(Value(name));
        row.addValue(Value(budget));
        departments->addRow(row);
    }
    
    std::cout << "Sample data created:\n";
    employees->printSample();
    std::cout << std::endl;
    departments->printSample();
    std::cout << std::endl;
    
    // Perform join
    std::cout << "Performing INNER JOIN on dept_id...\n";
    JoinEngine engine;
    auto result = engine.hashJoin(*employees, "dept_id", *departments, "dept_id", JoinType::INNER);
    
    std::cout << "Join completed. Results:\n";
    result->printSample(10);
    
    // Show performance report
    engine.getProfiler().printReport();
}

void demonstrateAdvancedFeatures() {
    std::cout << "=== Advanced Features Demo ===\n\n";
    
    DataLoader loader;
    
    std::cout << "Generating large test datasets...\n";
    auto leftTable = loader.generateTestTable("LargeLeft", 50000, 42);
    auto rightTable = loader.generateTestTable("LargeRight", 30000, 123);
    
    std::cout << "Testing different join types:\n\n";
    
    JoinEngine engine;
    const std::vector<std::pair<JoinType, std::string>> joinTypes = {
        {JoinType::INNER, "INNER JOIN"},
        {JoinType::LEFT_OUTER, "LEFT OUTER JOIN"},
        {JoinType::RIGHT_OUTER, "RIGHT OUTER JOIN"},
        {JoinType::FULL_OUTER, "FULL OUTER JOIN"}
    };
    
    for (const auto& [joinType, joinName] : joinTypes) {
        std::cout << "Performing " << joinName << "...\n";
        auto result = engine.hashJoin(*leftTable, "value", *rightTable, "value", joinType);
        std::cout << "Result: " << result->rowCount() << " rows\n";
        engine.getProfiler().printReport();
    }
    
    std::cout << "\nTesting collision strategies (INNER JOIN):\n";
    const std::vector<std::pair<CollisionStrategy, std::string>> strategies = {
        {CollisionStrategy::CHAINING, "Chaining"},
        {CollisionStrategy::LINEAR_PROBING, "Linear Probing"}
    };
    
    for (const auto& [strategy, strategyName] : strategies) {
        std::cout << "\nUsing " << strategyName << " strategy:\n";
        auto result = engine.hashJoin(*leftTable, "value", *rightTable, "value", 
                                     JoinType::INNER, strategy);
        engine.getProfiler().printReport();
    }
}

void runComprehensiveTests() {
    std::cout << "=== Comprehensive Test Suite ===\n\n";
    
    BenchmarkSuite benchmark;
    
    std::cout << "Running hash table strategy benchmark...\n";
    benchmark.runHashTableBenchmark();
    
    std::cout << "\nRunning join type benchmark...\n";
    benchmark.runJoinTypeBenchmark();
    
    std::cout << "\nRunning scalability benchmark...\n";
    benchmark.runScalabilityBenchmark();
}

void testMemoryManagement() {
    std::cout << "=== Memory Management Test ===\n\n";
    
    DataLoader loader;
    std::cout << "Creating large dataset to test memory usage...\n";
    
    auto largeTable1 = loader.generateTestTable("Large1", 100000, 42);
    auto largeTable2 = loader.generateTestTable("Large2", 100000, 123);
    
    std::cout << "Table 1 memory usage: " << largeTable1->estimateMemoryUsage() / 1024 << " KB\n";
    std::cout << "Table 2 memory usage: " << largeTable2->estimateMemoryUsage() / 1024 << " KB\n";
    
    JoinEngine engine;
    std::cout << "\nPerforming large join operation...\n";
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto result = engine.hashJoin(*largeTable1, "value", *largeTable2, "value");
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Join completed in " << duration.count() << " ms\n";
    std::cout << "Result rows: " << result->rowCount() << "\n";
    std::cout << "Result memory usage: " << result->estimateMemoryUsage() / 1024 << " KB\n";
    
    engine.getProfiler().printReport();
}


int main(int argc, char*argv[]) {
    std::cout << "Bifrost - An implementation of Hash Tables in database joins" << std::endl;
    // uint64_t seed = 1;
    // uint64_t hash_otpt[2];
    // const char *key = "hi";

    // MurmurHash3_x64_128(key, (uint64_t)strlen(key), seed, hash_otpt);

    // std::cout << "hashed" << hash_otpt[0] << hash_otpt[1] << std::endl;

    // testWithCSVFiles();
    try {
        std::cout << "Hash Join Engine - Database Query Processing System\n";
        std::cout << "==================================================\n\n";
        
        if (argc > 1) {
            std::string arg = argv[1];
            if (arg == "--demo") {
                demonstrateBasicUsage();
            } else if (arg == "--advanced") {
                demonstrateAdvancedFeatures();
            } else if (arg == "--benchmark") {
                runComprehensiveTests();
            } else if (arg == "--memory") {
                testMemoryManagement();
            } else if (arg == "--all") {
                demonstrateBasicUsage();
                demonstrateAdvancedFeatures();
                runComprehensiveTests();
                testMemoryManagement();
            } else {
                std::cout << "Usage: " << argv[0] << " [--demo|--advanced|--benchmark|--memory|--all]\n";
                return 1;
            }
        } else {
            // Default: run basic demo
            demonstrateBasicUsage();
            
            std::cout << "\nAvailable options:\n";
            std::cout << "  --demo      Basic usage demonstration\n";
            std::cout << "  --advanced  Advanced features demo\n";
            std::cout << "  --benchmark Comprehensive benchmarks\n";
            std::cout << "  --memory    Memory management tests\n";
            std::cout << "  --all       Run all tests\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}