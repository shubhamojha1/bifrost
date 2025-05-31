#include "Row.h"

void Row::addValue(const Value& value) {
    values_.push_back(value);
}

size_t Row::size() const {
    return values_.size();
}

const Value& Row::getValue(size_t index) const {
    return values_[index];
}

std::string Row::toString() const {
    std::string result = "(";
    for(size_t i=0; i<values_.size(); ++i){
        if(i>0) result += ", ";
        result += values_[i].toString();
    }
    result += ")";
    return result;
}

const Value& Row::operator[](size_t index) const {
    return values_[index];
}

Value& Row::operator[](size_t index) {
    return values_[index];
}
