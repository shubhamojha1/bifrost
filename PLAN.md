# Bifrost: Full-Featured Database Management System

## Architecture Overview

Bifrost will follow a layered architecture with pluggable components:

```
┌─────────────────────────────────────────────────────┐
│         Client Layer (SQL Interface)                │
├─────────────────────────────────────────────────────┤
│    Query Processing Layer (Parser → Optimizer)      │
├─────────────────────────────────────────────────────┤
│      Execution Engine (Operators + Join Engine)     │
├─────────────────────────────────────────────────────┤
│   Transaction Manager (MVCC + Lock Manager)         │
├─────────────────────────────────────────────────────┤
│  Storage Engine Interface (Pluggable Backends)      │
│    ├─ In-Memory  ├─ File-Based  ├─ Page-Based       │
│    └─ LSM-Tree (RocksDB-style)                      │
├─────────────────────────────────────────────────────┤
│     Recovery & Logging (WAL + Checkpointing)        │
├─────────────────────────────────────────────────────┤
│   Consensus Layer (Raft/Paxos for distributed TX)   │
└─────────────────────────────────────────────────────┘
```

## Phase 1: Storage Engine Foundation (Weeks 1-3)

**Goal:** Create pluggable storage engine interface with multiple backends

### 1.1 Storage Engine Interface
- Create `include/storage/StorageEngine.h` with abstract interface
- Define operations: `insert()`, `update()`, `delete()`, `scan()`, `seek()`
- Add `Page` abstraction for page-based systems
- Create `Tuple` class (more general than current `Row`)

**Key files to leverage:**
- Extend `Table.h`, `Row.h`, `Value.h` (already well-designed)
- Keep backward compatibility with existing hash join code

### 1.2 In-Memory Storage Engine
- Implement `InMemoryStorageEngine` (simplest - what you have now)
- Wrap existing `Table` class as storage backend
- Add index support using existing `CustomHashTable`

### 1.3 File-Based Storage (Append-Only Log)
- Implement `LogStorageEngine` 
- Sequential write for OLTP workloads
- Format: `[TxnID][Operation][Data]`
- Add compaction mechanism

### 1.4 Page-Based Storage Engine
- Implement `PagedStorageEngine` with buffer pool manager
- Page size: 4KB/8KB (configurable)
- Slotted page format for variable-length records
- Buffer pool with LRU/Clock eviction policies
- Directory: `include/storage/paged/` and `src/storage/paged/`

### 1.5 LSM-Tree Storage (Optional Future)
- Design interface compatible with RocksDB
- MemTable + SSTable architecture
- Bloom filters for lookups

**Configuration:** Runtime mode selection via `StorageConfig`:
```cpp
enum class StorageMode { IN_MEMORY, LOG_BASED, PAGE_BASED, LSM_TREE };
```

## Phase 2: Index Structures (Weeks 4-5)

**Goal:** Add efficient index support beyond hash tables

### 2.1 Index Interface
- Create `include/index/Index.h` abstract interface
- Operations: `insert()`, `remove()`, `search()`, `rangeScan()`

### 2.2 Hash Index (Already Have!)
- Refactor `CustomHashTable` as `HashIndex`
- Support for composite keys
- Keep existing collision strategies

### 2.3 B+ Tree Index
- Implement `BPlusTreeIndex` for range queries
- Order-preserving for `ORDER BY`, `GROUP BY`
- Variable fanout based on page size
- Directory: `include/index/btree/`

### 2.4 Bitmap Index (Optional)
- For low-cardinality columns
- Useful for analytical queries

## Phase 3: SQL Parser & Query Processing (Weeks 6-9)

**Goal:** Parse SQL into executable query plans

### 3.1 Lexer & Parser
- Use **flex/bison** or **ANTLR4** for SQL grammar
- Start with: `SELECT`, `INSERT`, `UPDATE`, `DELETE`
- Generate AST (Abstract Syntax Tree)
- Directory: `include/parser/` and `src/parser/`
- Files: `Lexer.h`, `Parser.h`, `AST.h`

### 3.2 Semantic Analyzer
- Type checking and validation
- Resolve table/column references
- Symbol table for scopes
- File: `SemanticAnalyzer.h`

### 3.3 Logical Plan
- Convert AST to logical operators
- Operators: Scan, Filter, Project, Join, Aggregate, Sort
- File: `LogicalPlan.h` with operator classes
- Keep existing `JoinEngine` for join execution

### 3.4 Basic Query Executor
- Volcano-style iterator model
- Each operator implements `next()` method
- Wire logical plan to existing join engine
- Directory: `include/executor/` and `src/executor/`

**SQL Support Progression:**
1. **Phase 3a:** SELECT with WHERE, simple predicates
2. **Phase 3b:** INSERT, UPDATE, DELETE
3. **Phase 3c:** JOINs (leverage existing hash join!)
4. **Phase 3d:** GROUP BY, HAVING, aggregates (SUM, COUNT, AVG, etc.)
5. **Phase 3e:** ORDER BY, LIMIT, OFFSET
6. **Phase 3f:** Subqueries
7. **Phase 3g:** Window functions, CTEs

## Phase 4: Query Optimizer (Weeks 10-12)

**Goal:** Transform logical plans into efficient physical plans

### 4.1 Cost Model
- Statistics collection: cardinality, selectivity
- Cost estimation for operators
- File: `include/optimizer/CostModel.h`

### 4.2 Rule-Based Optimization
- Predicate pushdown
- Projection pruning
- Join reordering (heuristic-based)
- Constant folding
- File: `include/optimizer/RuleOptimizer.h`

### 4.3 Cost-Based Optimization
- Dynamic programming for join ordering (System R style)
- Choose physical operators (hash join vs nested loop vs sort-merge)
- Leverage existing hash join benchmarks for cost calibration
- File: `include/optimizer/CostBasedOptimizer.h`

### 4.4 Statistics Manager
- Histograms for data distribution
- Column statistics (min, max, distinct count)
- File: `include/optimizer/Statistics.h`

## Phase 5: Transaction Management (Weeks 13-16)

**Goal:** ACID transactions with isolation levels

### 5.1 Transaction Context
- Transaction ID generation
- Transaction states: BEGIN, ACTIVE, COMMITTED, ABORTED
- File: `include/txn/Transaction.h`

### 5.2 Lock Manager
- Implement 2PL (Two-Phase Locking)
- Lock types: Shared (S), Exclusive (X), Intent locks
- Deadlock detection (wait-for graph)
- File: `include/txn/LockManager.h`

### 5.3 MVCC (Multi-Version Concurrency Control)
- Timestamp-based versioning
- Version chains in storage engine
- Snapshot isolation support
- Files: `include/txn/MVCCManager.h`, `TupleVersion.h`

### 5.4 Isolation Levels
- Read Uncommitted
- Read Committed (default)
- Repeatable Read
- Serializable
- Configure via `SET TRANSACTION ISOLATION LEVEL`

### 5.5 Transaction Manager
- Coordinate lock manager + MVCC
- Integrate with executor
- File: `include/txn/TransactionManager.h`

## Phase 6: Recovery & Durability (Weeks 17-19)

**Goal:** Crash recovery and data persistence

### 6.1 Write-Ahead Logging (WAL)
- Log format: `[LSN][TxnID][Type][Before-Image][After-Image]`
- Log buffer with group commit
- File: `include/recovery/WAL.h`

### 6.2 ARIES Recovery Algorithm
- Analysis, Redo, Undo phases
- Log sequence numbers (LSN)
- Checkpoint mechanism
- File: `include/recovery/RecoveryManager.h`

### 6.3 Checkpointing
- Fuzzy checkpointing (don't block writers)
- Periodic background checkpoints
- File: `include/recovery/CheckpointManager.h`

### 6.4 Buffer Pool Integration
- Dirty page tracking
- Write policies: NO-STEAL, FORCE/NO-FORCE

## Phase 7: Client-Server Architecture (Weeks 20-22)

**Goal:** Network protocol and multi-client support

### 7.1 Protocol Design
- Wire protocol (consider PostgreSQL-compatible?)
- Message types: Query, Response, Error, etc.
- Binary protocol for efficiency
- File: `include/network/Protocol.h`

### 7.2 Server Component
- TCP server with thread pool
- Connection management
- Session state per client
- Files: `include/server/Server.h`, `ConnectionHandler.h`
- Entry point: `bifrost_server.cpp`

### 7.3 Client Library
- C++ client API
- Connection pooling
- File: `include/client/BifrostClient.h`

### 7.4 CLI Tool
- Interactive SQL shell (like `psql`)
- Command history, auto-completion
- File: `bifrost_cli.cpp`

## Phase 8: Distributed Transactions & Consensus (Weeks 23-28)

**Goal:** Distributed transaction support with Raft/Paxos

### 8.1 Distributed Transaction Coordinator
- 2PC (Two-Phase Commit) implementation
- Coordinator and Participant roles
- File: `include/distributed/TwoPhaseCommit.h`

### 8.2 Consensus Algorithm Research
- **Raft:** Simpler, better understandability
- **Paxos:** More flexible, proven theory
- **Recommendation:** Start with **Raft** (easier to implement correctly)

### 8.3 Raft Implementation
- Leader election
- Log replication
- Safety guarantees
- Directory: `include/distributed/raft/`
- Files: `RaftNode.h`, `RaftLog.h`, `RaftStateMachine.h`

### 8.4 Replicated State Machine
- Apply committed log entries to local storage
- Snapshot mechanism for log compaction
- File: `include/distributed/StateMachine.h`

### 8.5 Distributed Query Execution
- Query routing to appropriate node
- Distributed joins (partitioned hash join)
- File: `include/distributed/DistributedExecutor.h`

## Phase 9: Advanced Features (Weeks 29-35)

### 9.1 Additional Join Algorithms
- Nested Loop Join (already implicit)
- Sort-Merge Join
- Partitioned Hash Join (for large datasets)
- Leverage existing `JoinEngine` benchmarks

### 9.2 Parallel Execution
- Intra-query parallelism
- Thread pool for operators
- Parallel hash join, parallel sort
- File: `include/executor/ParallelExecutor.h`

### 9.3 Advanced Data Types
- JSON support
- Array types
- Spatial types (PostGIS-style)
- Full-text search

### 9.4 Catalog System
- System tables for metadata
- Schema versioning
- File: `include/catalog/Catalog.h`

### 9.5 Views & Stored Procedures
- View creation and materialization
- Stored procedure engine
- Trigger support

### 9.6 Compression
- Page-level compression
- Dictionary encoding for strings
- Run-length encoding

## Phase 10: Testing & Benchmarking (Ongoing)

### 10.1 Extend Existing Benchmarks
- Keep current `BenchmarkSuite.h`
- Add TPC-C, TPC-H benchmarks
- YCSB for key-value workloads

### 10.2 Testing Framework
- Unit tests for each component
- Integration tests for end-to-end flows
- Use Google Test or Catch2

### 10.3 Correctness Testing
- Jepsen-style testing for distributed correctness
- Transaction anomaly detection
- Crash recovery testing

## Project Structure

```
bifrost/
├── include/
│   ├── storage/           # Storage engines
│   │   ├── StorageEngine.h
│   │   ├── InMemoryEngine.h
│   │   ├── PagedEngine.h
│   │   └── paged/         # Buffer pool, pages
│   ├── index/             # Index structures
│   │   ├── Index.h
│   │   ├── HashIndex.h
│   │   └── btree/
│   ├── parser/            # SQL parsing
│   │   ├── Lexer.h
│   │   ├── Parser.h
│   │   └── AST.h
│   ├── optimizer/         # Query optimization
│   │   ├── Optimizer.h
│   │   ├── CostModel.h
│   │   └── Statistics.h
│   ├── executor/          # Query execution
│   │   ├── Executor.h
│   │   └── operators/
│   ├── txn/              # Transactions
│   │   ├── Transaction.h
│   │   ├── LockManager.h
│   │   └── MVCCManager.h
│   ├── recovery/         # WAL & recovery
│   │   ├── WAL.h
│   │   └── RecoveryManager.h
│   ├── network/          # Client-server
│   │   ├── Protocol.h
│   │   └── Server.h
│   ├── distributed/      # Distributed systems
│   │   ├── raft/
│   │   └── TwoPhaseCommit.h
│   ├── catalog/          # System catalog
│   │   └── Catalog.h
│   └── [existing files]  # Keep all current headers
├── src/
│   ├── storage/
│   ├── index/
│   ├── parser/
│   ├── optimizer/
│   ├── executor/
│   ├── txn/
│   ├── recovery/
│   ├── network/
│   ├── distributed/
│   ├── catalog/
│   └── [existing files]  # Keep all current implementations
├── tests/                # Test suites
│   ├── unit/
│   ├── integration/
│   └── benchmarks/
├── tools/                # Utilities
│   ├── bifrost_cli.cpp   # Interactive shell
│   └── bifrost_server.cpp # Server daemon
├── docs/                 # Documentation
├── bifrost.cpp           # Keep for backward compatibility
└── CMakeLists.txt        # Build system (upgrade from manual g++)
```

## Key Design Principles

1. **Modularity:** Each layer has clean interfaces
2. **Pluggability:** Storage engines, indexes, and consensus algorithms are swappable
3. **Backward Compatibility:** Existing hash join code remains functional
4. **Incremental Development:** Each phase produces working software
5. **Configuration-Driven:** Runtime selection of storage modes, isolation levels, etc.
6. **Performance Monitoring:** Extend existing `Profiler` throughout system

## Configuration System

Create `include/config/BifrostConfig.h`:
```cpp
struct BifrostConfig {
    StorageMode storageMode = StorageMode::IN_MEMORY;
    IsolationLevel isolationLevel = IsolationLevel::READ_COMMITTED;
    bool enableWAL = true;
    size_t bufferPoolSize = 128 * 1024 * 1024; // 128MB
    size_t pageSize = 8192; // 8KB
    ConsensusMode consensusMode = ConsensusMode::RAFT;
    // ... more config options
};
```

## Migration Path

1. **Week 1-12:** Can still use existing `bifrost.cpp` for hash join demos
2. **Week 13+:** New `bifrost_server` runs as daemon, old code accessible via SQL
3. **Week 23+:** Enable distributed mode via config flag

## Dependencies to Consider

- **Parser:** ANTLR4 or hand-written recursive descent
- **Networking:** Boost.Asio or ASIO standalone
- **Testing:** Google Test
- **Logging:** spdlog
- **Build:** CMake (replace manual g++ commands)
- **Serialization:** Protocol Buffers for distributed messages

## Success Metrics

- [ ] Pass TPC-C benchmark
- [ ] Pass TPC-H queries
- [ ] ACID compliance verification
- [ ] Jepsen testing for distributed correctness
- [ ] Handle 10K+ concurrent connections
- [ ] Sub-millisecond transaction latency
- [ ] Crash recovery < 10 seconds

## Implementation TODOs

### Phase 1: Storage Engine Foundation
- [ ] Design and implement pluggable storage engine interface with abstract base class
- [ ] Wrap existing Table class as InMemoryStorageEngine implementation
- [ ] Implement append-only LogStorageEngine with compaction
- [ ] Build PagedStorageEngine with buffer pool manager and slotted pages

### Phase 2: Index Structures
- [ ] Create abstract Index interface for different index types
- [ ] Refactor CustomHashTable into HashIndex with composite key support
- [ ] Implement B+ Tree index for range queries and ordering

### Phase 3: SQL Parser & Query Processing
- [ ] Build SQL lexer and parser (flex/bison or ANTLR4) for basic DML statements
- [ ] Implement AST generation and semantic analyzer with type checking
- [ ] Create logical plan representation with operator classes
- [ ] Build Volcano-style query executor integrating with existing JoinEngine

### Phase 4: Query Optimizer
- [ ] Implement statistics collection and cost estimation model
- [ ] Build rule-based and cost-based query optimizer

### Phase 5: Transaction Management
- [ ] Create Transaction class with ID generation and state management
- [ ] Implement 2PL lock manager with deadlock detection
- [ ] Build MVCC manager with timestamp-based versioning and snapshot isolation

### Phase 6: Recovery & Durability
- [ ] Implement Write-Ahead Logging with log buffer and group commit
- [ ] Build ARIES-style recovery manager with analysis/redo/undo phases

### Phase 7: Client-Server Architecture
- [ ] Design wire protocol and implement TCP server with connection handling
- [ ] Build C++ client library and CLI tool for interactive SQL

### Phase 8: Distributed Transactions & Consensus
- [ ] Implement Raft consensus algorithm for distributed transactions
- [ ] Build 2PC coordinator and distributed query execution

### Phase 9: Advanced Features
- [ ] Implement system catalog for metadata and schema management
- [ ] Add sort-merge join and partitioned hash join algorithms
- [ ] Implement intra-query parallelism with thread pool

### Phase 10: Testing & Benchmarking
- [ ] Set up comprehensive testing with TPC benchmarks and correctness testing

This plan provides a roadmap for 6-9 months of development, building incrementally from your solid hash join foundation into a production-quality DBMS.

