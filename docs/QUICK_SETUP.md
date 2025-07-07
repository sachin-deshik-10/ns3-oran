# Quick Setup Guide

This guide provides step-by-step instructions for quickly setting up the O-RAN Module for ns-3.

## Prerequisites Checklist

Before starting, ensure you have:

- [ ] Linux, macOS, or Windows with WSL2
- [ ] At least 4GB RAM and 2GB free disk space
- [ ] Internet connection for downloading dependencies
- [ ] Administrative/sudo privileges for package installation

## One-Line Installation

For Ubuntu/Debian systems, use this quick installation script:

```bash
curl -fsSL https://raw.githubusercontent.com/your-org/ns3-oran-master/main/scripts/quick-install.sh | bash
```

## Manual Installation

### Step 1: Install System Dependencies

#### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git sqlite3 libsqlite3-dev \
                        python3 python3-pip mercurial wget tar
```

#### CentOS/RHEL/Fedora

```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install -y cmake git sqlite-devel python3 python3-pip mercurial wget tar
```

#### macOS

```bash
# Install Xcode command line tools
xcode-select --install

# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git sqlite python3 mercurial wget
```

### Step 2: Download ns-3

```bash
# Option 1: Latest stable release
wget https://www.nsnam.org/releases/ns-allinone-3.36.tar.bz2
tar xjf ns-allinone-3.36.tar.bz2
cd ns-allinone-3.36/ns-3.36

# Option 2: Development version (requires Mercurial)
hg clone http://code.nsnam.org/ns-3-allinone
cd ns-3-allinone
./download.py
cd ns-3.36
```

### Step 3: Install O-RAN Module

```bash
# Clone the O-RAN module
git clone https://github.com/sachin-deshik-10/ns3-oran-master.git
cp -r ns3-oran-master contrib/oran

# Configure ns-3 with O-RAN module
./waf configure --enable-tests --enable-sqlite

# Build everything
./waf build
```

### Step 4: Verify Installation

```bash
# Run a basic example
./waf --run "oran-data-repository-example"

# Run the test suite
./waf --run "test-runner --suite=oran"
```

## Docker Installation

For a containerized environment:

```bash
# Pull the pre-built image
docker pull nist/ns3-oran:latest

# Or build from source
git clone https://github.com/your-org/ns3-oran-master.git
cd ns3-oran-master
docker build -t ns3-oran .

# Run a container
docker run -it --rm ns3-oran
```

## Development Setup

For development work:

```bash
# Install additional development tools
sudo apt-get install -y clang-format clang-tidy doxygen graphviz

# Configure with debug symbols
./waf configure --enable-tests --enable-sqlite --build-profile=debug

# Install Python dependencies for ML examples
pip3 install torch onnx numpy
```

## Machine Learning Setup

To use ML-based logic modules:

### PyTorch Installation

```bash
# CPU version
pip3 install torch torchvision

# GPU version (if CUDA available)
pip3 install torch torchvision --index-url https://download.pytorch.org/whl/cu118
```

### ONNX Runtime Installation

```bash
# CPU version
pip3 install onnxruntime

# GPU version (if CUDA available)
pip3 install onnxruntime-gpu
```

### LibTorch (for C++ integration)

```bash
# Download LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.13.1%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.13.1+cpu.zip

# Configure ns-3 with LibTorch
./waf configure --enable-tests --enable-sqlite \
               --with-libtorch=/path/to/libtorch
```

## Quick Test Run

After installation, verify everything works:

```bash
# Test 1: Basic functionality
./waf --run "oran-data-repository-example"

# Test 2: LTE handover simulation
./waf --run "oran-lte-2-lte-distance-handover-example --simTime=30"

# Test 3: Machine learning handover (if ML dependencies installed)
./waf --run "oran-lte-2-lte-ml-handover-example --modelType=torch"

# Test 4: Multiple network devices
./waf --run "oran-multiple-net-devices-example --numUes=5"
```

## Troubleshooting

### Common Issues

#### Missing SQLite Development Headers

```bash
# Error: sqlite3.h not found
sudo apt-get install libsqlite3-dev  # Ubuntu/Debian
sudo dnf install sqlite-devel        # CentOS/RHEL/Fedora
```

#### Python Module Import Errors

```bash
# Error: ModuleNotFoundError: No module named 'torch'
pip3 install torch onnx numpy

# If using virtual environment
python3 -m venv venv
source venv/bin/activate
pip install torch onnx numpy
```

#### Build Errors

```bash
# Clean and rebuild
./waf clean
./waf configure --enable-tests --enable-sqlite
./waf build

# Check configuration
./waf configure --check-cxx-compiler
```

#### Permission Issues

```bash
# Fix file permissions
chmod +x ./waf
sudo chown -R $USER:$USER .
```

## Next Steps

After successful installation:

1. **Read the User Guide**: `docs/USER_GUIDE.md`
2. **Explore Examples**: Check the `examples/` directory
3. **Review API Documentation**: `docs/API_REFERENCE.md`
4. **Join the Community**: See contributing guidelines in `CONTRIBUTING.md`

## Alternative Installation Methods

### Package Managers

#### Conda

```bash
conda install -c conda-forge ns3-oran
```

#### Snap

```bash
sudo snap install ns3-oran
```

### IDE Integration

#### VS Code

```bash
# Install C++ extension
code --install-extension ms-vscode.cpptools

# Open workspace
code /path/to/ns-3.36
```

#### CLion

```bash
# Import CMake project
# File -> Open -> Select ns-3.36 directory
```

## Performance Optimization

For large-scale simulations:

```bash
# Optimized build
./waf configure --enable-tests --enable-sqlite --build-profile=optimized

# Parallel build
./waf build -j$(nproc)

# Memory optimization
export NS_LOG=""  # Disable logging for performance
```

## Platform-Specific Notes

### Windows (WSL2)

- Use Ubuntu 20.04 or later in WSL2
- Install Windows Terminal for better experience
- Mount points: `/mnt/c/` for C: drive access

### macOS

- May need to install Xcode from App Store
- Use Homebrew for dependency management
- Consider using macOS-specific ns-3 builds

### Docker

- Recommended for consistent environments
- Good for CI/CD pipelines
- Isolated from host system dependencies

This completes the quick setup guide. For detailed configuration options and advanced usage, refer to the comprehensive documentation.
