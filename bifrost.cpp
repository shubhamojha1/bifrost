#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "MurmurHash3.h"
#include <cstring>
#include <variant>
#include <cstdint>
#include <memory>
#include <map>

using DataValue = std::variant<std::int64_t, double, std::string, std::monostate>;

enum class DataType {
    INTEGER,
    DOUBLE, 
    STRING,
    NULL_VALUE
};

class Value {
public:
    DataValue data;

    Value() : data(std::monostate{}) {}
    Value(std::int64_t i) : data(i) {}
    Value(double d) : data(d) {}
    Value(const std::string& s) : data(s) {}

    DataType getType() const {
        return static_cast<DataType>(data.index());
    }

    bool isNull() const {
        return std::holds_alternative<std::monostate>(data);
    }

    std::string toString() const {
        if (isNull()) return "NULL";
        return std::visit([](const auto& v) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::int64_t>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                return v;
            } else {
                return "NULL";
            }
        }, data);
    }

    bool operator==(const Value& other) const {
        return data == other.data;
    }

    bool operator<(const Value& other) const {
        return data < other.data;
    }
};

class Row {
private:
    std::vector<Value> values_;

public:
    Row() = default;
    Row(std::vector<Value> values) : values_(std::move(values)) {}

    const Value& operator[](size_t index) const {
        return values_[index];
    }

    Value& operator[](size_t index) {
        return values_[index];
    }

    size_t size() const { return values_.size(); }

    void addValue(const Value& value) {
        values_.push_back(value);
    }

    const std::vector<Value>& getValues() const { return values_; }

    std::string toString() const {
        std::string result = "(";
        for(size_t i=0; i<values_.size(); ++i){
            if(i>0) result += ", ";
            result += values_[i].toString();
        }
        result += ")";
        return result;
    }
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
};


class DataLoader {
    private:
        std::vector<std::string> split(const std::string& str, char delimiter) {

        }

        DataType inferType(const std::string& value) {

        }

    public:
        std::unique_ptr<Table> loadFromCSV(const std::string& filename, const std::string& tablename) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file: "+ filename);
            }

            auto table = std::make_unique<Table>(tablename);
            std::string line;

            // Read header
            if (!std::getline(file, line)) {
                throw std::runtime_error("Empty CSV file");
            }

            auto headers = split(line, ',');
            // make the colummns first
            std::vector<DataType> columnTypes(headers.size(), DataType::NULL_VALUE);

            // go through first 10 rows to infer data type
            std::vector<std::vector<std::string>> sampleData;
            for(int i=0; i<10 && std::getline(file, line); ++i){
                sampleData.push_back(split(line, ','));
            }

            // infer column type
            for (size_t col = 0; col < headers.size(); ++col){
                std::map<DataType, int> typeCounts;
                for (const auto& row: sampleData) {
                    if (col < row.size()) {
                        DataType type = inferType(row[col]);
                        typeCounts[type]++;
                    }
                }

                // choose most common non-null type
                DataType bestType = DataType::NULL_VALUE; // null_value or string.. which to use?
                int maxCount = 0;
                for(const auto& [type, count]: typeCounts){
                    if (type != DataType::NULL_VALUE && count > maxCount){
                        bestType = type;
                        maxCount = count;
                    }
                }
                columnTypes[col] = bestType;
            }

            // add columns to table
            for (size_t i=0; i<headers.size(); ++i){
                table->addColumn(headers[i], columnTypes[i]);
            }

            file.clear();
            file.seekg(0);
            std::getline(file, line); // start from the first row, first value

            // reading starts
            while (std::getline(file, line)) {
                auto values = split(line, ',');
                Row row;

                for(size_t i=0; i<headers.size() && i<values.size(); ++i){
                    row.addValue(parseValue(values[i], columnTypes[i]));
                }

                while(row.size() < headers.size()){
                    row.addValue(Value()); // some null value
                }

                table->addRow(row);
            }

            return table;
        }

};

int main() {
    std::cout << "Bifrost - An implementation of Hash Tables in database joins" << std::endl;
    uint64_t seed = 1;
    uint64_t hash_otpt[2];
    const char *key = "hi";

    MurmurHash3_x64_128(key, (uint64_t)strlen(key), seed, hash_otpt);

    std::cout << "hashed" << hash_otpt[0] << hash_otpt[1] << std::endl;
}