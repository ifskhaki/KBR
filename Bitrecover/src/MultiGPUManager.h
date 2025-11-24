#ifndef MULTI_GPU_MANAGER_H
#define MULTI_GPU_MANAGER_H

#include "bitrecover/types.h"
#include "KeySearchDevice.h"
#include "KeyFinder.h"
#include "DeviceManager.h"
#include "RandomKeyGenerator.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <string>

class MultiGPUManager {
public:
    struct GPUWorker {
        int deviceId;
        int physicalDeviceId;
        std::string deviceName;
        std::unique_ptr<KeySearchDevice> device;
        std::unique_ptr<KeyFinder> finder;
        std::thread workerThread;
        std::atomic<bool> running;
        std::atomic<uint64_t> keysProcessed;
        std::atomic<double> currentSpeed;
        std::atomic<uint64_t> freeMemory;
        std::atomic<uint64_t> totalMemory;
        uint64_t targets;
        secp256k1::uint256 startKey;
        secp256k1::uint256 currentKey;
        
        GPUWorker() : running(false), keysProcessed(0), currentSpeed(0.0),
                     freeMemory(0), totalMemory(0), targets(0) {}
    };
    
    using ResultCallback = std::function<void(const KeySearchResult&, int gpuId)>;
    using StatusCallback = std::function<void(const bitrecover::GPUStats&)>;
    
    MultiGPUManager();
    ~MultiGPUManager();
    
    // Initialize all GPUs
    bool initializeAllGPUs(const std::string& targetsFile,
                          const bitrecover::Config::GPUConfig& gpuConfig,
                          const bitrecover::Config::SearchConfig& searchConfig);
    
    // Start parallel search across all GPUs
    void startParallelSearch(const bitrecover::Config::SearchConfig& searchConfig);
    
    // Stop all GPU workers
    void stopAll();
    
    // Get current statistics
    std::vector<bitrecover::GPUStats> getStats() const;
    
    // Set callbacks
    void setResultCallback(ResultCallback callback);
    void setStatusCallback(StatusCallback callback);
    
    // Check if any GPU is still running
    bool isAnyRunning() const;
    
private:
    // Helper to get device name from device type
    std::string getDeviceTypeName(int deviceType) const;
    void workerThreadFunc(GPUWorker& worker, 
                         const bitrecover::Config::SearchConfig& searchConfig);
    void aggregateResults();
    void updateStatus();
    
    std::vector<GPUWorker> workers_;
    std::mutex resultsMutex_;
    std::vector<KeySearchResult> allResults_;
    
    ResultCallback resultCallback_;
    StatusCallback statusCallback_;
    
    std::atomic<bool> shouldStop_;
    std::thread statusThread_;
    
    uint64_t statusIntervalMs_;
    uint64_t lastStatusUpdate_;
    
    bitrecover::Config::SearchConfig searchConfig_;
};

#endif // MULTI_GPU_MANAGER_H

