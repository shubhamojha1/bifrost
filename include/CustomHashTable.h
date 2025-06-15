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
        size_t hash(const Key& key) const {
            return hasher_(key) % capacity_;
        }
        size_t linearProbe(size_t startPos, const Key& key) const {
            size_t currentPos = startPos;
            for (size_t i = 0; i < capacity_; ++i) {
                currentPos = (startPos + i) % capacity_;
                if (!table_[currentPos] || table_[currentPos]->key == key) {
                    return currentPos;
                }
            }
            return capacity_; // Table is full
        }

    public:
        CustomHashTable(size_t initialCapacity = 16, CollisionStrategy strategy = CollisionStrategy::CHAINING)
            : size_(0), capacity_(initialCapacity), strategy_(strategy), hasher_() {
            table_.resize(capacity_);
        }

        CustomHashTable(size_t capacity, CollisionStrategy strategy, Hash hasher)
            : size_(0), capacity_(capacity), strategy_(strategy), hasher_(std::move(hasher)) {
            table_.resize(capacity_);
        }

        void insert(const Key& key, const Value& value) {
            size_t index = hash(key);

            if (strategy_ == CollisionStrategy::CHAINING) {
                if (!table_[index]) {
                    table_[index] = std::make_unique<Entry>(key, value);
                } else {
                    auto current = table_[index].get();
                    while (current->next) {
                        current = current->next.get();
                    }
                    current->next = std::make_unique<Entry>(key, value);
                }
                size_++;
            } else {
                index = linearProbe(index, key);
                if (index < capacity_) {
                    if (!table_[index]) {
                        table_[index] = std::make_unique<Entry>(key, value);
                        size_++;
                    } else {
                        table_[index]->values.push_back(value);
                    }
                }
            }
            updateStats();
        }

        std::vector<Value> find(const Key& key) const {
            std::vector<Value> results;
            size_t index = hash(key);

            if (strategy_ == CollisionStrategy::CHAINING) {
                auto current = table_[index].get();
                while (current) {
                    if (current->key == key) {
                        results.insert(results.end(), current->values.begin(), current->values.end());
                    }
                    current = current->next.get();
                }
            } else {
                index = linearProbe(index, key);
                if (index < capacity_ && table_[index] && table_[index]->key == key) {
                    results = table_[index]->values;
                }
            }
            return results;
        }

        bool contains(const Key& key) const {
            return !find(key).empty();
        }

        void clear() {
            table_.clear();
            table_.resize(capacity_);
            size_ = 0;
            updateStats();
        }

        size_t size() const { return size_; }
        bool empty() const { return size_ == 0; }

        void updateStats() {
            stats_ = HashTableStats{};
            stats_.totalBuckets = capacity_;
            
            for (const auto& bucket : table_) {
                if (bucket) {
                    stats_.usedBuckets++;
                    size_t chainLength = 1;
                    auto current = bucket->next.get();
                    while (current) {
                        chainLength++;
                        current = current->next.get();
                    }
                    if (chainLength > 1) {
                        stats_.collisions += (chainLength - 1);
                    }
                    stats_.maxChainLength = std::max(stats_.maxChainLength, chainLength);
                }
            }
            
            stats_.totalEntries = size_;
            stats_.loadFactor = static_cast<double>(stats_.usedBuckets) / capacity_;
            stats_.avgChainLength = stats_.usedBuckets > 0 ? 
                static_cast<double>(size_) / stats_.usedBuckets : 0.0;
            stats_.memoryUsage = estimateMemoryUsage();
        }

        const HashTableStats& getStats() const {
            return stats_;
        }

        size_t estimateMemoryUsage() const {
            size_t total = sizeof(*this);
            total += capacity_ * sizeof(std::unique_ptr<Entry>);
            
            for (const auto& bucket : table_) {
                if (bucket) {
                    total += sizeof(Entry);
                    total += bucket->values.capacity() * sizeof(Value);
                    auto current = bucket->next.get();
                    while (current) {
                        total += sizeof(Entry);
                        total += current->values.capacity() * sizeof(Value);
                        current = current->next.get();
                    }
                }
            }
            return total;
        }
};