#include <algorithm>
#include "JoinEngine.h"
#include "HashFunction.h"

// struct ValueHasher {
//     std::size_t operator()(const Value& v) const {
//         return std::visit([](const auto& val) -> std::size_t {
//             using T = std::decay_t<decltype(val)>;
//             if constexpr (std::is_same_v<T, std::monostate>) {
//                 return 0;
//             } else if constexpr (std::is_same_v<T, std::string>) {
//                 return std::hash<std::string>{}(val);
//             } else if constexpr (std::is_same_v<T, std::int64_t>) {
//                 return std::hash<std::int64_t>{}(val);
//             } else if constexpr (std::is_same_v<T, double>) {
//                 return std::hash<double>{}(val);
//             }
//             return 0;
//         }, v.data);
//     }
// };

std::pair<const Table *, const Table *> JoinEngine::chooseBuildProbe(const Table &left, const Table &right)
{
    if (left.rowCount() <= right.rowCount())
    {
        return {&left, &right};
    }
    else
    {
        return {&right, &left};
    }
}

Row JoinEngine::combineRows(const Row &left, const Row &right)
{
    std::vector<Value> combined;
    combined.reserve(left.size() + right.size());

    for (const auto &value : left.getValues())
    {
        combined.push_back(value);
    }

    for (const auto &value : right.getValues())
    {
        combined.push_back(value);
    }

    return Row(std::move(combined));
}

Row JoinEngine::createNullRow(size_t columnCount)
{
    std::vector<Value> nullValues(columnCount);
    return Row(std::move(nullValues));
}

std::unique_ptr<Table> JoinEngine::hashJoin(
    const Table &leftTable,
    const std::string &leftColumn,
    const Table &rightTable,
    const std::string &rightColumn,
    JoinType joinType,
    CollisionStrategy strategy)
{
    profiler_.startProfiling();

    auto leftColIdx = leftTable.getColumnIndex(leftColumn);
    auto rightColIdx = rightTable.getColumnIndex(rightColumn);

    if (!leftColIdx || !rightColIdx)
    {
        throw std::runtime_error("Join column not found!");
    }

    auto [buildTable, probeTable] = chooseBuildProbe(leftTable, rightTable);
    bool leftIsBuild = (buildTable == &leftTable);

    size_t buildColIdx = leftIsBuild ? *leftColIdx : *rightColIdx;
    size_t probeColIdx = leftIsBuild ? *rightColIdx : *leftColIdx;

    // Create result with combined schema
    auto result = std::make_unique<Table>("JoinResult");

    // Add columns from left table
    for (const auto &col : leftTable.getSchema())
    {
        result->addColumn("L_" + col.name, col.type);
    }

    // Add columns from right table
    for (const auto &col : rightTable.getSchema())
    {
        result->addColumn("R_" + col.name, col.type);
    }

    // BUILD PHASE
    // ValueHasher hasher;
    MurmurValueHasher hasher;
    CustomHashTable<Value, size_t, MurmurValueHasher> hashTable(
        buildTable->rowCount() * 2,
        strategy,
        hasher);

    // Build phase
    for (size_t i = 0; i < buildTable->rowCount(); ++i)
    {
        const Value &joinKey = buildTable->getRow(i)[buildColIdx];
        hashTable.insert(joinKey, i);
    }

    profiler_.markBuildComplete();
    profiler_.recordHashStats(hashTable.getStats());

    // PROBE PHASE
    std::vector<bool> buildMatched(buildTable->rowCount(), false);

    for (size_t probeIdx = 0; probeIdx < probeTable->rowCount(); ++probeIdx)
    {
        const Value &probeKey = probeTable->getRow(probeIdx)[probeColIdx];
        auto matchingBuildIndices = hashTable.find(probeKey);

        if (!matchingBuildIndices.empty())
        {
            for (size_t buildIdx : matchingBuildIndices)
            {
                buildMatched[buildIdx] = true;

                const Row &leftRow = leftIsBuild ? buildTable->getRow(buildIdx) : probeTable->getRow(probeIdx);
                const Row &rightRow = leftIsBuild ? probeTable->getRow(probeIdx) : buildTable->getRow(buildIdx);

                result->addRow(combineRows(leftRow, rightRow));
            }
        }
        else if (joinType == JoinType::LEFT_OUTER || joinType == JoinType::FULL_OUTER)
        {
            if (!leftIsBuild)
            {
                const Row &leftRow = probeTable->getRow(probeIdx);
                const Row nullRightRow = createNullRow(rightTable.columnCount());
                result->addRow(combineRows(leftRow, nullRightRow));
            }
        }
    }

    // Handle unmatched build table rows for outer joins
    if (joinType == JoinType::RIGHT_OUTER || joinType == JoinType::FULL_OUTER)
    {
        for (size_t buildIdx = 0; buildIdx < buildTable->rowCount(); ++buildIdx)
        {
            if (!buildMatched[buildIdx])
            {
                if (leftIsBuild)
                {
                    const Row &leftRow = buildTable->getRow(buildIdx);
                    const Row nullRightRow = createNullRow(rightTable.columnCount());
                    result->addRow(combineRows(leftRow, nullRightRow));
                }
                else
                {
                    const Row nullLeftRow = createNullRow(leftTable.columnCount());
                    const Row &rightRow = buildTable->getRow(buildIdx);
                    result->addRow(combineRows(nullLeftRow, rightRow));
                }
            }
        }
    }

    profiler_.markProbeComplete();
    profiler_.recordResults(result->rowCount(), leftTable.rowCount() * rightTable.rowCount());
    profiler_.stopProfiling();

    return result;
}

const Profiler &JoinEngine::getProfiler() const
{
    return profiler_;
}