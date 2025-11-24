# 🚀 Bitrecover Quick Start Guide

Get up and running with Bitrecover in minutes!

## Prerequisites

- NVIDIA GPU with CUDA support (Compute Capability 7.0+)
- CUDA Toolkit 11.0 or later
- CMake 3.18+
- C++17 compatible compiler
- Python 3.7+ (for email scripts)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/bitrecover.git
cd bitrecover
```

### 2. Preparation

- **Edit `config/config.json`** with your email/GPU/search settings.
- **Edit `address.txt`** with your Bitcoin addresses (one per line).

### 3. Build

```bash
# Using Makefile (recommended)
make build

# Or using CMake directly
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 4. Verify Installation

```bash
./build/bin/bitrecover --list-devices
```

You should see your GPU(s) listed.

## Configuration

### 1. Required Files

- `config/config.json` (edit directly)
- `address.txt` (edit directly; one Bitcoin address per line)

### 2. Configure Email (Optional)

Edit `config/config.json`:

```json
{
    "email": {
        "enabled": true,
        "smtp_server": "smtp.gmail.com",
        "smtp_port": 587,
        "username": "your_email@gmail.com",
        "password": "your_app_password",
        "recipients": ["recipient@gmail.com"]
    }
}
```

**Note**: For Gmail, you need to generate an [App Password](https://support.google.com/accounts/answer/185833).

## Running

### Basic Usage

```bash
./build/bin/bitrecover
```

This will:
- Use all available GPUs
- Read addresses from `address.txt`
- Save matches to `Success.txt`
- Send email notifications (if configured)

### Advanced Options

```bash
# Use specific GPUs
./build/bin/bitrecover --gpu 0 --gpu 1

# Custom configuration file
./build/bin/bitrecover --config config/config.json

# Custom targets file
./build/bin/bitrecover --targets address.txt

# Custom output file
./build/bin/bitrecover --output results.txt
```

## Monitoring

The real-time status display shows:
- Per-GPU speed (MKeys/s)
- Total keys processed
- GPU memory usage
- Number of targets
- Running status

## Results

Matches are saved to `Success.txt` in the format:
```
address private_key_hex wif [GPU:0]
```

Email notifications are sent immediately when matches are found.

## Google Colab

See `notebooks/bitrecover_colab.ipynb` for cloud execution instructions.

## Troubleshooting

### No GPUs Detected

- Verify CUDA is installed: `nvcc --version`
- Check GPU is visible: `nvidia-smi`
- Ensure CUDA version matches your GPU driver

### Build Errors

- Ensure CMake version is 3.18+
- Check CUDA toolkit is properly installed
- Verify compiler supports C++17

### Email Not Working

- Check SMTP credentials in config
- For Gmail, use App Password (not regular password)
- Verify network connectivity

## Performance Tips

1. **Use all GPUs**: Enable `use_all_gpus: true` in config
2. **Auto-optimize**: Enable `auto_optimize: true` for best performance
3. **Adjust threads**: For older GPUs, reduce `threads_per_block` to 128
4. **Monitor temperature**: Ensure GPUs don't overheat during long runs

## Next Steps

- Read the full [README.md](README.md) for detailed documentation
- Check [CONTRIBUTING.md](CONTRIBUTING.md) if you want to contribute
- Join discussions in GitHub Issues

Happy searching! 🔍

