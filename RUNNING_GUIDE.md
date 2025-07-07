# Enhanced O-RAN Module - Running Guide

## Overview

This repository contains an enhanced ns-3 O-RAN module with advanced features including:

- **Reinforcement Learning** for intelligent handovers
- **Digital Twin** framework for network prediction  
- **Multi-access Edge Computing (MEC)** for low-latency services
- **Cloud-Native** orchestration with Kubernetes simulation
- **Advanced AI/ML** integration and analytics

## Current Status

✅ **Module Enhancement Complete**

- Advanced C++ modules implemented
- Comprehensive examples created  
- Test suite developed
- Documentation updated
- Build system configured

## How to Run the Codebase

### Prerequisites

This is an **ns-3 module** that requires integration with the ns-3 network simulator framework.

#### Required Software

- ns-3 (version 3.35 or later)
- Python 3.6+
- CMake 3.10+
- GCC 7+ or Clang 6+
- SQLite3 development libraries

#### Optional (for ML features)

- PyTorch C++ API (libtorch)
- ONNX Runtime

### Installation Steps

#### 1. Install ns-3

**On Ubuntu/Debian:**

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install g++ python3 python3-dev pkg-config sqlite3 libsqlite3-dev
sudo apt-get install cmake libgtk-3-dev libxml2 libxml2-dev

# Download and build ns-3
git clone https://gitlab.com/nsnam/ns-3-dev.git
cd ns-3-dev
```

**On Windows:**

- Install Visual Studio 2019/2022 with C++ support
- Install Python 3.6+
- Use vcpkg or manual installation for dependencies

#### 2. Integrate O-RAN Module

```bash
# Copy this O-RAN module to ns-3 contrib directory
cp -r /path/to/ns3-oran-master /path/to/ns-3-dev/contrib/oran

# Or create symbolic link
ln -s /path/to/ns3-oran-master /path/to/ns-3-dev/contrib/oran
```

#### 3. Configure and Build

```bash
cd /path/to/ns-3-dev

# Configure with SQLite support (required for O-RAN)
./ns3 configure --enable-examples --enable-tests --enable-sqlite

# Build the entire ns-3 with O-RAN module
./ns3 build
```

#### 4. Verify Installation

```bash
# Run basic O-RAN example
./ns3 run oran-data-repository-example

# Run advanced integration example
./ns3 run oran-advanced-integration-example

# Run AI/ML edge example
./ns3 run oran-advanced-ai-edge-example
```

### Running Examples

#### Basic Examples

```bash
# Data repository functionality
./ns3 run oran-data-repository-example

# Keep-alive mechanism
./ns3 run oran-keep-alive-example

# Distance-based handover
./ns3 run oran-lte-2-lte-distance-handover-example
```

#### Advanced Examples (Enhanced Features)

```bash
# AI/ML edge computing with reinforcement learning
./ns3 run oran-advanced-ai-edge-example

# Complete integration test with all advanced features
./ns3 run oran-advanced-integration-example
```

#### Example with Parameters

```bash
# Run with custom parameters
./ns3 run "oran-advanced-integration-example --numUes=50 --numEnbs=10 --simTime=600"
```

### Running Tests

```bash
# Run O-RAN module tests
./ns3 run oran-test-suite

# Run advanced modules test
./ns3 run oran-advanced-modules-test

# Run all tests
./test.py -s oran
```

### Configuration Options

#### Environment Variables (Optional)

```bash
# For PyTorch support
export LIBTORCHPATH=/path/to/libtorch

# For ONNX Runtime support  
export LIBONNXPATH=/path/to/onnxruntime
```

#### CMake Options

```bash
# Configure with specific options
./ns3 configure --enable-examples --enable-tests --enable-sqlite --enable-python
```

### Output and Results

#### Generated Files

- **Database files**: `*.db` (SQLite databases with simulation data)
- **Log files**: Console output with detailed O-RAN operations
- **Trace files**: Network performance metrics
- **Model files**: Trained ML models (if ML features used)

#### Key Metrics

- Handover success rates
- Network latency and throughput
- ML model performance
- Digital twin prediction accuracy
- MEC service deployment statistics

### Troubleshooting

#### Common Issues

1. **SQLite not found**

   ```bash
   sudo apt-get install libsqlite3-dev
   ./ns3 configure --enable-sqlite
   ```

2. **Python bindings issues**

   ```bash
   ./ns3 configure --enable-python
   ```

3. **PyTorch/ONNX not found**
   - Set environment variables or install libraries
   - Module works without ML libraries (falls back to basic algorithms)

### Advanced Usage

#### Custom Logic Modules

```cpp
// Create custom reinforcement learning configuration
oranHelper.SetLogicModule("ns3::OranLmReinforcementLearning",
                          "LearningRate", DoubleValue(0.001),
                          "DiscountFactor", DoubleValue(0.95));
```

#### Digital Twin Configuration  

```cpp
// Setup predictive analytics
digitalTwin->SetAttribute("PredictionWindow", TimeValue(Seconds(60.0)));
digitalTwin->SetAttribute("EnableWhatIfAnalysis", BooleanValue(true));
```

#### MEC Framework

```cpp
// Deploy edge services
mecFramework->DeployService("VideoAnalytics", requirements);
mecFramework->EnableFederatedLearning(true);
```

## Current Limitations

- **Full Build**: Requires complete ns-3 installation
- **Hardware Dependencies**: Some ML features require specific libraries
- **Platform**: Primarily tested on Linux (adaptable to Windows/macOS)

## Next Steps

1. **Install ns-3** following the official documentation
2. **Copy this module** to the ns-3 contrib directory
3. **Configure and build** with `./ns3 configure --enable-sqlite && ./ns3 build`
4. **Run examples** starting with basic ones, then advanced features
5. **Customize** for your specific research needs

## File Structure Summary

```
Enhanced O-RAN Module:
├── model/                          # Core implementations
│   ├── oran-lm-reinforcement-learning.{h,cc}    # RL logic module
│   ├── oran-digital-twin.{h,cc}                 # Digital twin framework  
│   ├── oran-mec-framework.{h,cc}                # MEC orchestration
│   └── oran-cloud-native.{h,cc}                 # Cloud-native features
├── examples/                       # Runnable examples
│   ├── oran-advanced-ai-edge-example.cc         # AI/ML edge demo
│   └── oran-advanced-integration-example.cc     # Complete integration
├── test/                          # Test suite
│   └── oran-advanced-modules-test.cc            # Advanced features tests
└── docs/                          # Documentation
    └── ADVANCED_FEATURES.md                     # Detailed feature guide
```

This enhanced O-RAN module represents a significant advancement in network simulation capabilities, providing researchers with state-of-the-art tools for investigating intelligent RAN architectures.
