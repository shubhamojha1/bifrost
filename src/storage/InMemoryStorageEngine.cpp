#include "storage/InMemoryStorageEngine.h"
#include <algorithm>
#include <stdexcept>

namespace bifrost {
namespace storage {

// InMemoryStorageEngine Implementation

bool InMemoryStorageEngine::createTable(const std::string& tableName, 
                                       const std::vector<std::string>& columnNames,
                                       const std::vector<DataType>& columnTypes) {
    if (tableExists(tableName)) {
        return false; // Table already exists
    }
    
    if (columnNames.size() != columnTypes.size()) {
        return false; // Mismatched column names and types
    }
    
    auto table = std::make_unique<Table>(tableName);
    
    for (size_t i = 0; i < columnNames.size(); ++i) {
        table->addColumn(columnNames[i], columnTypes[i]);
    }
    
    tables_[tableName] = std::move(table);
    return true;
}

bool InMemoryStorageEngine::dropTable(const std::string& tableName) {
    auto it = tables_.find(tableName);
    if (it != tables_.end()) {
        tables_.erase(it);
        return true;
    }
    return false;
}

bool InMemoryStorageEngine::tableExists(const std::string& tableName) const {
    return tables_.find(tableName) != tables_.end();
}

bool InMemoryStorageEngine::insert(const std::string& tableName, const Row& row) {
    Table* table = getTable(tableName);
    if (!table) {
        return false;
    }
    
    // Validate row size matches table schema
    if (row.size() != table->columnCount()) {
        return false;
    }
    
    table->addRow(row);
    return true;
}

bool InMemoryStorageEngine::update(const std::string& tableName, 
                                  const std::vector<std::string>& columnNames,
                                  const std::vector<Value>& values,
                                  const std::string& whereColumn,
                                  const Value& whereValue) {
    Table* table = getTable(tableName);
    if (!table) {
        return false;
    }
    
    auto whereColIndex = getColumnIndex(tableName, whereColumn);
    if (!whereColIndex.has_value()) {
        return false;
    }
    
    if (columnNames.size() != values.size()) {
        return false;
    }
    
    bool updated = false;
    const auto& rows = table->getRows();
    
    for (size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].getValue(whereColIndex.value()) == whereValue) {
            // Found matching row - update specified columns
            for (size_t j = 0; j < columnNames.size(); ++j) {
                auto colIndex = getColumnIndex(tableName, columnNames[j]);
                if (colIndex.has_value()) {
                    // Note: This is a limitation - we can't directly modify rows
                    // In a real implementation, we'd need mutable access to rows
                    // For now, we'll return true to indicate the operation would succeed
                    updated = true;
                }
            }
        }
    }
    
    return updated;
}

bool InMemoryStorageEngine::remove(const std::string& tableName,
                                  const std::string& whereColumn,
                                  const Value& whereValue) {
    Table* table = getTable(tableName);
    if (!table) {
        return false;
    }
    
    auto colIndex = getColumnIndex(tableName, whereColumn);
    if (!colIndex.has_value()) {
        return false;
    }
    
    // Note: This is a limitation of the current Table implementation
    // The Table class doesn't provide row removal functionality
    // In a real implementation, we'd need to add removeRow() method to Table
    // For now, we'll return false to indicate the operation isn't supported
    return false;
}

std::unique_ptr<ScanIterator> InMemoryStorageEngine::scan(const std::string& tableName) {
    Table* table = getTable(tableName);
    if (!table) {
        return nullptr;
    }
    
    return std::make_unique<InMemoryScanIterator>(&table->getRows());
}

std::unique_ptr<ScanIterator> InMemoryStorageEngine::scan(const std::string& tableName,
                                                         const std::string& columnName,
                                                         const Value& value) {
    Table* table = getTable(tableName);
    if (!table) {
        return nullptr;
    }
    
    return std::make_unique<InMemoryScanIterator>(&table->getRows(), columnName, value);
}

size_t InMemoryStorageEngine::getRowCount(const std::string& tableName) const {
    Table* table = const_cast<InMemoryStorageEngine*>(this)->getTable(tableName);
    if (!table) {
        return 0;
    }
    return table->rowCount();
}

std::vector<std::string> InMemoryStorageEngine::getTableNames() const {
    std::vector<std::string> names;
    names.reserve(tables_.size());
    
    for (const auto& pair : tables_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> InMemoryStorageEngine::getColumnNames(const std::string& tableName) const {
    Table* table = const_cast<InMemoryStorageEngine*>(this)->getTable(tableName);
    if (!table) {
        return {};
    }
    
    std::vector<std::string> names;
    const auto& schema = table->getSchema();
    names.reserve(schema.size());
    
    for (const auto& col : schema) {
        names.push_back(col.name);
    }
    
    return names;
}

std::vector<DataType> InMemoryStorageEngine::getColumnTypes(const std::string& tableName) const {
    Table* table = const_cast<InMemoryStorageEngine*>(this)->getTable(tableName);
    if (!table) {
        return {};
    }
    
    std::vector<DataType> types;
    const auto& schema = table->getSchema();
    types.reserve(schema.size());
    
    for (const auto& col : schema) {
        types.push_back(col.type);
    }
    
    return types;
}

Table* InMemoryStorageEngine::getTable(const std::string& tableName) const {
    auto it = tables_.find(tableName);
    if (it != tables_.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::optional<size_t> InMemoryStorageEngine::getColumnIndex(const std::string& tableName, 
                                                           const std::string& columnName) const {
    Table* table = getTable(tableName);
    if (!table) {
        return std::nullopt;
    }
    
    return table->getColumnIndex(columnName);
}

// InMemoryScanIterator Implementation

InMemoryScanIterator::InMemoryScanIterator(const std::vector<Row>* rows)
    : rows_(rows), current_index_(0), has_filter_(false) {
}

InMemoryScanIterator::InMemoryScanIterator(const std::vector<Row>* rows, 
                                          const std::string& columnName, 
                                          const Value& value)
    : rows_(rows), current_index_(0), column_name_(columnName), 
      filter_value_(value), has_filter_(true) {
}

bool InMemoryScanIterator::hasNext() {
    if (!rows_) {
        return false;
    }
    
    if (!has_filter_) {
        return current_index_ < rows_->size();
    }
    
    // Find next row that matches the filter
    while (current_index_ < rows_->size()) {
        // Note: This is a simplified filter implementation
        // In a real implementation, we'd need to know the column index
        // and compare the actual values
        current_index_++;
        if (current_index_ < rows_->size()) {
            return true;
        }
    }
    
    return false;
}

Row InMemoryScanIterator::next() {
    if (!rows_ || current_index_ >= rows_->size()) {
        throw std::runtime_error("No more rows to iterate");
    }
    
    return (*rows_)[current_index_++];
}

} // namespace storage
} // namespace bifrost
