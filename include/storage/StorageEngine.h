#pragma once
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include "Value.h"
#include "Row.h"

namespace bifrost {
namespace storage {

// Forward declarations
class ScanIterator;

/**
 * @brief Abstract storage engine interface for pluggable storage backends
 * 
 * This interface defines the core operations that any storage engine must support.
 * Different implementations can provide various performance characteristics and
 * durability guarantees.
 */
class StorageEngine {
public:
    virtual ~StorageEngine() = default;

    // Table management operations
    virtual bool createTable(const std::string& tableName, 
                           const std::vector<std::string>& columnNames,
                           const std::vector<DataType>& columnTypes) = 0;
    
    virtual bool dropTable(const std::string& tableName) = 0;
    
    virtual bool tableExists(const std::string& tableName) const = 0;

    // Data operations (CRUD)
    virtual bool insert(const std::string& tableName, const Row& row) = 0;
    
    virtual bool update(const std::string& tableName, 
                       const std::vector<std::string>& columnNames,
                       const std::vector<Value>& values,
                       const std::string& whereColumn,
                       const Value& whereValue) = 0;
    
    virtual bool remove(const std::string& tableName,
                       const std::string& whereColumn,
                       const Value& whereValue) = 0;

    // Scan operations
    virtual std::unique_ptr<ScanIterator> scan(const std::string& tableName) = 0;
    
    virtual std::unique_ptr<ScanIterator> scan(const std::string& tableName,
                                              const std::string& columnName,
                                              const Value& value) = 0;

    // Utility operations
    virtual size_t getRowCount(const std::string& tableName) const = 0;
    
    virtual std::vector<std::string> getTableNames() const = 0;
    
    virtual std::vector<std::string> getColumnNames(const std::string& tableName) const = 0;
    
    virtual std::vector<DataType> getColumnTypes(const std::string& tableName) const = 0;

    // Transaction support (placeholder for future phases)
    virtual void beginTransaction() {}
    virtual void commitTransaction() {}
    virtual void rollbackTransaction() {}
};

/**
 * @brief Iterator for scanning table data
 */
class ScanIterator {
public:
    virtual ~ScanIterator() = default;
    
    virtual bool hasNext() = 0;
    
    virtual Row next() = 0;
    
    virtual void close() {}
};

/**
 * @brief Configuration for storage engines
 */
struct StorageConfig {
    // enum class Mode {
    //     IN_MEMORY,    // Fast, but data lost on restart
    //     LOG_BASED,    // Append-only log, good for OLTP
    //     PAGE_BASED,   // Traditional page-based storage
    //     LSM_TREE      // LSM-tree for write-heavy workloads
    // };
    enum class Mode {
        // PAGE_BASED,   // B+ tree (DEFAULT - best for learning)
        // IN_MEMORY,    // Fast, but data lost on restart  
        // LOG_BASED,    // Append-only log, good for OLTP
        // LSM_TREE      // Advanced option for later
        IN_MEMORY, // Hash Map
        LOG_BASED, // Append-Only
        PAGE_BASED, // B-Tree
        LSM_TREEE // LSM Tree
    };
    
    Mode mode = Mode::IN_MEMORY;
    std::string dataDirectory = "./data"; // need to find a better way
    size_t pageSize = 8192;  // 8KB pages
    size_t bufferPoolSize = 128 * 1024 * 1024;  // 128MB
    bool enableCompression = false;
    bool enableChecksums = true;
};

/**
 * @brief Factory for creating storage engines
 */
class StorageEngineFactory {
public:
    static std::unique_ptr<StorageEngine> create(const StorageConfig& config);
    
    static std::unique_ptr<StorageEngine> createInMemory();
    
    static std::unique_ptr<StorageEngine> createLogBased(const std::string& dataDir);
    
    static std::unique_ptr<StorageEngine> createPageBased(const StorageConfig& config);
};

} // namespace storage
} // namespace bifrost