#include <iostream>
#include <cstdint>
#include <variant>

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

        Value():
            data(std::monostate{}){}
        
        Value(std::int64_t i):
            data(i){}
        
        Value(double d):
            data(d){}

        Value(const std::string& s):
            data(s){}

        DataType getType() const {
            return static_cast<DataType>(data.index());
        }

        bool isNull() const {
            return std::holds_alternative<std::monostate>(data);
        }

        std::string toString() const {

        }

        bool operator==(const Value& other) const {
            return data == other.data;
        }

        bool operator<(const Value& other) const {
            return data < other.data;
        }
};

int main(int argc, char *argv[]){
    // take csv inputs table1.csv and table2.csv
    // or, some demo data
    try {
        std::cout<< "Database Query Processing System\n";
        std::cout<< "=================================\n\n";

        if (argc > 1){
            std::string arg = argv[1];
            std::cout << arg;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}