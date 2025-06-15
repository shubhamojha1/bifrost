#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Value.h"
#include "Table.h"

class DataLoader {
    private:
        std::vector<std::string> split(const std::string& str, char delimiter);
        DataType inferType(const std::string& value);
        Value parseValue(const std::string& str, DataType type);

    public:
        std::unique_ptr<Table> loadFromCSV(const std::string& filename, const std::string& tablename);
        std::unique_ptr<Table> generateTestTable(const std::string& name, size_t rows, int seed);
};