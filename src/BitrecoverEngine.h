#ifndef BITRECOVER_ENGINE_H
#define BITRECOVER_ENGINE_H

#include "bitrecover/types.h"
#include "MultiGPUManager.h"
#include "EmailNotifier.h"
#include "StatusDisplay.h"
#include "ConfigManager.h"
#include "KeySearchTypes.h"
#include <string>
#include <memory>

class BitrecoverEngine {
public:
    BitrecoverEngine();
    ~BitrecoverEngine();
    
    // Initialize engine with configuration
    bool initialize(const std::string& configFile = "");
    
    // Run the search
    int run();
    
    // Stop the search
    void stop();
    
private:
    bool loadConfiguration(const std::string& configFile);
    bool gatherSystemInfo();
    void setupCallbacks();
    void sendStartupNotification();
    
    std::unique_ptr<MultiGPUManager> gpuManager_;
    std::unique_ptr<EmailNotifier> emailNotifier_;
    std::unique_ptr<StatusDisplay> statusDisplay_;
    std::unique_ptr<ConfigManager> configManager_;
    
    bitrecover::Config config_;
    bitrecover::SystemInfo systemInfo_;
    
    bool initialized_;
};

#endif // BITRECOVER_ENGINE_H

