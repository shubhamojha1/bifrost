#include <cstdio>    // for remove
#include <iostream>  // for cout, cerr
#include <string>   // for string operations
#include <fstream>  // for ofstream (CSV file operations)
#include "DataLoader.h"  // includes Value.h, Row.h, and Table.h internally

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

int main() {
    std::cout << "Bifrost - An implementation of Hash Tables in database joins" << std::endl;
    // uint64_t seed = 1;
    // uint64_t hash_otpt[2];
    // const char *key = "hi";

    // MurmurHash3_x64_128(key, (uint64_t)strlen(key), seed, hash_otpt);

    // std::cout << "hashed" << hash_otpt[0] << hash_otpt[1] << std::endl;

    testWithCSVFiles();
}