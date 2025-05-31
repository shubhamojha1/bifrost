#pragma once
#include <vector>
#include "Value.h"

class Row {
private:
    std::vector<Value> values_;

public:
    Row() = default;
    Row(std::vector<Value> values) : values_(std::move(values)) {}
    
    void addValue(const Value& value);
    size_t size() const;
    const Value& getValue(size_t index) const;
    std::string toString() const;    const Value& operator[](size_t index) const;
    Value& operator[](size_t index);
    const std::vector<Value>& getValues() const { return values_; }
};