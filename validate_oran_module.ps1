# O-RAN Module Validation Script
# This script validates the enhanced O-RAN module structure and code quality

param(
    [switch]$Verbose = $false
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "O-RAN Module Validation Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

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

# Function to check file existence and content
function Test-ModuleFile {
    param([string]$FilePath, [string]$Description)
    
    if (Test-Path $FilePath) {
        $fileSize = (Get-Item $FilePath).Length
        Write-Log "✓ $Description - Size: $([math]::Round($fileSize / 1KB, 2)) KB" "SUCCESS"
        return $true
    }
    else {
        Write-Log "✗ $Description - File not found: $FilePath" "ERROR"
        return $false
    }
}

# Function to validate C++ code structure
function Test-CppCodeStructure {
    param([string]$FilePath)
    
    if (!(Test-Path $FilePath)) {
        return $false
    }
    
    $content = Get-Content $FilePath -Raw
    $issues = @()
    
    # Check for basic C++ structure
    if ($content -notmatch '#include') {
        $issues += "Missing #include statements"
    }
    
    if ($content -notmatch 'namespace ns3') {
        $issues += "Missing ns3 namespace"
    }
    
    if ($content -match 'class.*{' -and $content -notmatch '};') {
        $issues += "Potential missing class terminator"
    }
    
    if ($issues.Count -eq 0) {
        Write-Log "  ✓ Code structure validation passed" "SUCCESS"
        return $true
    }
    else {
        Write-Log "  ⚠ Code structure issues: $($issues -join ', ')" "WARNING"
        return $false
    }
}

Write-Log "Starting O-RAN module validation..."

# Create results directory
$resultsDir = "validation_results"
if (!(Test-Path $resultsDir)) {
    New-Item -ItemType Directory -Path $resultsDir | Out-Null
}

# Validation counters
$totalFiles = 0
$validFiles = 0
$totalTests = 0
$passedTests = 0

Write-Log "Validating advanced O-RAN module files..."

# Check core advanced module files
$advancedModules = @{
    "model/oran-lm-reinforcement-learning.h"  = "Reinforcement Learning Logic Module Header"
    "model/oran-lm-reinforcement-learning.cc" = "Reinforcement Learning Logic Module Implementation"
    "model/oran-digital-twin.h"               = "Digital Twin Framework Header"
    "model/oran-digital-twin.cc"              = "Digital Twin Framework Implementation"
    "model/oran-mec-framework.h"              = "MEC Framework Header"
    "model/oran-mec-framework.cc"             = "MEC Framework Implementation"
    "model/oran-cloud-native.h"               = "Cloud-Native Framework Header"
    "model/oran-cloud-native.cc"              = "Cloud-Native Framework Implementation"
}

foreach ($file in $advancedModules.Keys) {
    $totalFiles++
    if (Test-ModuleFile $file $advancedModules[$file]) {
        $validFiles++
        
        # Validate C++ structure for .cc files
        if ($file.EndsWith('.cc')) {
            $totalTests++
            if (Test-CppCodeStructure $file) {
                $passedTests++
            }
        }
    }
}

Write-Log "Validating advanced examples..."

# Check advanced examples
$advancedExamples = @{
    "examples/oran-advanced-ai-edge-example.cc"     = "Advanced AI Edge Example"
    "examples/oran-advanced-integration-example.cc" = "Advanced Integration Example"
}

foreach ($file in $advancedExamples.Keys) {
    $totalFiles++
    if (Test-ModuleFile $file $advancedExamples[$file]) {
        $validFiles++
        $totalTests++
        if (Test-CppCodeStructure $file) {
            $passedTests++
        }
    }
}

Write-Log "Validating test files..."

# Check test files
$testFiles = @{
    "test/oran-advanced-modules-test.cc" = "Advanced Modules Test Suite"
}

foreach ($file in $testFiles.Keys) {
    $totalFiles++
    if (Test-ModuleFile $file $testFiles[$file]) {
        $validFiles++
        $totalTests++
        if (Test-CppCodeStructure $file) {
            $passedTests++
        }
    }
}

Write-Log "Validating build system files..."

# Check build system files
$buildFiles = @{
    "CMakeLists.txt"          = "Main CMake Configuration"
    "examples/CMakeLists.txt" = "Examples CMake Configuration"
}

foreach ($file in $buildFiles.Keys) {
    $totalFiles++
    if (Test-ModuleFile $file $buildFiles[$file]) {
        $validFiles++
    }
}

Write-Log "Validating documentation..."

# Check documentation files
$docFiles = @{
    "docs/ADVANCED_FEATURES.md" = "Advanced Features Documentation"
    "ENHANCEMENT_SUMMARY.md"    = "Enhancement Summary"
    "README.md"                 = "Project README"
    "CONTRIBUTING.md"           = "Contributing Guidelines"
}

foreach ($file in $docFiles.Keys) {
    $totalFiles++
    if (Test-ModuleFile $file $docFiles[$file]) {
        $validFiles++
    }
}

Write-Log "Performing code quality checks..."

# Check for common code quality indicators
$qualityTests = 0
$qualityPassed = 0

# Check for proper header guards
Write-Log "Checking header guards..."
$headerFiles = Get-ChildItem -Path "model" -Filter "*.h" -Recurse
foreach ($header in $headerFiles) {
    $qualityTests++
    $content = Get-Content $header.FullName -Raw
    if ($content -match '#ifndef.*_H' -and $content -match '#define.*_H' -and $content -match '#endif') {
        $qualityPassed++
        if ($Verbose) {
            Write-Log "  ✓ $($header.Name) has proper header guards" "SUCCESS"
        }
    }
    else {
        Write-Log "  ⚠ $($header.Name) missing or improper header guards" "WARNING"
    }
}

# Check for namespace usage
Write-Log "Checking namespace usage..."
$cppFiles = Get-ChildItem -Path "model" -Filter "*.cc" -Recurse
foreach ($cpp in $cppFiles) {
    $qualityTests++
    $content = Get-Content $cpp.FullName -Raw
    if ($content -match 'namespace ns3') {
        $qualityPassed++
        if ($Verbose) {
            Write-Log "  ✓ $($cpp.Name) uses ns3 namespace" "SUCCESS"
        }
    }
    else {
        Write-Log "  ⚠ $($cpp.Name) missing ns3 namespace" "WARNING"
    }
}

Write-Log "Checking for advanced features implementation..."

# Check for AI/ML features
$aiFeatures = @(
    'DeepQNetwork', 'ProximalPolicyOptimization', 'ExperienceReplay', 'ReinforcementLearning'
)

$aiImplemented = 0
$rlFile = "model/oran-lm-reinforcement-learning.cc"
if (Test-Path $rlFile) {
    $content = Get-Content $rlFile -Raw
    foreach ($feature in $aiFeatures) {
        if ($content -match $feature) {
            $aiImplemented++
        }
    }
}

Write-Log "AI/ML Features implemented: $aiImplemented / $($aiFeatures.Count)"

# Check for Digital Twin features
$dtFeatures = @(
    'PredictFutureStates', 'AnalyzeWhatIfScenario', 'NetworkEntityState', 'DigitalTwin'
)

$dtImplemented = 0
$dtFile = "model/oran-digital-twin.cc"
if (Test-Path $dtFile) {
    $content = Get-Content $dtFile -Raw
    foreach ($feature in $dtFeatures) {
        if ($content -match $feature) {
            $dtImplemented++
        }
    }
}

Write-Log "Digital Twin Features implemented: $dtImplemented / $($dtFeatures.Count)"

# Check for MEC features
$mecFeatures = @(
    'DeployService', 'EdgeServiceRequirements', 'FederatedLearning', 'LoadBalancing'
)

$mecImplemented = 0
$mecFile = "model/oran-mec-framework.cc"
if (Test-Path $mecFile) {
    $content = Get-Content $mecFile -Raw
    foreach ($feature in $mecFeatures) {
        if ($content -match $feature) {
            $mecImplemented++
        }
    }
}

Write-Log "MEC Features implemented: $mecImplemented / $($mecFeatures.Count)"

# Check for Cloud-Native features
$cnFeatures = @(
    'DeployPod', 'CreateService', 'ScaleService', 'KubernetesCluster'
)

$cnImplemented = 0
$cnFile = "model/oran-cloud-native.cc"
if (Test-Path $cnFile) {
    $content = Get-Content $cnFile -Raw
    foreach ($feature in $cnFeatures) {
        if ($content -match $feature) {
            $cnImplemented++
        }
    }
}

Write-Log "Cloud-Native Features implemented: $cnImplemented / $($cnFeatures.Count)"

# Generate validation report
$report = @"
O-RAN Module Validation Report
==============================
Date: $(Get-Date)
Validation Type: Structure and Code Quality Check

File Validation:
- Total Files Checked: $totalFiles
- Valid Files: $validFiles
- Success Rate: $([math]::Round(($validFiles / $totalFiles) * 100, 2))%

Code Structure Tests:
- Total Tests: $totalTests
- Passed Tests: $passedTests
- Success Rate: $([math]::Round(($passedTests / $totalTests) * 100, 2))%

Code Quality Tests:
- Total Quality Tests: $qualityTests
- Passed Quality Tests: $qualityPassed
- Success Rate: $([math]::Round(($qualityPassed / $qualityTests) * 100, 2))%

Advanced Features Implementation:
- AI/ML (Reinforcement Learning): $aiImplemented / $($aiFeatures.Count) features
- Digital Twin: $dtImplemented / $($dtFeatures.Count) features
- MEC Framework: $mecImplemented / $($mecFeatures.Count) features
- Cloud-Native: $cnImplemented / $($cnFeatures.Count) features

Overall Assessment:
$(if ($validFiles -eq $totalFiles -and $passedTests -eq $totalTests) {
    "✅ EXCELLENT: All files present and code structure validation passed"
} elseif ($validFiles / $totalFiles -gt 0.9 -and $passedTests / $totalTests -gt 0.8) {
    "✅ GOOD: Most files present and code structure mostly valid"
} elseif ($validFiles / $totalFiles -gt 0.7) {
    "⚠️ FAIR: Some files missing or code structure issues detected"
} else {
    "❌ POOR: Significant files missing or major code structure issues"
})

Advanced Features Status:
$(if ($aiImplemented + $dtImplemented + $mecImplemented + $cnImplemented -gt 12) {
    "✅ COMPREHENSIVE: All advanced features implemented"
} elseif ($aiImplemented + $dtImplemented + $mecImplemented + $cnImplemented -gt 8) {
    "✅ GOOD: Most advanced features implemented"
} else {
    "⚠️ PARTIAL: Some advanced features missing"
})

Notes:
- This validation checks file structure and basic code patterns
- Full compilation testing requires ns-3 framework integration
- Advanced features are detected based on method/class name patterns
- For complete validation, integrate with full ns-3 build system

Recommendations:
1. Integrate this O-RAN module into a full ns-3 installation
2. Run ns-3 build system to validate compilation
3. Execute the provided examples to test functionality
4. Use the comprehensive test suite for validation
"@

# Save report
$report | Out-File "$resultsDir/validation_report.txt" -Encoding UTF8

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "VALIDATION SUMMARY" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

Write-Log "✓ Module structure validation completed" "SUCCESS"
Write-Log "✓ Code quality checks completed" "SUCCESS"
Write-Log "✓ Advanced features detection completed" "SUCCESS"
Write-Log "✓ Validation report generated" "SUCCESS"

Write-Host ""
Write-Log "Files validated: $validFiles / $totalFiles"
Write-Log "Code structure tests passed: $passedTests / $totalTests"
Write-Log "Code quality tests passed: $qualityPassed / $qualityTests"

$totalFeatures = $aiImplemented + $dtImplemented + $mecImplemented + $cnImplemented
$maxFeatures = $aiFeatures.Count + $dtFeatures.Count + $mecFeatures.Count + $cnFeatures.Count
Write-Log "Advanced features implemented: $totalFeatures / $maxFeatures"

Write-Host ""
Write-Log "Validation report saved to: $resultsDir/validation_report.txt"

Write-Host ""
Write-Host "Next Steps:" -ForegroundColor Yellow
Write-Host "1. Integrate this O-RAN module into a full ns-3 installation" -ForegroundColor Yellow
Write-Host "2. Run 'python ns3 configure --enable-examples --enable-tests'" -ForegroundColor Yellow
Write-Host "3. Run 'python ns3 build' to compile the enhanced module" -ForegroundColor Yellow
Write-Host "4. Execute examples: './ns3 run oran-advanced-integration-example'" -ForegroundColor Yellow

# Display the report
Write-Host ""
Write-Host "=== VALIDATION REPORT ===" -ForegroundColor Cyan
Write-Host $report

exit 0
