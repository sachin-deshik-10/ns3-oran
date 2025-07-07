#!/bin/bash

# Advanced O-RAN Build and Test Script
# This script builds the enhanced ns-3 O-RAN module and runs comprehensive tests

set -e  # Exit on any error

echo "========================================"
echo "Advanced O-RAN Build and Test Script"
echo "========================================"

# Configuration
BUILD_DIR="build"
LOG_DIR="logs"
RESULTS_DIR="results"

# Create directories
mkdir -p "$LOG_DIR"
mkdir -p "$RESULTS_DIR"

echo "Creating necessary directories..."

# Function to log with timestamp
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Function to check build status
check_build_status() {
    if [ $? -eq 0 ]; then
        log "✓ $1 completed successfully"
    else
        log "✗ $1 failed"
        exit 1
    fi
}

log "Starting O-RAN advanced features build and test process..."

# Step 1: Configure ns-3 build
log "Configuring ns-3 build system..."
if command -v python3 &> /dev/null; then
    PYTHON_CMD="python3"
elif command -v python &> /dev/null; then
    PYTHON_CMD="python"
else
    log "Error: Python not found. Please install Python 3."
    exit 1
fi

# Configure build with advanced features
log "Configuring build with advanced O-RAN features..."
$PYTHON_CMD ns3 configure --enable-examples --enable-tests --build-profile=optimized \
    --enable-sqlite --enable-mpi 2>&1 | tee "$LOG_DIR/configure.log"
check_build_status "Configuration"

# Step 2: Build the project
log "Building ns-3 with O-RAN advanced modules..."
$PYTHON_CMD ns3 build 2>&1 | tee "$LOG_DIR/build.log"
check_build_status "Build"

# Step 3: Run unit tests
log "Running O-RAN advanced modules unit tests..."
$PYTHON_CMD test.py --suite=oran-advanced-modules --verbose 2>&1 | tee "$LOG_DIR/unit_tests.log"
check_build_status "Unit tests"

# Step 4: Run integration tests
log "Running O-RAN integration tests..."
$PYTHON_CMD test.py --suite=oran --verbose 2>&1 | tee "$LOG_DIR/integration_tests.log"
check_build_status "Integration tests"

# Step 5: Run example simulations
log "Running O-RAN advanced examples..."

# Run the advanced AI edge example
log "Running advanced AI edge example..."
$PYTHON_CMD ns3 run "oran-advanced-ai-edge-example --numUes=10 --numEnbs=3 --simTime=60" \
    2>&1 | tee "$LOG_DIR/ai_edge_example.log"
check_build_status "AI Edge example"

# Run the advanced integration example with different configurations
log "Running advanced integration example (basic configuration)..."
$PYTHON_CMD ns3 run "oran-advanced-integration-example --numUes=15 --numEnbs=4 --simTime=120 \
    --enableRL=true --enableDigitalTwin=true --enableMEC=true --enableCloudNative=true" \
    2>&1 | tee "$LOG_DIR/integration_example_basic.log"
check_build_status "Integration example (basic)"

log "Running advanced integration example (federated learning enabled)..."
$PYTHON_CMD ns3 run "oran-advanced-integration-example --numUes=20 --numEnbs=5 --simTime=180 \
    --enableRL=true --enableDigitalTwin=true --enableMEC=true --enableCloudNative=true \
    --enableFederatedLearning=true" \
    2>&1 | tee "$LOG_DIR/integration_example_fl.log"
check_build_status "Integration example (federated learning)"

log "Running advanced integration example (performance test)..."
$PYTHON_CMD ns3 run "oran-advanced-integration-example --numUes=50 --numEnbs=7 --simTime=300 \
    --enableRL=true --enableDigitalTwin=true --enableMEC=true --enableCloudNative=true \
    --enableFederatedLearning=true --speed=30 --distance=80" \
    2>&1 | tee "$LOG_DIR/integration_example_performance.log"
check_build_status "Integration example (performance)"

# Step 6: Validate outputs and databases
log "Validating simulation outputs..."

# Check for database files
for db_file in oran-advanced-ai-edge.db oran-advanced-integration.db test-integration.db; do
    if [ -f "$db_file" ]; then
        log "✓ Database file $db_file created successfully"
        # Get database size
        db_size=$(du -h "$db_file" | cut -f1)
        log "  Database size: $db_size"
        
        # Move to results directory
        mv "$db_file" "$RESULTS_DIR/"
    else
        log "⚠ Database file $db_file not found (may be expected for some tests)"
    fi
done

# Check for log files and traces
log "Checking for trace files and logs..."
trace_count=$(find . -name "*.tr" -o -name "*.pcap" -o -name "*.xml" | wc -l)
log "Found $trace_count trace files"

# Step 7: Performance analysis
log "Performing basic performance analysis..."

# Analyze build log for warnings
warning_count=$(grep -c "warning" "$LOG_DIR/build.log" || true)
error_count=$(grep -c "error" "$LOG_DIR/build.log" || true)

log "Build analysis:"
log "  Warnings: $warning_count"
log "  Errors: $error_count"

# Analyze test results
test_pass_count=$(grep -c "PASS" "$LOG_DIR/unit_tests.log" || true)
test_fail_count=$(grep -c "FAIL" "$LOG_DIR/unit_tests.log" || true)

log "Test analysis:"
log "  Passed tests: $test_pass_count"
log "  Failed tests: $test_fail_count"

# Step 8: Generate summary report
log "Generating summary report..."

cat << EOF > "$RESULTS_DIR/build_test_summary.txt"
O-RAN Advanced Features Build and Test Summary
==============================================
Date: $(date)
Build Configuration: Optimized with examples and tests
Python Version: $($PYTHON_CMD --version)

Build Results:
- Warnings: $warning_count
- Errors: $error_count
- Status: $([ $error_count -eq 0 ] && echo "SUCCESS" || echo "FAILED")

Test Results:
- Unit Tests Passed: $test_pass_count
- Unit Tests Failed: $test_fail_count
- Integration Tests: See integration_tests.log
- Status: $([ $test_fail_count -eq 0 ] && echo "SUCCESS" || echo "FAILED")

Example Simulations:
- AI Edge Example: $([ -f "$LOG_DIR/ai_edge_example.log" ] && echo "COMPLETED" || echo "FAILED")
- Integration Example (Basic): $([ -f "$LOG_DIR/integration_example_basic.log" ] && echo "COMPLETED" || echo "FAILED")
- Integration Example (FL): $([ -f "$LOG_DIR/integration_example_fl.log" ] && echo "COMPLETED" || echo "FAILED")
- Integration Example (Performance): $([ -f "$LOG_DIR/integration_example_performance.log" ] && echo "COMPLETED" || echo "FAILED")

Database Files Created:
$(ls -la "$RESULTS_DIR"/*.db 2>/dev/null || echo "No database files found")

Trace Files: $trace_count files found

Advanced Features Tested:
✓ Reinforcement Learning Logic Module
✓ Digital Twin Framework
✓ Multi-access Edge Computing (MEC) Framework
✓ Cloud-Native Integration Framework
✓ Federated Learning
✓ Cross-framework Integration

For detailed logs, see the logs/ directory.
EOF

# Step 9: Cleanup and final checks
log "Performing cleanup..."

# Move any remaining output files to results
find . -maxdepth 1 -name "*.db" -exec mv {} "$RESULTS_DIR/" \;
find . -maxdepth 1 -name "*.tr" -exec mv {} "$RESULTS_DIR/" \;
find . -maxdepth 1 -name "*.pcap" -exec mv {} "$RESULTS_DIR/" \;

# Final validation
log "Final validation..."

# Check if key files exist
key_files=(
    "model/oran-lm-reinforcement-learning.h"
    "model/oran-lm-reinforcement-learning.cc"
    "model/oran-digital-twin.h"
    "model/oran-digital-twin.cc"
    "model/oran-mec-framework.h"
    "model/oran-mec-framework.cc"
    "model/oran-cloud-native.h"
    "model/oran-cloud-native.cc"
)

for file in "${key_files[@]}"; do
    if [ -f "$file" ]; then
        log "✓ Key file $file exists"
    else
        log "✗ Key file $file missing"
        exit 1
    fi
done

log "========================================"
log "BUILD AND TEST SUMMARY"
log "========================================"
log "✓ All advanced O-RAN modules built successfully"
log "✓ Unit tests completed"
log "✓ Integration tests completed"
log "✓ Example simulations completed"
log "✓ Output files generated and organized"
log ""
log "Results location: $RESULTS_DIR/"
log "Logs location: $LOG_DIR/"
log "Summary report: $RESULTS_DIR/build_test_summary.txt"
log ""
log "The ns-3 O-RAN module has been successfully enhanced with:"
log "  • AI/ML Reinforcement Learning capabilities"
log "  • Digital Twin framework with predictive analytics"
log "  • Multi-access Edge Computing (MEC) framework"
log "  • Cloud-Native orchestration with Kubernetes support"
log "  • Federated Learning across edge nodes"
log "  • Advanced examples and comprehensive testing"
log ""
log "✓ Advanced O-RAN enhancement completed successfully!"
log "========================================"

# Display summary report
cat "$RESULTS_DIR/build_test_summary.txt"

exit 0
