#include "Table.h"
#include <iostream>
#include <algorithm>
#include <variant>
#include <string>

void Table::addRow(const Row& row) {
    if (row.size() != schema_.size()) {
        throw std::runtime_error("Row size doesn't match schema");
    }
    rows_.push_back(row);
}

const Row& Table::getRow(size_t index) const {
    return rows_[index];
}

size_t Table::rowCount() const { 
    return rows_.size(); 
}

size_t Table::columnCount() const { 
    return schema_.size(); 
}

const std::vector<ColumnInfo>& Table::getSchema() const { 
    return schema_; 
}

const std::vector<Row>& Table::getRows() const { 
    return rows_; 
}

const std::string& Table::getName() const { 
    return name_; 
}

std::optional<size_t> Table::getColumnIndex(const std::string& colName) const {
    for (const auto& col : schema_) {
        if (col.name == colName) return col.index;
    }
    return std::nullopt;
}
size_t Table::estimateMemoryUsage() const {
    size_t total = sizeof(*this);
    total += schema_.capacity() * sizeof(ColumnInfo);
    total += rows_.capacity() * sizeof(Row);
    
    for (const auto& row : rows_) {
        for (const auto& value : row.getValues()) {
            if (std::holds_alternative<std::string>(value.data)) {
                total += std::get<std::string>(value.data).capacity();
            }
        }
    }
    return total;
}

void Table::clear() {
    rows_.clear();
}

void Table::printSchema() const {
    std::cout << "Table: " << name_ << std::endl;
    std::cout << "Schema: ";
    for(size_t i=0; i<schema_.size(); ++i) {
        if(i>0) std::cout << ", ";
        std::cout << schema_[i].name;
    }
    std::cout << std::endl;
}

void Table::printSample(size_t maxRows) const {
    printSchema();
    std::cout << "Sample data (" << std::min(maxRows, rows_.size()) << " rows):" << std::endl;
    for (size_t i = 0; i < std::min(maxRows, rows_.size()); ++i) {
        std::cout << "  " << rows_[i].toString() << std::endl;
    }
    if (rows_.size() > maxRows) {
        std::cout << "  ... (" << (rows_.size() - maxRows) << " more rows)" << std::endl;
    }
    std::cout << std::endl;
}

