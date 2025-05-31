#include <cstdio>    // for remove
#include <iostream>  // for cout, cerr
#include <string>   // for string operations
#include <fstream>  // for ofstream (CSV file operations)
#include "DataLoader.h"  // includes Value.h, Row.h, and Table.h internally

void testWithCSVFiles() {
    std::cout << "=== CSV File Test ===\n";
    
    // Create sample CSV files for testing
    std::ofstream emp_file("employees.csv");
    emp_file << "emp_id,name,dept_id\n";
    emp_file << "1,Alice,10\n2,Bob,20\n3,Charlie,10\n4,Diana,30\n";
    emp_file.close();
    
    std::ofstream dept_file("departments.csv");
    dept_file << "dept_id,dept_name,budget\n";
    dept_file << "10,Engineering,500000.0\n20,Marketing,200000.0\n30,Sales,300000.0\n";
    dept_file.close();
    
    try {
        DataLoader loader;
        auto employees = loader.loadFromCSV("employees.csv", "Employees");
        auto departments = loader.loadFromCSV("departments.csv", "Departments");
        
        std::cout << "Loaded tables from CSV:\n";
        employees->printSample();
        departments->printSample();
        
        // JoinEngine engine;
        // auto result = engine.hashJoin(*employees, "dept_id", *departments, "dept_id");
        
        // std::cout << "Join result:\n";
        // result->printSample();
        
        // engine.getProfiler().printReport();
        
        // Cleanup
        std::remove("employees.csv");
        std::remove("departments.csv");
        
    } catch (const std::exception& e) {
        std::cerr << "CSV test failed: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "Bifrost - An implementation of Hash Tables in database joins" << std::endl;
    // uint64_t seed = 1;
    // uint64_t hash_otpt[2];
    // const char *key = "hi";

    // MurmurHash3_x64_128(key, (uint64_t)strlen(key), seed, hash_otpt);

    // std::cout << "hashed" << hash_otpt[0] << hash_otpt[1] << std::endl;

    testWithCSVFiles();
}