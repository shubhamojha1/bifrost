#include "Profiler.h"

class BenchmarkSuite {
    private:
        Profiler::ProfileData benchmarkJoinWithStrategy(size_t tableSize, CollisionStrategy strategy);

    public:
        void runHashTableBenchmark();
        void runJoinTypeBenchmark();
        void runScalabilityBenchmark();
};