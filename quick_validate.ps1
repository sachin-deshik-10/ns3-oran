# Quick O-RAN Module Validation Script

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Quick O-RAN Module Validation" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

$validFiles = 0
$totalFiles = 0

# Function to check file
function Test-File {
    param([string]$FilePath, [string]$Description)
    $script:totalFiles++
    if (Test-Path $FilePath) {
        $size = (Get-Item $FilePath).Length
        Write-Host "✓ $Description - Size: $([math]::Round($size / 1KB, 2)) KB" -ForegroundColor Green
        $script:validFiles++
        return $true
    } else {
        Write-Host "✗ $Description - Not found" -ForegroundColor Red
        return $false
    }
}

Write-Host "Checking core module files..."

# Check advanced modules
Test-File "model/oran-lm-reinforcement-learning.h" "RL Module Header"
Test-File "model/oran-lm-reinforcement-learning.cc" "RL Module Implementation"
Test-File "model/oran-digital-twin.h" "Digital Twin Header"
Test-File "model/oran-digital-twin.cc" "Digital Twin Implementation"
Test-File "model/oran-mec-framework.h" "MEC Framework Header"
Test-File "model/oran-mec-framework.cc" "MEC Framework Implementation"
Test-File "model/oran-cloud-native.h" "Cloud-Native Header"
Test-File "model/oran-cloud-native.cc" "Cloud-Native Implementation"

# Check examples
Test-File "examples/oran-advanced-ai-edge-example.cc" "Advanced AI Edge Example"
Test-File "examples/oran-advanced-integration-example.cc" "Advanced Integration Example"

# Check test
Test-File "test/oran-advanced-modules-test.cc" "Advanced Modules Test"

# Check build files
Test-File "CMakeLists.txt" "Main CMake File"
Test-File "examples/CMakeLists.txt" "Examples CMake File"

# Check documentation
Test-File "docs/ADVANCED_FEATURES.md" "Advanced Features Documentation"
Test-File "ENHANCEMENT_SUMMARY.md" "Enhancement Summary"

Write-Host ""
Write-Host "Validation Summary:" -ForegroundColor Yellow
Write-Host "Files found: $validFiles / $totalFiles" -ForegroundColor $(if ($validFiles -eq $totalFiles) { "Green" } else { "Yellow" })

if ($validFiles -eq $totalFiles) {
    Write-Host "✅ All advanced O-RAN module files are present!" -ForegroundColor Green
} else {
    Write-Host "⚠️ Some files are missing." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Next Steps for Running the Codebase:" -ForegroundColor Cyan
Write-Host "1. This is an ns-3 module - it requires integration with ns-3" -ForegroundColor White
Write-Host "2. Copy this folder to ns-3's contrib/ directory" -ForegroundColor White
Write-Host "3. Run: ./ns3 configure --enable-examples --enable-tests" -ForegroundColor White
Write-Host "4. Run: ./ns3 build" -ForegroundColor White
Write-Host "5. Run examples: ./ns3 run oran-advanced-integration-example" -ForegroundColor White
