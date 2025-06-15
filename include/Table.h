#pragma once
#include <vector>
#include <string>
#include <optional>
#include "Value.h"
#include "Row.h"

struct ColumnInfo {
    std::string name;
    DataType type;
    size_t index;
};

class Table {
/*
           col_1 | col_2 | col_3
 (row1)    val1  | val2  | val3
 (row2)    val1  | val2  | val3
 (row3)    val1  | val2  | val3
*/
private:
    std::vector<ColumnInfo> schema_;
    std::vector<Row> rows_;
    std::string name_;

public:
    Table(const std::string& name) : name_(name) {}

    void addColumn(const std::string& name, DataType type) {
        schema_.push_back({name, type, schema_.size()});
    }

    void addRow(const Row& row);
    const Row& getRow(size_t index) const;
    size_t rowCount() const;
    size_t columnCount() const;
    const std::vector<ColumnInfo>& getSchema() const;
    const std::vector<Row>& getRows() const;
    const std::string& getName() const;
    std::optional<size_t> getColumnIndex(const std::string& colName) const;
    size_t estimateMemoryUsage() const;
    void clear();
    void printSchema() const;
    void printSample(size_t maxRows = 5) const;
};