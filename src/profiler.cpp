#include <chrono>
#include "Profiler.h"
#include <iostream>
#include <iomanip>

void Profiler::startProfiling() {
    profiling_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
    data_ = Profiler::ProfileData{};
}

void Profiler::markBuildComplete() {
    if(!profiling_) return;
    auto now = std::chrono::high_resolution_clock::now();
    data_.buildTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime_);
}

void Profiler::markProbeComplete() {
    if(!profiling_) return;
    auto now = std::chrono::high_resolution_clock::now();
    data_.probeTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime_) - data_.buildTime;
    data_.totalTime = data_.buildTime + data_.probeTime;
}

void Profiler::recordHashStats(const HashTableStats& stats) {
    data_.hashStats = stats;
    data_.memoryUsage = stats.memoryUsage;
    data_.peakMemoryUsage = std::max(data_.peakMemoryUsage, stats.memoryUsage);
}

void Profiler::recordResults(size_t resultRows, size_t totalPossibleRows){
    data_.resultRows = resultRows;
    data_.selectivity = totalPossibleRows > 0 ?
                            static_cast<double>(resultRows) / totalPossibleRows : 0.0;
}

void Profiler::stopProfiling() {
    profiling_ = false;
}

const Profiler::ProfileData& Profiler::getData() const { return data_; }

void Profiler::printReport() const {
    if (!profiling_ && data_.totalTime.count() == 0) {
            std::cout << "No profiling data available.\n";
            return;
        }

    std::cout << "\n=== Performance Report ===\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Build Time:     " << data_.buildTime.count() / 1e6 << " ms\n";
        std::cout << "Probe Time:     " << data_.probeTime.count() / 1e6 << " ms\n";
        std::cout << "Total Time:     " << data_.totalTime.count() / 1e6 << " ms\n";
        std::cout << "Result Rows:    " << data_.resultRows << "\n";
        std::cout << "Selectivity:    " << data_.selectivity * 100 << "%\n";
        
        std::cout << "\n=== Hash Table Statistics ===\n";
        std::cout << "Total Buckets:  " << data_.hashStats.totalBuckets << "\n";
        std::cout << "Used Buckets:   " << data_.hashStats.usedBuckets << "\n";
        std::cout << "Load Factor:    " << data_.hashStats.loadFactor << "\n";
        std::cout << "Collisions:     " << data_.hashStats.collisions << "\n";
        std::cout << "Avg Chain Len:  " << data_.hashStats.avgChainLength << "\n";
        std::cout << "Max Chain Len:  " << data_.hashStats.maxChainLength << "\n";
        std::cout << "Memory Usage:   " << data_.hashStats.memoryUsage / 1024 << " KB\n";
        std::cout << "========================\n\n";
}