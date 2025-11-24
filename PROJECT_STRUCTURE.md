# Bitrecover Project Structure

```
bitrecover/
├── .github/
│   └── workflows/
│       └── build.yml              # CI/CD pipeline
├── config/
│   └── config.json                # User-editable configuration
├── include/
│   └── bitrecover/
│       ├── types.h                # Type definitions
│       └── constants.h            # Constants
├── notebooks/
│   └── bitrecover_colab.ipynb     # Google Colab notebook
├── scripts/
│   ├── send_email.py              # Email notification script
│   └── startup_notify.py          # Startup notification script
├── src/
│   ├── main.cpp                   # Main entry point
│   ├── BitrecoverEngine.cpp/h     # Core engine
│   ├── MultiGPUManager.cpp/h      # Multi-GPU coordination
│   ├── EmailNotifier.cpp/h        # Email notifications
│   ├── StatusDisplay.cpp/h        # Real-time status display
│   ├── ConfigManager.cpp/h        # Configuration management
│   └── RandomKeyGenerator.cpp/h   # Random key generation
├── .gitignore                     # Git ignore rules
├── CMakeLists.txt                 # CMake build configuration
├── Makefile                       # Convenience Makefile
├── LICENSE                        # MIT License
├── README.md                      # Main documentation
├── QUICKSTART.md                  # Quick start guide
├── CONTRIBUTING.md                # Contribution guidelines
├── PROJECT_STRUCTURE.md           # This file
├── address.txt                    # Target addresses (user-editable)
```

## Key Components

### Core Engine (`src/BitrecoverEngine.*`)
- Main orchestrator for the entire application
- Manages initialization, configuration, and execution flow
- Coordinates between GPU manager, email notifier, and status display

### Multi-GPU Manager (`src/MultiGPUManager.*`)
- Handles parallel execution across multiple GPUs
- Manages worker threads, one per GPU
- Aggregates results and statistics
- Provides real-time status updates

### Email Notifier (`src/EmailNotifier.*`)
- Sends startup notifications
- Sends match notifications when keys are found
- Integrates with Python email scripts

### Status Display (`src/StatusDisplay.*`)
- Real-time terminal-based dashboard
- Shows per-GPU statistics
- Displays matches prominently
- Cross-platform (Windows/Linux)

### Configuration Manager (`src/ConfigManager.*`)
- Loads configuration from `config/config.json`
- Provides defaults
- System information gathering

### Random Key Generator (`src/RandomKeyGenerator.*`)
- Generates random 256-bit keys for pre-2012 wallets
- Ensures unique starting points per GPU
- Validates key ranges

## Build System

### CMake
- Modern CMake 3.18+ configuration
- Auto-detects GPU architectures
- Supports multiple CUDA compute capabilities

### Makefile
- Convenience wrapper around CMake
- Quick build commands
- Common development tasks

## Scripts

### Python Scripts
- `send_email.py`: Handles email sending (startup and matches)
- `startup_notify.py`: Startup notification wrapper

## Configuration

- `config/config.json` (edit directly to personalize)
- `address.txt` (edit directly with your Bitcoin target addresses)

## Dependencies

### Required
- CUDA Toolkit 11.0+
- CMake 3.18+
- C++17 compiler
- Python 3.7+ (for scripts)

### Optional
- nlohmann/json (for full JSON config support)
- Email server access (for notifications)

## Build Output

```
build/
├── bin/
│   └── bitrecover          # Main executable
└── lib/                    # Libraries (if any)
```

## Usage Flow

1. **Initialization**
   - Edit `config/config.json` and `address.txt` as needed
   - Load configuration
   - Detect GPUs
   - Initialize GPU workers
   - Send startup email

2. **Execution**
   - Start parallel GPU threads
   - Update status display
   - Aggregate results
   - Handle matches

3. **Shutdown**
   - Stop all workers
   - Save final statistics
   - Cleanup resources

## Future Enhancements

- [ ] Full JSON config parser (nlohmann/json integration)
- [ ] Web dashboard for remote monitoring
- [ ] Database backend for match storage
- [ ] Distributed computing support
- [ ] Advanced GPU optimization profiles
- [ ] Checkpoint/resume per GPU
- [ ] Performance analytics and reporting

