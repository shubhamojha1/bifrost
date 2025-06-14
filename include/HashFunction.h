#pragma once
#include "Value.h"
#include "MurmurHash3.h"
#include "RainbowHash.h"
#include <functional>
#include <memory>

// Base hash function wrapper class
template<typename T>
class HashFunction {
public:
    virtual size_t operator()(const T& value) const = 0;
    virtual ~HashFunction() = default;
    virtual const char* getName() const = 0;
};

// // Standard hash implementation
// template<typename T>
// class StandardHasher : public HashFunction<T> {
// public:
//     size_t operator()(const T& value) const override {
//         return std::hash<T>{}(value);
//     }
    
//     const char* getName() const override {
//         return "StandardHash";
//     }
// };

// Specialization for Value type using standard hash
class ValueHasher : public HashFunction<Value> {
public:
    size_t operator()(const Value& v) const override {
        return std::visit([](const auto& val) -> std::size_t {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                return 0;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return std::hash<std::string>{}(val);
            } else if constexpr (std::is_same_v<T, std::int64_t>) {
                return std::hash<std::int64_t>{}(val);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::hash<double>{}(val);
            }
            return 0;
        }, v.data);
    }

    const char* getName() const override {
        return "ValueHasher";
    }
};

// MurmurHash implementation for Value type
class MurmurValueHasher : public HashFunction<Value> {
public:
    MurmurValueHasher(uint32_t seed = 0) : seed_(seed) {}

    size_t operator()(const Value& v) const override {
        return std::visit([this](const auto& val) -> std::size_t {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                return 0;
            } else if constexpr (std::is_same_v<T, std::string>) {
                uint64_t hash[2];
                MurmurHash3_x64_128(val.data(), val.size(), seed_, hash);
                return hash[0];
            } else if constexpr (std::is_same_v<T, std::int64_t>) {
                uint64_t hash[2];
                MurmurHash3_x64_128(&val, sizeof(val), seed_, hash);
                return hash[0];
            } else if constexpr (std::is_same_v<T, double>) {
                uint64_t hash[2];
                MurmurHash3_x64_128(&val, sizeof(val), seed_, hash);
                return hash[0];
            }
            return 0;
        }, v.data);
    }

    const char* getName() const override {
        return "MurmurHash3";
    }

private:
    uint32_t seed_;
};

// Factory for creating hash functions
template<typename T>
class HashFunctionFactory {
public:
    static std::unique_ptr<HashFunction<T>> create(const std::string& type = "standard", uint32_t seed = 0) {
        if (type == "murmur") {
            return std::make_unique<MurmurValueHasher>(seed);
        } else if(type == "valuehasher") {
            return std::make_unique<ValueHasher>();}
        // } else if (type == "standard") {
        //     return std::make_unique<StandardHasher<T>>();
        // }
        // Add more hash function types here
        
        throw std::runtime_error("Unknown hash function type: " + type);
    }
};

// Example of another hash function implementation
class MurmurHasher : public HashFunction<Value> {
public:
    size_t operator()(const Value& v) const override {
        // Implement MurmurHash here
        // This is just a placeholder - you'll need to implement the actual MurmurHash logic
        return 0;
    }
};