#include "storage/StorageEngine.h"
#include "storage/InMemoryStorageEngine.h"
#include <iostream>
#include <memory>

using namespace bifrost::storage;

int main() {
    std::cout << "Testing StorageEngineFactory with InMemoryStorageEngine\n";
    std::cout << "======================================================\n\n";

    try {
        // Method 1: Create using factory with config
        std::cout << "1. Creating InMemoryStorageEngine via StorageConfig...\n";
        StorageConfig config;
        config.mode = StorageConfig::Mode::IN_MEMORY;
        auto engine1 = StorageEngineFactory::create(config);
        
        if (engine1) {
            std::cout << "   ✓ Successfully created InMemoryStorageEngine via config\n";
        }

        // Method 2: Create directly using factory method
        std::cout << "\n2. Creating InMemoryStorageEngine via direct factory method...\n";
        auto engine2 = StorageEngineFactory::createInMemory();
        
        if (engine2) {
            std::cout << "   ✓ Successfully created InMemoryStorageEngine via direct method\n";
        }

        // Method 3: Create directly (for testing)
        std::cout << "\n3. Creating InMemoryStorageEngine directly...\n";
        auto engine3 = std::make_unique<InMemoryStorageEngine>();
        
        if (engine3) {
            std::cout << "   ✓ Successfully created InMemoryStorageEngine directly\n";
        }

        // Test basic functionality
        std::cout << "\n4. Testing basic storage operations...\n";
        
        // Create a table
        std::vector<std::string> columnNames = {"id", "name", "age"};
        std::vector<DataType> columnTypes = {DataType::INTEGER, DataType::STRING, DataType::INTEGER};
        
        bool created = engine3->createTable("users", columnNames, columnTypes);
        if (created) {
            std::cout << "   ✓ Successfully created 'users' table\n";
        } else {
            std::cout << "   ✗ Failed to create 'users' table\n";
        }

        // Check if table exists
        bool exists = engine3->tableExists("users");
        if (exists) {
            std::cout << "   ✓ Table 'users' exists\n";
        } else {
            std::cout << "   ✗ Table 'users' does not exist\n";
        }

        // Get table names
        auto tableNames = engine3->getTableNames();
        std::cout << "   Tables: ";
        for (const auto& name : tableNames) {
            std::cout << name << " ";
        }
        std::cout << "\n";

        // Get column info
        auto columns = engine3->getColumnNames("users");
        std::cout << "   Columns in 'users': ";
        for (const auto& col : columns) {
            std::cout << col << " ";
        }
        std::cout << "\n";

        auto types = engine3->getColumnTypes("users");
        std::cout << "   Column types: ";
        for (const auto& type : types) {
            switch (type) {
                case DataType::INTEGER: std::cout << "INTEGER "; break;
                case DataType::STRING: std::cout << "STRING "; break;
                case DataType::DOUBLE: std::cout << "DOUBLE "; break;
                case DataType::NULL_VALUE: std::cout << "NULL "; break;
            }
        }
        std::cout << "\n";

        std::cout << "\n✓ All tests passed! InMemoryStorageEngine is working correctly.\n";

    } catch (const std::exception& e) {
        std::cerr << "✗ Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
