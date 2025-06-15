#include "DataLoader.h"
#include <iostream>
#include <sstream>
#include <map>
#include <fstream>

std::vector<std::string> DataLoader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        tokens.push_back(token);
    }
    return tokens;
}

DataType DataLoader::inferType(const std::string& value) {
    if (value.empty() || value == "NULL" || value == "null") {
        return DataType::NULL_VALUE;
    }

    try {
        std::stoll(value);
        return DataType::INTEGER;
    } catch (...) {}

    // Try double
    try {
        std::stod(value);
        return DataType::DOUBLE;
    } catch (...) {}

    return DataType::STRING;
}

Value DataLoader::parseValue(const std::string& str, DataType type) {
    if (str.empty() || str == "NULL" || str == "null") {
        return Value();
    }
    
    switch (type) {
        case DataType::INTEGER:
            try {
                return Value(static_cast<std::int64_t>(std::stoll(str)));
            } catch (...) {
                return Value();
            }
        case DataType::DOUBLE:
            try {
                return Value(std::stod(str));
            } catch (...) {
                return Value();
            }
        case DataType::STRING:
            return Value(str);
        default:
            return Value();
    }
}

std::unique_ptr<Table> DataLoader::loadFromCSV(const std::string& filename, const std::string& tablename) {
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
        DataType bestType = DataType::NULL_VALUE;
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


std::unique_ptr<Table> DataLoader::generateTestTable(const std::string& name, size_t rows, int seed = 42) {
        srand(seed);
        auto table = std::make_unique<Table>(name);
        
        table->addColumn("id", DataType::INTEGER);
        table->addColumn("value", DataType::INTEGER);
        table->addColumn("name", DataType::STRING);
        table->addColumn("score", DataType::DOUBLE);
        
        for (size_t i = 0; i < rows; ++i) {
            Row row;
            row.addValue(Value(static_cast<std::int64_t>(i + 1)));
            row.addValue(Value(static_cast<std::int64_t>(rand() % 100)));
            row.addValue(Value("Item_" + std::to_string(i % 50)));
            row.addValue(Value(static_cast<double>(rand()) / RAND_MAX * 100.0));
            table->addRow(row);
        }
        
        return table;
    }