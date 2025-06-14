// to generate profile for the entire join operation
#include <chrono>
#include "CustomHashTable.h"
class Profiler{
    public:
        struct ProfileData {
            std::chrono::nanoseconds buildTime{0};
            std::chrono::nanoseconds probeTime{0};
            std::chrono::nanoseconds totalTime{0};
            size_t memoryUsage = 0;
            size_t peakMemoryUsage = 0;
            HashTableStats hashStats;
            size_t resultRows = 0;
            double selectivity = 0.0;
        };

        void startProfiling();

        void markBuildComplete();

        void markProbeComplete();

        void recordHashStats(const HashTableStats& stats);

        void recordResults(size_t resultRows, size_t totalPossibleRows);

        void stopProfiling();

        const ProfileData& getData() const;

        void printReport() const;

    private:
        std::chrono::high_resolution_clock::time_point startTime_;
        ProfileData data_;
        bool profiling_ = false;
};