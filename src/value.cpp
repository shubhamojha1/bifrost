#include "value.h"
#include <type_traits>

Value::Value() : data(std::monostate{}) {}
Value::Value(std::int64_t i) : data(i) {}
Value::Value(double d) : data(d) {}
Value::Value(const std::string& s) : data(s) {}

DataType Value::getType() const {
    return static_cast<DataType>(data.index());
}

bool Value::isNull() const {
    return std::holds_alternative<std::monostate>(data);
}

std::string Value::toString() const {
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

bool Value::operator==(const Value& other) const {
    return data == other.data;
}

bool Value::operator<(const Value& other) const {
    return data < other.data;
}