#pragma once
#include "CustomHashTable.h"

template<typename Key, typename Value, typename Hash>
CustomHashTable<Key, Value, Hash>::CustomHashTable(size_t initialCapacity, CollisionStrategy strategy)
    : capacity_(initialCapacity), size_(0), strategy_(strategy) {
    table_.resize(capacity_);
    stats_.totalBuckets = capacity_;
}

template<typename Key, typename Value, typename Hash>
void CustomHashTable<Key, Value, Hash>::resize() {
    auto oldTable = std::move(table_);
    size_t oldCapacity = capacity_;

    capacity_ = capacity_ * 2;
    table_.clear();
    table_.resize(capacity_);
    size_ = 0;
    stats_.collisions = 0;

    // rehashing all entries
    for (auto& entry: oldTable) {
        if(strategy_ == CollisionStrategy::CHAINING) {
            while(entry){
                for (const auto& value: entry->values){
                    insert(entry->key, value);
                }
                entry = std::move(entry->next);
            }
        } else if (entry && entry->occupied) {
            for (const auto& value: entry->values) {
                insert(entry->key, value);
            }
        }
    }
}

template<typename Key, typename Value, typename Hash>
size_t CustomHashTable<Key, Value, Hash>::hash(const Key& key) const {
    return hasher_(key) % capacity_;
}

template<typename Key, typename Value, typename Hash>
size_t CustomHashTable<Key, Value, Hash>::linearProbe(size_t startPos, const Key& key) const {
    size_t pos = startPos;
    while (table_[pos] && table_[pos]->occupied && table_[pos]->key != key) {
        pos = (pos + 1) % capacity_;
        if (pos == startPos) break; // table full
    }
    return pos;
}

template<typename Key, typename Value, typename Hash>
void CustomHashTable<Key, Value, Hash>::insert(const Key& key, const Value& value) {
    if (static_cast<double>(size_) / capacity_ > LOAD_FACTOR_THRESHOLD) {
        resize();
    }

    size_t pos = hash(key);

    if (strategy_ == CollisionStrategy::CHAINING) {
        if (!table_[pos]) {
            table_[pos] = std::make_unique<Entry>(key, value);
            size_++;
        } else {
            // Check if key already exists in chain
            Entry* current = table_[pos].get();
            while (current) {
                if (current->key == key) {
                    current->values.push_back(value);
                    return;
                }
                if (!current->next) break;
                current = current->next.get();
            }
            // Add to end of chain
            current->next = std::make_unique<Entry>(key, value);
            stats_.collisions++;
            size_++;
        }
    } else { // LINEAR_PROBING
        pos = linearProbe(pos, key);
        if (!table_[pos]) {
            table_[pos] = std::make_unique<Entry>(key, value);
            size_++;
        } else if (table_[pos]->key == key) {
            table_[pos]->values.push_back(value);
        } else {
            stats_.collisions++;
            table_[pos] = std::make_unique<Entry>(key, value);
            size_++;
        }
    }

    updateStats();
}

template<typename Key, typename Value, typename Hash>
std::vector<Value> CustomHashTable<Key, Value, Hash>::find(const Key& key) const {
    size_t pos = hash(key);

    if (strategy_ == CollisionStrategy::CHAINING) {
        Entry* current = table_[pos].get();
        while (current) {
            if (current->key == key) {
                return current->values;
            }
            current = current->next.get();
        }
    } else { // LINEAR_PROBING
        pos = linearProbe(pos, key);
        if (table_[pos] && table_[pos]->occupied && table_[pos]->key == key) {
            return table_[pos]->values;
        }
    }

    return {};
}

template<typename Key, typename Value, typename Hash>
bool CustomHashTable<Key, Value, Hash>::contains(const Key& key) const {
    return !find(key).empty();
}

template<typename Key, typename Value, typename Hash>
void CustomHashTable<Key, Value, Hash>::clear() {
    table_.clear();
    table_.resize(capacity_);
    size_ = 0;
    stats_ = HashTableStats{};
    stats_.totalBuckets = capacity_;
}

template<typename Key, typename Value, typename Hash>
size_t CustomHashTable<Key, Value, Hash>::size() const { 
    return size_; 
}

template<typename Key, typename Value, typename Hash>
bool CustomHashTable<Key, Value, Hash>::empty() const { 
    return size_ == 0; 
}

template<typename Key, typename Value, typename Hash>
void CustomHashTable<Key, Value, Hash>::updateStats() {
    stats_.totalBuckets = capacity_;
    stats_.totalEntries = size_;
    stats_.loadFactor = static_cast<double>(size_) / capacity_;

    size_t usedBuckets = 0;
    size_t totalChainLength = 0;
    size_t maxChainLength = 0;

    for (const auto& entry : table_) {
        if (entry) {
            usedBuckets++;
            size_t chainLength = 0;
            Entry* current = entry.get();
            while (current) {
                chainLength++;
                current = current->next.get();
            }
            totalChainLength += chainLength;
            maxChainLength = std::max(maxChainLength, chainLength);
        }
    }

    stats_.usedBuckets = usedBuckets;
    stats_.avgChainLength = usedBuckets > 0 ? static_cast<double>(totalChainLength) / usedBuckets : 0.0;
    stats_.maxChainLength = maxChainLength;
    stats_.memoryUsage = estimateMemoryUsage();
}

template<typename Key, typename Value, typename Hash>
const HashTableStats& CustomHashTable<Key, Value, Hash>::getStats() const { 
    return stats_; 
}

template<typename Key, typename Value, typename Hash>
size_t CustomHashTable<Key, Value, Hash>::estimateMemoryUsage() const {
    size_t total = sizeof(*this);
    total += table_.capacity() * sizeof(std::unique_ptr<Entry>);

    for (const auto& entry : table_) {
        Entry* current = entry.get();
        while (current) {
            total += sizeof(Entry);
            total += current->values.capacity() * sizeof(Value);
            current = current->next.get();
        }
    }
    return total;
}
