#include <iostream>
#include <cstdint>
#include <variant>
#include <vector>
#include <optional>

using DataValue = std::variant<std::int64_t, double, std::string, std::monostate>;

enum class DataType {
    INTEGER,
    DOUBLE,
    STRING,
    NULL_VALUE
};

struct ColumnInfo {
    std::string name;
    DataType type;
    size_t index;
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
        if (isNull()) return "NULL";
        return std::visit([](const auto& v) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::monostate>) {
                return "NULL";
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                return v;
            } else {
                return std::to_string(v);
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
        Row(std::vector<Value> values): values_(std::move(values)){}

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
            for(size_t i=0;i<values_.size();++i){
                if(i>0) result += ", ";
                result += values_[i].toString();
            }
            result += ")";
            return result;
        }
};

class Table {
    private:
        std::vector<ColumnInfo> schema_;
        std::vector<Row> rows_;
        std::string name_;

    public:
        Table(const std::string& name): name_(name) {};

        void addColumn(const std::string& name, DataType type) {
            schema_.push_back({name, type, schema_.size()});
        }

        void addRow(const Row& row) {
            if(row.size() != schema_.size()){
                throw std::runtime_error("Row size doesn't match schema");
            }
            rows_.push_back(row);
        }

        const Row& getRow(size_t index) const {
            return rows_[index];
        }

        size_t rowCount() const { return rows_.size(); }
        size_t columnCount() const { return schema_.size(); }

        const std::vector<ColumnInfo>& getSchema() const { return schema_; }
        const std::vector<Row>& getRows() const { return rows_; }
        const std::string& getName() const { return name_; }

        std::optional<size_t> getColumnIndex(const std::string& colName) const {
            for (const auto& col: schema_) {
                if (col.name == colName) return col.index;
            }
            return std::nullopt;
        }
        
        void clear() {
            rows_.clear();
        }

        // Memory estimation
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