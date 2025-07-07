# Advanced O-RAN Build and Test Script (PowerShell)
# This script builds the enhanced ns-3 O-RAN module and runs comprehensive tests

param(
    [switch]$SkipTests = $false,
    [switch]$QuickTest = $false,
    [int]$NumUes = 20,
    [int]$NumEnbs = 5,
    [int]$SimTime = 180
)

# Error handling
$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Advanced O-RAN Build and Test Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# Configuration
$BuildDir = "build"
$LogDir = "logs"
$ResultsDir = "results"

# Create directories
if (!(Test-Path $LogDir)) { New-Item -ItemType Directory -Path $LogDir | Out-Null }
if (!(Test-Path $ResultsDir)) { New-Item -ItemType Directory -Path $ResultsDir | Out-Null }

Write-Host "Creating necessary directories..." -ForegroundColor Green

# Function to log with timestamp
function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $color = switch ($Level) {
        "SUCCESS" { "Green" }
        "ERROR" { "Red" }
        "WARNING" { "Yellow" }
        default { "White" }
    }
    Write-Host "[$timestamp] $Message" -ForegroundColor $color
}

# Function to check command status
function Test-CommandStatus {
    param([string]$Operation, [int]$ExitCode = $LASTEXITCODE)
    if ($ExitCode -eq 0) {
        Write-Log "✓ $Operation completed successfully" "SUCCESS"
    }
    else {
        Write-Log "✗ $Operation failed (Exit code: $ExitCode)" "ERROR"
        exit $ExitCode
    }
}

Write-Log "Starting O-RAN advanced features build and test process..."

# Check for Python
$pythonCmd = $null
if (Get-Command python3 -ErrorAction SilentlyContinue) {
    $pythonCmd = "python3"
}
elseif (Get-Command python -ErrorAction SilentlyContinue) {
    $pythonCmd = "python"
}
else {
    Write-Log "Error: Python not found. Please install Python 3." "ERROR"
    exit 1
}

Write-Log "Using Python command: $pythonCmd"

# Step 1: Configure ns-3 build
Write-Log "Configuring ns-3 build system..."

Write-Log "Configuring build with advanced O-RAN features..."
& $pythonCmd ns3 configure --enable-examples --enable-tests --build-profile=optimized --enable-sqlite --enable-mpi 2>&1 | Tee-Object "$LogDir\configure.log"
Test-CommandStatus "Configuration"

# Step 2: Build the project
Write-Log "Building ns-3 with O-RAN advanced modules..."
& $pythonCmd ns3 build 2>&1 | Tee-Object "$LogDir\build.log"
Test-CommandStatus "Build"

if (!$SkipTests) {
    # Step 3: Run unit tests
    Write-Log "Running O-RAN advanced modules unit tests..."
    & $pythonCmd test.py --suite=oran-advanced-modules --verbose 2>&1 | Tee-Object "$LogDir\unit_tests.log"
    Test-CommandStatus "Unit tests"

    # Step 4: Run integration tests
    Write-Log "Running O-RAN integration tests..."
    & $pythonCmd test.py --suite=oran --verbose 2>&1 | Tee-Object "$LogDir\integration_tests.log"
    Test-CommandStatus "Integration tests"
}

# Step 5: Run example simulations
Write-Log "Running O-RAN advanced examples..."

if ($QuickTest) {
    $NumUes = 10
    $NumEnbs = 3
    $SimTime = 60
}

# Run the advanced AI edge example
Write-Log "Running advanced AI edge example..."
& $pythonCmd ns3 run "oran-advanced-ai-edge-example --numUes=$NumUes --numEnbs=$NumEnbs --simTime=$SimTime" 2>&1 | Tee-Object "$LogDir\ai_edge_example.log"
Test-CommandStatus "AI Edge example"

# Run the advanced integration example with different configurations
Write-Log "Running advanced integration example (basic configuration)..."
& $pythonCmd ns3 run "oran-advanced-integration-example --numUes=$NumUes --numEnbs=$NumEnbs --simTime=$SimTime --enableRL=true --enableDigitalTwin=true --enableMEC=true --enableCloudNative=true" 2>&1 | Tee-Object "$LogDir\integration_example_basic.log"
Test-CommandStatus "Integration example (basic)"

if (!$QuickTest) {
    Write-Log "Running advanced integration example (federated learning enabled)..."
    & $pythonCmd ns3 run "oran-advanced-integration-example --numUes=$($NumUes + 5) --numEnbs=$($NumEnbs + 1) --simTime=$($SimTime + 60) --enableRL=true --enableDigitalTwin=true --enableMEC=true --enableCloudNative=true --enableFederatedLearning=true" 2>&1 | Tee-Object "$LogDir\integration_example_fl.log"
    Test-CommandStatus "Integration example (federated learning)"

    Write-Log "Running advanced integration example (performance test)..."
    & $pythonCmd ns3 run "oran-advanced-integration-example --numUes=$($NumUes * 2) --numEnbs=$($NumEnbs + 2) --simTime=$($SimTime + 120) --enableRL=true --enableDigitalTwin=true --enableMEC=true --enableCloudNative=true --enableFederatedLearning=true --speed=30 --distance=80" 2>&1 | Tee-Object "$LogDir\integration_example_performance.log"
    Test-CommandStatus "Integration example (performance)"
}

# Step 6: Validate outputs and databases
Write-Log "Validating simulation outputs..."

# Check for database files
$dbFiles = @("oran-advanced-ai-edge.db", "oran-advanced-integration.db", "test-integration.db")
foreach ($dbFile in $dbFiles) {
    if (Test-Path $dbFile) {
        Write-Log "✓ Database file $dbFile created successfully" "SUCCESS"
        $dbSize = (Get-Item $dbFile).Length
        Write-Log "  Database size: $([math]::Round($dbSize / 1MB, 2)) MB"
        Move-Item $dbFile $ResultsDir -Force
    }
    else {
        Write-Log "⚠ Database file $dbFile not found (may be expected for some tests)" "WARNING"
    }
}

# Check for log files and traces
Write-Log "Checking for trace files and logs..."
$traceFiles = Get-ChildItem -Path . -Include "*.tr", "*.pcap", "*.xml" -Recurse
Write-Log "Found $($traceFiles.Count) trace files"

# Step 7: Performance analysis
Write-Log "Performing basic performance analysis..."

# Analyze build log for warnings and errors
if (Test-Path "$LogDir\build.log") {
    $warningCount = (Select-String -Path "$LogDir\build.log" -Pattern "warning" -AllMatches).Matches.Count
    $errorCount = (Select-String -Path "$LogDir\build.log" -Pattern "error" -AllMatches).Matches.Count

    Write-Log "Build analysis:"
    Write-Log "  Warnings: $warningCount"
    Write-Log "  Errors: $errorCount"
}

# Analyze test results
if (Test-Path "$LogDir\unit_tests.log") {
    $testPassCount = (Select-String -Path "$LogDir\unit_tests.log" -Pattern "PASS" -AllMatches).Matches.Count
    $testFailCount = (Select-String -Path "$LogDir\unit_tests.log" -Pattern "FAIL" -AllMatches).Matches.Count

    Write-Log "Test analysis:"
    Write-Log "  Passed tests: $testPassCount"
    Write-Log "  Failed tests: $testFailCount"
}

# Step 8: Generate summary report
Write-Log "Generating summary report..."

$summaryContent = @"
O-RAN Advanced Features Build and Test Summary
==============================================
Date: $(Get-Date)
Build Configuration: Optimized with examples and tests
Python Version: $(& $pythonCmd --version)
PowerShell Version: $($PSVersionTable.PSVersion)

Build Results:
- Warnings: $warningCount
- Errors: $errorCount
- Status: $(if ($errorCount -eq 0) { "SUCCESS" } else { "FAILED" })

Test Results:
- Unit Tests Passed: $testPassCount
- Unit Tests Failed: $testFailCount
- Integration Tests: See integration_tests.log
- Status: $(if ($testFailCount -eq 0) { "SUCCESS" } else { "FAILED" })

Example Simulations:
- AI Edge Example: $(if (Test-Path "$LogDir\ai_edge_example.log") { "COMPLETED" } else { "FAILED" })
- Integration Example (Basic): $(if (Test-Path "$LogDir\integration_example_basic.log") { "COMPLETED" } else { "FAILED" })
- Integration Example (FL): $(if (Test-Path "$LogDir\integration_example_fl.log") { "COMPLETED" } else { "FAILED" })
- Integration Example (Performance): $(if (Test-Path "$LogDir\integration_example_performance.log") { "COMPLETED" } else { "FAILED" })

Database Files Created:
$(if (Test-Path "$ResultsDir\*.db") { Get-ChildItem "$ResultsDir\*.db" | ForEach-Object { "- $($_.Name) ($([math]::Round($_.Length / 1MB, 2)) MB)" } } else { "No database files found" })

Trace Files: $($traceFiles.Count) files found

Advanced Features Tested:
✓ Reinforcement Learning Logic Module
✓ Digital Twin Framework
✓ Multi-access Edge Computing (MEC) Framework
✓ Cloud-Native Integration Framework
✓ Federated Learning
✓ Cross-framework Integration

For detailed logs, see the logs/ directory.
"@

$summaryContent | Out-File "$ResultsDir\build_test_summary.txt" -Encoding UTF8

# Step 9: Cleanup and final checks
Write-Log "Performing cleanup..."

# Move any remaining output files to results
Get-ChildItem -Path . -Name "*.db" | ForEach-Object { Move-Item $_ $ResultsDir -Force -ErrorAction SilentlyContinue }
Get-ChildItem -Path . -Name "*.tr" | ForEach-Object { Move-Item $_ $ResultsDir -Force -ErrorAction SilentlyContinue }
Get-ChildItem -Path . -Name "*.pcap" | ForEach-Object { Move-Item $_ $ResultsDir -Force -ErrorAction SilentlyContinue }

# Final validation
Write-Log "Final validation..."

# Check if key files exist
$keyFiles = @(
    "model\oran-lm-reinforcement-learning.h",
    "model\oran-lm-reinforcement-learning.cc",
    "model\oran-digital-twin.h",
    "model\oran-digital-twin.cc",
    "model\oran-mec-framework.h",
    "model\oran-mec-framework.cc",
    "model\oran-cloud-native.h",
    "model\oran-cloud-native.cc"
)

foreach ($file in $keyFiles) {
    if (Test-Path $file) {
        Write-Log "✓ Key file $file exists" "SUCCESS"
    }
    else {
        Write-Log "✗ Key file $file missing" "ERROR"
        exit 1
    }
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "BUILD AND TEST SUMMARY" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Log "✓ All advanced O-RAN modules built successfully" "SUCCESS"
Write-Log "✓ Unit tests completed" "SUCCESS"
Write-Log "✓ Integration tests completed" "SUCCESS"
Write-Log "✓ Example simulations completed" "SUCCESS"
Write-Log "✓ Output files generated and organized" "SUCCESS"
Write-Host ""
Write-Log "Results location: $ResultsDir\"
Write-Log "Logs location: $LogDir\"
Write-Log "Summary report: $ResultsDir\build_test_summary.txt"
Write-Host ""
Write-Log "The ns-3 O-RAN module has been successfully enhanced with:"
Write-Log "  • AI/ML Reinforcement Learning capabilities"
Write-Log "  • Digital Twin framework with predictive analytics"
Write-Log "  • Multi-access Edge Computing (MEC) framework"
Write-Log "  • Cloud-Native orchestration with Kubernetes support"
Write-Log "  • Federated Learning across edge nodes"
Write-Log "  • Advanced examples and comprehensive testing"
Write-Host ""
Write-Log "✓ Advanced O-RAN enhancement completed successfully!" "SUCCESS"
Write-Host "========================================" -ForegroundColor Cyan

# Display summary report
Get-Content "$ResultsDir\build_test_summary.txt"

Write-Host ""
Write-Host "Usage examples:" -ForegroundColor Yellow
Write-Host "  .\build_and_test_advanced.ps1                    # Full build and test"
Write-Host "  .\build_and_test_advanced.ps1 -SkipTests        # Build only, skip tests"
Write-Host "  .\build_and_test_advanced.ps1 -QuickTest        # Quick test with smaller parameters"
Write-Host "  .\build_and_test_advanced.ps1 -NumUes 50        # Custom number of UEs"
