#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

enum class CollisionStrategy {
    CHAINING,
    LINEAR_PROBING
};

struct HashTableStats {
    size_t totalBuckets = 0;
    size_t usedBuckets = 0;
    size_t totalEntries = 0;
    size_t collisions = 0;
    double loadFactor = 0.0;
    double avgChainLength = 0.0;
    size_t maxChainLength = 0;
    size_t memoryUsage = 0;
};

template<typename Key, typename Value, typename Hash = std::hash<Key>>
class CustomHashTable {
    private:
        struct Entry {
            Key key;
            std::vector<Value> values; // support for non-unique keys
            std::unique_ptr<Entry> next; // for chaining
            bool occupied = false; // for linear probing

            Entry() = default;
            Entry(const Key& k, const Value& v) : key(k), occupied(true) {
                values.push_back(v);
            }
        };

        std::vector<std::unique_ptr<Entry>> table_;
        size_t size_;
        size_t capacity_;
        CollisionStrategy strategy_;
        Hash hasher_;
        HashTableStats stats_;

        static constexpr double LOAD_FACTOR_THRESHOLD = 0.75;

        void resize();
        size_t hash(const Key& key) const;
        size_t linearProbe(size_t startPos, const Key& key) const;

    public:
        CustomHashTable(size_t initialCapacity = 16, CollisionStrategy strategy = CollisionStrategy::CHAINING);
        // CustomHashTable(size_t capacity, CollisionStrategy strategy, Hash hasher = Hash())
        //     : size_(0), capacity_(capacity), strategy_(strategy), hasher_(std::move(hasher)) {
        //     table_.resize(capacity);
        // }
        void insert(const Key& key, const Value& value);
        std::vector<Value> find(const Key& key) const;
        bool contains(const Key& key) const;
        void clear();
        size_t size() const;
        bool empty() const;
        void updateStats();
        const HashTableStats& getStats() const;
        size_t estimateMemoryUsage() const;
};

// #include "../src/custom_hash_table.hpp"