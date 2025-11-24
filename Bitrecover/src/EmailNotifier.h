#ifndef EMAIL_NOTIFIER_H
#define EMAIL_NOTIFIER_H

#include "bitrecover/types.h"
#include <vector>
#include <string>

class EmailNotifier {
public:
    EmailNotifier(const bitrecover::Config::EmailConfig& config);
    
    void sendStartupNotification(const bitrecover::SystemInfo& sysInfo,
                                const std::vector<bitrecover::GPUInfo>& gpus);
    void sendMatchNotification(const bitrecover::MatchResult& match);
    std::string getCurrentTimestamp() const;
    
private:
    void executeScript(const std::string& command);
    
    bitrecover::Config::EmailConfig config_;
};

#endif // EMAIL_NOTIFIER_H

