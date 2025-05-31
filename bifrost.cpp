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

};

class Row {

};

class Table {

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