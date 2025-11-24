#ifndef BITRECOVER_TYPES_H
#define BITRECOVER_TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>

// Forward declarations
class KeySearchDevice;
class KeyFinder;

namespace bitrecover {

struct GPUInfo {
    int id;
    std::string name;
    uint64_t memory;
    int computeUnits;
    int computeCapabilityMajor;
    int computeCapabilityMinor;
};

struct GPUStats {
    int gpuId;
    std::string deviceName;
    double speed;              // MKeys/s
    uint64_t keysProcessed;
    uint64_t freeMemory;
    uint64_t totalMemory;
    uint64_t targets;
    bool isRunning;
    std::string currentKey;
};

struct SystemInfo {
    std::string hostname;
    std::string os;
    std::vector<GPUInfo> gpus;
    uint64_t totalGPUMemory;
    int gpuCount;
};

struct MatchResult {
    std::string address;
    std::string privateKeyHex;
    std::string wif;
    int gpuId;
    std::string timestamp;
};

struct Config {
    struct EmailConfig {
        bool enabled;
        std::string smtpServer;
        int smtpPort;
        std::string username;
        std::string password;
        std::vector<std::string> recipients;
    } email;

    struct GPUConfig {
        bool useAllGPUs;
        std::vector<int> gpuIds;
        bool autoOptimize;
        int threadsPerBlock;
        int blocks;
        int pointsPerThread;
    } gpu;

    struct SearchConfig {
        std::string targetsFile;
        std::string outputFile;
        std::string compression;
        bool random256;
        uint64_t statusIntervalMs;
        std::string checkpointFile;
        uint64_t checkpointIntervalMs;
    } search;

    struct DisplayConfig {
        bool realTime;
        uint64_t updateIntervalMs;
        bool showGPUDetails;
        bool clearScreen;
    } display;
};

} // namespace bitrecover

#endif // BITRECOVER_TYPES_H

