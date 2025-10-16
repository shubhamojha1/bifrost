#include "storage/StorageEngine.h"
#include "storage/InMemoryStorageEngine.h"
#include <stdexcept>

namespace bifrost {
namespace storage {

std::unique_ptr<StorageEngine> StorageEngineFactory::create(const StorageConfig& config) {
    switch (config.mode) {
        case StorageConfig::Mode::IN_MEMORY:
            return createInMemory();
            
        case StorageConfig::Mode::LOG_BASED:
            return createLogBased(config.dataDirectory);
            
        case StorageConfig::Mode::PAGE_BASED:
            return createPageBased(config);
            
        case StorageConfig::Mode::LSM_TREEE:
            // TODO: Implement LSM tree storage engine in future phases
            throw std::runtime_error("LSM Tree storage engine not yet implemented");
            
        default:
            throw std::runtime_error("Unknown storage mode");
    }
}

std::unique_ptr<StorageEngine> StorageEngineFactory::createInMemory() {
    return std::make_unique<InMemoryStorageEngine>();
}

std::unique_ptr<StorageEngine> StorageEngineFactory::createLogBased(const std::string& dataDir) {
    // TODO: Implement LogStorageEngine in future phases
    throw std::runtime_error("Log-based storage engine not yet implemented");
}

std::unique_ptr<StorageEngine> StorageEngineFactory::createPageBased(const StorageConfig& config) {
    // TODO: Implement PagedStorageEngine in future phases
    throw std::runtime_error("Page-based storage engine not yet implemented");
}

} // namespace storage
} // namespace bifrost
