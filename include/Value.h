#pragma once
#include <variant>
#include <string>
#include <cstdint>

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

    Value();
    Value(std::int64_t i);
    Value(double d);
    Value(const std::string& s);

    DataType getType() const;
    bool isNull() const;
    std::string toString() const;

    bool operator==(const Value& other) const;
    bool operator<(const Value& other) const;
};