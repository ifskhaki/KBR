#ifndef STATUS_DISPLAY_H
#define STATUS_DISPLAY_H

#include "bitrecover/types.h"
#include <vector>

class StatusDisplay {
public:
    StatusDisplay();
    ~StatusDisplay();
    
    void update(const std::vector<bitrecover::GPUStats>& stats);
    void showMatch(const bitrecover::MatchResult& match);
    
private:
    void initialize();
    void clearScreen();
    void moveCursor(int row, int col);
    void printHeader();
    void printGPUStats(const bitrecover::GPUStats& stats, int index);
    void printSummary(const std::vector<bitrecover::GPUStats>& stats);
    void cleanup();
    
    int consoleWidth_;
    bool firstUpdate_;
};

#endif // STATUS_DISPLAY_H

