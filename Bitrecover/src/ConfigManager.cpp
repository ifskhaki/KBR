#include "ConfigManager.h"
#include "bitrecover/types.h"
#include "bitrecover/constants.h"
#include "Logger.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

ConfigManager::ConfigManager() {
    loadDefaults();
}

void ConfigManager::loadDefaults() {
    // Email defaults
    config_.email.enabled = false;
    config_.email.smtpServer = DEFAULT_SMTP_SERVER;
    config_.email.smtpPort = DEFAULT_SMTP_PORT;
    config_.email.username = "";
    config_.email.password = "";
    
    // GPU defaults
    config_.gpu.useAllGPUs = true;
    config_.gpu.autoOptimize = true;
    config_.gpu.threadsPerBlock = DEFAULT_THREADS_PER_BLOCK;
    config_.gpu.blocks = 0; // Auto-detect
    config_.gpu.pointsPerThread = DEFAULT_POINTS_PER_THREAD;
    
    // Search defaults
    config_.search.targetsFile = DEFAULT_TARGETS_FILE;
    config_.search.outputFile = DEFAULT_OUTPUT_FILE;
    config_.search.compression = "UNCOMPRESSED";
    config_.search.random256 = true;
    config_.search.statusIntervalMs = DEFAULT_STATUS_INTERVAL_MS;
    config_.search.checkpointFile = "";
    config_.search.checkpointIntervalMs = DEFAULT_CHECKPOINT_INTERVAL_MS;
    
    // Display defaults
    config_.display.realTime = true;
    config_.display.updateIntervalMs = DEFAULT_DISPLAY_UPDATE_MS;
    config_.display.showGPUDetails = true;
    config_.display.clearScreen = true;
}

bool ConfigManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::log(LogLevel::Warning, "Config file not found: " + filename + ", using defaults");
        return false;
    }
    
    std::string line;
    std::string section = "";
    
    while (std::getline(file, line)) {
        util::trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Simple JSON-like parser (for basic config)
        // In production, use a proper JSON library like nlohmann/json
        
        // For now, we'll use a simple key=value format
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);
            util::trim(key);
            util::trim(value);
            
            parseConfigKey(key, value);
        }
    }
    
    Logger::log(LogLevel::Info, "Config loaded from: " + filename);
    return true;
}

void ConfigManager::parseConfigKey(const std::string& key, const std::string& value) {
    // Simple key-value parser
    // For full JSON support, integrate nlohmann/json library
    
    if (key == "email.enabled") {
        config_.email.enabled = (value == "true" || value == "1");
    } else if (key == "email.smtp_server") {
        config_.email.smtpServer = value;
    } else if (key == "email.smtp_port") {
        config_.email.smtpPort = std::stoi(value);
    } else if (key == "email.username") {
        config_.email.username = value;
    } else if (key == "email.password") {
        config_.email.password = value;
    } else if (key == "gpu.use_all_gpus") {
        config_.gpu.useAllGPUs = (value == "true" || value == "1");
    } else if (key == "gpu.threads_per_block") {
        config_.gpu.threadsPerBlock = std::stoi(value);
    } else if (key == "gpu.points_per_thread") {
        config_.gpu.pointsPerThread = std::stoi(value);
    } else if (key == "search.targets_file") {
        config_.search.targetsFile = value;
    } else if (key == "search.output_file") {
        config_.search.outputFile = value;
    } else if (key == "search.compression") {
        config_.search.compression = value;
    } else if (key == "search.random256") {
        config_.search.random256 = (value == "true" || value == "1");
    }
}

bitrecover::Config ConfigManager::getConfig() const {
    return config_;
}

void ConfigManager::setConfig(const bitrecover::Config& config) {
    config_ = config;
}

std::string ConfigManager::getHostname() const {
#ifdef _WIN32
    char hostname[256];
    DWORD size = sizeof(hostname);
    if (GetComputerNameA(hostname, &size)) {
        return std::string(hostname);
    }
    return "unknown";
#else
    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
    return "unknown";
#endif
}

