#include "Profiler.h"
#include "Table.h"
#include "Row.h"
#include <memory>
#include <string>
#include <utility>

enum class JoinType {
    INNER,
    LEFT_OUTER,
    RIGHT_OUTER,
    FULL_OUTER
};

class JoinEngine {
    private:
        Profiler profiler_;

        // choose which table should be built vs probe based on size
        std::pair<const Table*, const Table*> chooseBuildProbe(const Table& left, const Table& right); 

        Row combineRows(const Row& left, const Row& right);

        Row createNullRow(size_t columnCount);

    public:
        std::unique_ptr<Table> hashJoin(
        const Table& leftTable,
        const std::string& leftColumn,
        const Table& rightTable, 
        const std::string& rightColumn,
        JoinType joinType = JoinType::INNER,
        CollisionStrategy strategy = CollisionStrategy::CHAINING
    );

    std::unique_ptr<Table> nestedLoopJoin(
        const Table& outerTable,
        const std::string& outerColumn,
        const Table& innerTable,
        const std::string& innerColumn,
        JoinType joinType = JoinType::INNER,
        CollisionStrategy strategy = CollisionStrategy::CHAINING
    );

    const Profiler& getProfiler() const;
};