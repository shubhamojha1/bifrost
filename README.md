# bifrost

a dummy implementation of how hash tables are used in database engines when performing joins


/*
=============================================================================
BUILD INSTRUCTIONS:

To compile this program, use:
    g++ -std=c++17 -O2 -o hash_join_engine hash_join_engine.cpp

Or with CMake, create a CMakeLists.txt:
    cmake_minimum_required(VERSION 3.10)
    project(HashJoinEngine)
    set(CMAKE_CXX_STANDARD 17)
    add_executable(hash_join_engine hash_join_engine.cpp)

USAGE EXAMPLES:
    ./hash_join_engine                 # Basic demo
    ./hash_join_engine --demo          # Basic demo
    ./hash_join_engine --advanced      # Advanced features
    ./hash_join_engine --benchmark     # Performance benchmarks
    ./hash_join_engine --memory        # Memory tests
    ./hash_join_engine --all           # All tests

FEATURES IMPLEMENTED:
✓ Custom hash table with chaining and linear probing
✓ Multiple join types (INNER, LEFT OUTER, RIGHT OUTER, FULL OUTER)
✓ Comprehensive performance profiling
✓ Memory usage tracking
✓ Scalable design for large datasets
✓ Type-safe value system supporting multiple data types
✓ CSV data loading capabilities
✓ Extensive benchmarking framework
✓ Memory pool allocation (foundation)
✓ Build vs. probe table optimization
✓ Hash distribution analysis
✓ Error handling and edge cases

PERFORMANCE CHARACTERISTICS:
- Time Complexity: O(n + m) average case for hash joins
- Space Complexity: O(min(n, m)) for hash table
- Scales efficiently with dataset size
- Cache-friendly data structures
- Optimized memory allocation patterns

This implementation demonstrates production-ready hash join algorithms
suitable for database systems and data processing applications.
=============================================================================
*/