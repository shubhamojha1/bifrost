#pragma once
#include "storage/StorageEngine.h"
#include "Table.h"
#include <unordered_map>
#include <memory>

namespace bifrost {
namespace storage {

/**
 * @brief In-memory storage engine implementation
 * 
 * This engine stores all data in memory using the existing Table class.
 * Data is lost when the engine is destroyed, making it suitable for
 * testing and temporary workloads.
 */
class InMemoryStorageEngine : public StorageEngine {
private:
    std::unordered_map<std::string, std::unique_ptr<Table>> tables_;

public:
    InMemoryStorageEngine() = default;
    virtual ~InMemoryStorageEngine() = default;

    // Table management operations
    bool createTable(const std::string& tableName, 
                    const std::vector<std::string>& columnNames,
                    const std::vector<DataType>& columnTypes) override;
    
    bool dropTable(const std::string& tableName) override;
    
    bool tableExists(const std::string& tableName) const override;

    // Data operations (CRUD)
    bool insert(const std::string& tableName, const Row& row) override;
    
    bool update(const std::string& tableName, 
               const std::vector<std::string>& columnNames,
               const std::vector<Value>& values,
               const std::string& whereColumn,
               const Value& whereValue) override;
    
    bool remove(const std::string& tableName,
               const std::string& whereColumn,
               const Value& whereValue) override;

    // Scan operations
    std::unique_ptr<ScanIterator> scan(const std::string& tableName) override;
    
    std::unique_ptr<ScanIterator> scan(const std::string& tableName,
                                      const std::string& columnName,
                                      const Value& value) override;

    // Utility operations
    size_t getRowCount(const std::string& tableName) const override;
    
    std::vector<std::string> getTableNames() const override;
    
    std::vector<std::string> getColumnNames(const std::string& tableName) const override;
    
    std::vector<DataType> getColumnTypes(const std::string& tableName) const override;

private:
    Table* getTable(const std::string& tableName) const;
    std::optional<size_t> getColumnIndex(const std::string& tableName, 
                                        const std::string& columnName) const;
};

/**
 * @brief Iterator for scanning in-memory table data
 */
class InMemoryScanIterator : public ScanIterator {
private:
    const std::vector<Row>* rows_;
    size_t current_index_;
    std::string column_name_;
    Value filter_value_;
    bool has_filter_;

public:
    InMemoryScanIterator(const std::vector<Row>* rows);
    InMemoryScanIterator(const std::vector<Row>* rows, 
                        const std::string& columnName, 
                        const Value& value);
    
    bool hasNext() override;
    Row next() override;
    void close() override {}
};

} // namespace storage
} // namespace bifrost
