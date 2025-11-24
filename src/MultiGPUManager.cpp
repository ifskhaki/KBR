#include "MultiGPUManager.h"
#include "Logger.h"
#include "util.h"
#include "AddressUtil.h"
#include "KeySearchTypes.h"
#include <algorithm>
#include <chrono>
#include <thread>

#ifdef BUILD_CUDA
#include "CudaKeySearchDevice.h"
#include <cuda_runtime.h>
#endif

#ifdef BUILD_OPENCL
#include "CLKeySearchDevice.h"
#endif

MultiGPUManager::MultiGPUManager() 
    : shouldStop_(false), statusIntervalMs_(1000), lastStatusUpdate_(0) {
}

MultiGPUManager::~MultiGPUManager() {
    stopAll();
}

bool MultiGPUManager::initializeAllGPUs(const std::string& targetsFile,
                                        const bitrecover::Config::GPUConfig& gpuConfig,
                                        const bitrecover::Config::SearchConfig& searchConfig) {
    // Get all available devices
    std::vector<DeviceManager::DeviceInfo> allDevices;
    try {
        allDevices = DeviceManager::getDevices();
    } catch (DeviceManager::DeviceManagerException& ex) {
        Logger::log(LogLevel::Error, "Failed to get devices: " + ex.msg);
        return false;
    }
    
    // Filter GPU devices (CUDA and OpenCL)
    std::vector<DeviceManager::DeviceInfo> gpuDevices;
    for (const auto& dev : allDevices) {
        if (dev.type == DeviceManager::DeviceType::CUDA || 
            dev.type == DeviceManager::DeviceType::OpenCL) {
            gpuDevices.push_back(dev);
        }
    }
    
    if (gpuDevices.empty()) {
        Logger::log(LogLevel::Error, "No GPU devices found (CUDA or OpenCL)");
        return false;
    }
    
    // Determine which GPUs to use
    std::vector<DeviceManager::DeviceInfo> devicesToUse;
    if (gpuConfig.useAllGPUs) {
        devicesToUse = gpuDevices;
    } else if (!gpuConfig.gpuIds.empty()) {
        for (int id : gpuConfig.gpuIds) {
            if (id >= 0 && id < static_cast<int>(gpuDevices.size())) {
                devicesToUse.push_back(gpuDevices[id]);
            }
        }
    } else {
        devicesToUse.push_back(gpuDevices[0]); // Default to first GPU
    }
    
    if (devicesToUse.empty()) {
        Logger::log(LogLevel::Error, "No valid GPUs selected");
        return false;
    }
    
    Logger::log(LogLevel::Info, "Initializing " + util::format(devicesToUse.size()) + " GPU(s)");
    
    // Create workers for each GPU
    workers_.clear();
    workers_.reserve(devicesToUse.size());
    
    for (size_t i = 0; i < devicesToUse.size(); ++i) {
        const auto& devInfo = devicesToUse[i];
        GPUWorker worker;
        
        worker.deviceId = static_cast<int>(i);
        worker.physicalDeviceId = devInfo.physicalId;
        worker.deviceName = devInfo.name;
        
        // Determine optimal parameters
        int threads = gpuConfig.threadsPerBlock;
        int blocks = gpuConfig.blocks;
        int points = gpuConfig.pointsPerThread;
        
        if (gpuConfig.autoOptimize && blocks == 0) {
            // Auto-optimize based on GPU compute units
            blocks = devInfo.computeUnits;
            if (threads == 0) {
                threads = 256; // Default
            }
        }
        
        if (blocks == 0) {
            blocks = 32; // Fallback
        }
        
        try {
            // Create device based on type
            if (devInfo.type == DeviceManager::DeviceType::CUDA) {
#ifdef BUILD_CUDA
                worker.device = std::unique_ptr<KeySearchDevice>(
                    new CudaKeySearchDevice(static_cast<int>(devInfo.physicalId), threads, points, blocks)
                );
#else
                Logger::log(LogLevel::Warning, "CUDA not compiled, skipping GPU " + util::format(i));
                continue;
#endif
            } else if (devInfo.type == DeviceManager::DeviceType::OpenCL) {
#ifdef BUILD_OPENCL
                worker.device = std::unique_ptr<KeySearchDevice>(
                    new CLKeySearchDevice(devInfo.physicalId, threads, points, blocks)
                );
#else
                Logger::log(LogLevel::Warning, "OpenCL not compiled, skipping GPU " + util::format(i));
                continue;
#endif
            } else {
                Logger::log(LogLevel::Warning, "Unknown device type, skipping GPU " + util::format(i));
                continue;
            }
            
            // Generate unique random start key for this GPU
            worker.startKey = RandomKeyGenerator::generateRandomStart(
                static_cast<uint32_t>(i), 
                static_cast<uint32_t>(devicesToUse.size())
            );
            
            // Determine compression
            int compression = PointCompressionType::UNCOMPRESSED;
            if (searchConfig.compression == "COMPRESSED") {
                compression = PointCompressionType::COMPRESSED;
            } else if (searchConfig.compression == "BOTH") {
                compression = PointCompressionType::BOTH;
            }
            
            // Create KeyFinder
            secp256k1::uint256 endKey = secp256k1::N - 1;
            secp256k1::uint256 stride(1);
            
            worker.finder = std::unique_ptr<KeyFinder>(
                new KeyFinder(worker.startKey, endKey, compression, 
                             worker.device.get(), stride)
            );
            
            // Set targets
            worker.finder->setTargets(targetsFile);
            
            // Get memory info (may fail for some devices)
            try {
                uint64_t freeMem, totalMem;
                worker.device->getMemoryInfo(freeMem, totalMem);
                worker.freeMemory = freeMem;
                worker.totalMemory = totalMem;
            } catch (...) {
                // Some devices may not support memory info
                worker.freeMemory = 0;
                worker.totalMemory = devInfo.memory;
            }
            
            // Get target count (approximate)
            // This would need to be passed or retrieved differently
            worker.targets = 0; // Will be set after initialization
            
            workers_.push_back(std::move(worker));
            
            Logger::log(LogLevel::Info, 
                "GPU " + util::format(i) + " (" + devInfo.name + ") initialized");
            
        } catch (KeySearchException& ex) {
            Logger::log(LogLevel::Error, 
                "Failed to initialize GPU " + util::format(i) + ": " + ex.msg);
            continue;
        }
    }
    
    if (workers_.empty()) {
        Logger::log(LogLevel::Error, "No GPUs successfully initialized");
        return false;
    }
    
    Logger::log(LogLevel::Info, 
        util::format(workers_.size()) + " GPU(s) ready for parallel search");
    
    return true;
}

void MultiGPUManager::startParallelSearch(const bitrecover::Config::SearchConfig& searchConfig) {
    shouldStop_ = false;
    searchConfig_ = searchConfig;
    statusIntervalMs_ = searchConfig.statusIntervalMs;
    
    // Start worker threads
    for (auto& worker : workers_) {
        worker.running = true;
        worker.workerThread = std::thread(
            &MultiGPUManager::workerThreadFunc, this, 
            std::ref(worker),
            std::ref(searchConfig_)
        );
    }
    
    // Start status update thread
    statusThread_ = std::thread([this]() {
        while (!shouldStop_ && isAnyRunning()) {
            updateStatus();
            std::this_thread::sleep_for(
                std::chrono::milliseconds(statusIntervalMs_)
            );
        }
    });
    
    // Aggregate results in main thread
    while (!shouldStop_ && isAnyRunning()) {
        aggregateResults();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void MultiGPUManager::workerThreadFunc(GPUWorker& worker,
                                      const bitrecover::Config::SearchConfig& searchConfig) {
    // Set device context for this thread (CUDA only)
#ifdef BUILD_CUDA
    // Only set CUDA device if it's a CUDA device
    // OpenCL devices handle context internally
    if (worker.device && worker.device->getDeviceName().find("CUDA") != std::string::npos) {
        cudaSetDevice(worker.physicalDeviceId);
    }
#endif
    
    try {
        // Initialize the finder
        worker.finder->init();
        
        // Set result callback
        worker.finder->setResultCallback([this, &worker](KeySearchResult result) {
            if (resultCallback_) {
                resultCallback_(result, worker.deviceId);
            }
        });
        
        // Set status callback
        worker.finder->setStatusCallback([this, &worker](KeySearchStatus status) {
            worker.keysProcessed = status.total;
            worker.currentSpeed = status.speed;
            worker.freeMemory = status.freeMemory;
            worker.totalMemory = status.deviceMemory;
            worker.targets = status.targets;
            worker.currentKey = status.nextKey;
        });
        
        // Run the search
        worker.finder->run();
        
    } catch (KeySearchException& ex) {
        Logger::log(LogLevel::Error, 
            "GPU " + util::format(worker.deviceId) + " error: " + ex.msg);
    }
    
    worker.running = false;
}

void MultiGPUManager::aggregateResults() {
    for (auto& worker : workers_) {
        if (!worker.device) continue;
        
        std::vector<KeySearchResult> results;
        size_t count = worker.device->getResults(results);
        
        if (count > 0) {
            std::lock_guard<std::mutex> lock(resultsMutex_);
            for (const auto& result : results) {
                allResults_.push_back(result);
                if (resultCallback_) {
                    resultCallback_(result, worker.deviceId);
                }
            }
        }
    }
}

void MultiGPUManager::updateStatus() {
    auto stats = getStats();
    for (const auto& stat : stats) {
        if (statusCallback_) {
            statusCallback_(stat);
        }
    }
}

void MultiGPUManager::stopAll() {
    shouldStop_ = true;
    
    // Stop all finders
    for (auto& worker : workers_) {
        if (worker.finder) {
            worker.finder->stop();
        }
        worker.running = false;
    }
    
    // Wait for worker threads
    for (auto& worker : workers_) {
        if (worker.workerThread.joinable()) {
            worker.workerThread.join();
        }
    }
    
    // Wait for status thread
    if (statusThread_.joinable()) {
        statusThread_.join();
    }
}

std::vector<bitrecover::GPUStats> MultiGPUManager::getStats() const {
    std::vector<bitrecover::GPUStats> stats;
    stats.reserve(workers_.size());
    
    for (const auto& worker : workers_) {
        bitrecover::GPUStats stat;
        stat.gpuId = worker.deviceId;
        stat.deviceName = worker.deviceName;
        stat.speed = worker.currentSpeed.load();
        stat.keysProcessed = worker.keysProcessed.load();
        stat.freeMemory = worker.freeMemory.load();
        stat.totalMemory = worker.totalMemory.load();
        stat.targets = worker.targets;
        stat.isRunning = worker.running.load();
        stat.currentKey = worker.currentKey.toString(16);
        
        stats.push_back(stat);
    }
    
    return stats;
}

void MultiGPUManager::setResultCallback(ResultCallback callback) {
    resultCallback_ = callback;
}

void MultiGPUManager::setStatusCallback(StatusCallback callback) {
    statusCallback_ = callback;
}

bool MultiGPUManager::isAnyRunning() const {
    for (const auto& worker : workers_) {
        if (worker.running.load()) {
            return true;
        }
    }
    return false;
}

std::string MultiGPUManager::getDeviceTypeName(int deviceType) const {
    if (deviceType == DeviceManager::DeviceType::CUDA) {
        return "CUDA";
    } else if (deviceType == DeviceManager::DeviceType::OpenCL) {
        return "OpenCL";
    }
    return "Unknown";
}

