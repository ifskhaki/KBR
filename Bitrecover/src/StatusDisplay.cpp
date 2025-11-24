#include "StatusDisplay.h"
#include "bitrecover/types.h"
#include "util.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

StatusDisplay::StatusDisplay() : firstUpdate_(true) {
    initialize();
}

StatusDisplay::~StatusDisplay() {
    cleanup();
}

void StatusDisplay::initialize() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        consoleWidth_ = csbi.dwSize.X;
    } else {
        consoleWidth_ = 80;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        consoleWidth_ = w.ws_col;
    } else {
        consoleWidth_ = 80;
    }
#endif
}

void StatusDisplay::update(const std::vector<bitrecover::GPUStats>& stats) {
    if (firstUpdate_) {
        clearScreen();
        printHeader();
        firstUpdate_ = false;
    }
    
    // Move cursor to start of status area
    moveCursor(3, 0);
    
    // Print each GPU's stats
    for (size_t i = 0; i < stats.size(); ++i) {
        printGPUStats(stats[i], static_cast<int>(i));
        // Add separator between GPUs (except last)
        if (i < stats.size() - 1) {
            std::cout << "╠══════════════════════════════════════════════════════════════════╣\n";
        }
    }
    
    // Print summary
    printSummary(stats);
    
    // Flush output
    std::cout.flush();
}

void StatusDisplay::showMatch(const bitrecover::MatchResult& match) {
    // Move to bottom of screen
    moveCursor(10 + static_cast<int>(match.gpuId), 0);
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    🎉 MATCH FOUND! 🎉                             ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Address:    " << std::left << std::setw(58) << match.address << "║\n";
    std::cout << "║ Private Key: " << std::left << std::setw(57) << match.privateKeyHex << "║\n";
    std::cout << "║ WIF:         " << std::left << std::setw(57) << match.wif << "║\n";
    std::cout << "║ GPU:         " << std::left << std::setw(57) << std::to_string(match.gpuId) << "║\n";
    std::cout << "║ Time:        " << std::left << std::setw(57) << match.timestamp << "║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    std::cout.flush();
}

void StatusDisplay::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

void StatusDisplay::moveCursor(int row, int col) {
#ifdef _WIN32
    COORD coord;
    coord.X = col;
    coord.Y = row;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    std::cout << "\033[" << row << ";" << col << "H";
#endif
}

void StatusDisplay::printHeader() {
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    BITRECOVER - Multi-GPU Scanner                ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════╣\n";
    std::cout.flush();
}

void StatusDisplay::printGPUStats(const bitrecover::GPUStats& stats, int index) {
    std::ostringstream oss;
    
    oss << "║ GPU " << index << ": " << std::left << std::setw(50);
    std::string name = stats.deviceName;
    if (name.length() > 45) {
        name = name.substr(0, 42) + "...";
    }
    oss << name << "║\n";
    
    oss << "║   Speed: " << std::right << std::setw(10) << std::fixed 
        << std::setprecision(2) << stats.speed << " MKeys/s";
    oss << "  |  Processed: " << std::setw(15) 
        << util::formatThousands(stats.keysProcessed) << " keys     ║\n";
    
    oss << "║   Memory: " << std::setw(6) << (stats.freeMemory / (1024 * 1024))
        << " / " << std::setw(6) << (stats.totalMemory / (1024 * 1024)) << " MB";
    oss << "  |  Status: " << std::setw(10) 
        << (stats.isRunning ? "Running" : "Stopped") << "            ║\n";
    
    std::cout << oss.str();
}

void StatusDisplay::printSummary(const std::vector<bitrecover::GPUStats>& stats) {
    uint64_t totalKeys = 0;
    double totalSpeed = 0.0;
    uint64_t totalTargets = 0;
    
    for (const auto& s : stats) {
        totalKeys += s.keysProcessed;
        totalSpeed += s.speed;
        if (s.targets > 0) {
            totalTargets = s.targets; // All should have same target count
        }
    }
    
    std::cout << "╠══════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ TOTAL: " << std::right << std::setw(10) << std::fixed 
              << std::setprecision(2) << totalSpeed << " MKeys/s";
    std::cout << "  |  " << std::setw(15) << util::formatThousands(totalKeys) 
              << " keys  |  Targets: " << std::setw(6) << totalTargets << "     ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
}

void StatusDisplay::cleanup() {
    // Restore cursor position
    std::cout << "\n";
}

