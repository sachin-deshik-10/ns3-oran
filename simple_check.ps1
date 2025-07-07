# Quick O-RAN Module Validation

Write-Host "Quick O-RAN Module Validation" -ForegroundColor Cyan

$files = @(
    "model/oran-lm-reinforcement-learning.h",
    "model/oran-lm-reinforcement-learning.cc",
    "model/oran-digital-twin.h", 
    "model/oran-digital-twin.cc",
    "model/oran-mec-framework.h",
    "model/oran-mec-framework.cc",
    "model/oran-cloud-native.h",
    "model/oran-cloud-native.cc",
    "examples/oran-advanced-ai-edge-example.cc",
    "examples/oran-advanced-integration-example.cc",
    "test/oran-advanced-modules-test.cc"
)

$found = 0
foreach ($file in $files) {
    if (Test-Path $file) {
        Write-Host "✓ $file" -ForegroundColor Green
        $found++
    } else {
        Write-Host "✗ $file" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "Files found: $found / $($files.Count)" -ForegroundColor Yellow

Write-Host ""
Write-Host "Note: This is an ns-3 module that requires integration with ns-3 framework"
