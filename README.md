# Bifrost: 

A modern C++ implementation of hash join algorithms commonly used in database engines, demonstrating various join operations and hash table optimizations.

![Hash Join Visualization](./assets/uqn12o5d.png)

## Features

- **Join Operations**
  - Inner Join
  - Left Outer Join
  - Right Outer Join
  - Full Outer Join

- **Hash Table Implementation**
  - Multiple collision resolution strategies
  - Configurable hash functions (std::hash, MurmurHash3)
  - Performance statistics and profiling

- **Data Support**
  - Integer, Double, String types
  - NULL value handling
  - CSV file import/export

## Performance Features

- Dynamic hash table resizing
- Collision handling strategies:
  - Chaining
  - Linear Probing
- Built-in profiling and statistics
- Memory usage tracking

## Getting Started

### Prerequisites
- C++17 compatible compiler
- Standard library support

### Building
```bash
g++ -std=c++17 -Iinclude bifrost.cpp src/*.cpp -o bifrost
```

### Running Tests
```bash
./bifrost --demo      # Basic usage demonstration
./bifrost --advanced  # Advanced features
./bifrost --benchmark # Performance benchmarks
```

## Project Structure

```
bifrost/
├── include/          # Header files
│   ├── CustomHashTable.h
│   ├── JoinEngine.h
│   └── ...
├── src/             # Implementation files
│   ├── join_engine.cpp
│   ├── custom_hash_table.hpp
│   └── ...
└── assets/          # Resources and examples
    └── csv/         # Sample data files
```

## Example Usage

```cpp
// Create join engine
JoinEngine engine;

// Load tables
auto students = loader.loadFromCSV("students.csv", "Students");
auto grades = loader.loadFromCSV("grades.csv", "Grades");

// Perform join
auto result = engine.hashJoin(
    *students, "StudentID",
    *grades, "StudentID",
    JoinType::INNER,
    CollisionStrategy::CHAINING
);
```

## Performance Metrics

The engine includes comprehensive performance monitoring:
- Build and probe phase timing
- Memory usage tracking
- Hash table statistics
- Join selectivity metrics

## Future Work
- Support for nested loop joins, sort-merge joins, partitioned hash joins
- Query processing engine (SQL parser, optimizer, execution engine)
- Concurrency control (multi-threading, transaction manager, lock manager)
- recovery and durability features (write-ahead logging, checkpointing)
- distributed join algorithms (map-reduce style joins, distributed hash joins)
- Integration with existing database systems (PostgreSQL, SQLite)
- Support for more complex data types (JSON, XML, etc.)

# Storage Engine
## In-Memory Engine -> unordered map
## Log-BasedStorage Engine -> Append only
## LSMStorage Engine -> LSM Tree
## PagedStorage Engine -> B+ Tree