#ifndef BITRECOVER_CONSTANTS_H
#define BITRECOVER_CONSTANTS_H

namespace bitrecover {

// Default values
constexpr int DEFAULT_THREADS_PER_BLOCK = 256;
constexpr int DEFAULT_POINTS_PER_THREAD = 32;
constexpr uint64_t DEFAULT_STATUS_INTERVAL_MS = 1000;
constexpr uint64_t DEFAULT_DISPLAY_UPDATE_MS = 100;
constexpr uint64_t DEFAULT_CHECKPOINT_INTERVAL_MS = 60000;

// File paths
constexpr const char* DEFAULT_TARGETS_FILE = "address.txt";
constexpr const char* DEFAULT_OUTPUT_FILE = "Success.txt";
constexpr const char* DEFAULT_CONFIG_FILE = "config/config.json";

// Email
constexpr const char* DEFAULT_SMTP_SERVER = "smtp.gmail.com";
constexpr int DEFAULT_SMTP_PORT = 587;

// Display
constexpr int STATUS_LINE_WIDTH = 80;
constexpr int MAX_GPU_NAME_LENGTH = 32;

} // namespace bitrecover

#endif // BITRECOVER_CONSTANTS_H

