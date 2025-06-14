#include "Profiler.h"

class BenchmarkSuite {
    public:
        void runHashTableBenchmark();
        void runJoinTypeBenchmark();
        void runScalabilityBenchmark();

    private:
        Profiler::ProfileData benchmarkJoinWithStrategy(size_t tableSize, CollisionStrategy strategy);
};