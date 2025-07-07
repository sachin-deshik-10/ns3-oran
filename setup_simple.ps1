#!/usr/bin/env powershell
# O-RAN Module Setup Script for Windows

Write-Host "================================================================" -ForegroundColor Green
Write-Host " O-RAN MODULE SETUP - INSTALLING REQUIREMENTS" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Green

# Check Python installation
Write-Host "Checking Python installation..." -ForegroundColor Yellow
try {
    $pythonVersion = python --version 2>&1
    Write-Host "Found: $pythonVersion" -ForegroundColor Green
}
catch {
    Write-Error "Python not found! Please install Python 3.6+ from https://python.org"
    exit 1
}

# Check if pip is available
Write-Host "Checking pip installation..." -ForegroundColor Yellow
try {
    $pipVersion = pip --version 2>&1
    Write-Host "Found: $pipVersion" -ForegroundColor Green
}
catch {
    Write-Error "pip not found! Please ensure Python was installed with pip."
    exit 1
}

# Install Python requirements
Write-Host "Installing Python requirements..." -ForegroundColor Yellow

$requirements = @(
    "numpy>=1.19.0",
    "pandas>=1.2.0", 
    "matplotlib>=3.3.0",
    "seaborn>=0.11.0",
    "scipy>=1.6.0",
    "scikit-learn>=0.24.0"
)

foreach ($package in $requirements) {
    Write-Host "Installing $package..." -ForegroundColor Cyan
    try {
        pip install $package --quiet
        Write-Host "✓ $package installed successfully" -ForegroundColor Green
    }
    catch {
        Write-Warning "Failed to install $package - continuing anyway"
    }
}

# Create output directories
Write-Host "Creating output directories..." -ForegroundColor Yellow
$outputDirs = @("output", "logs", "results", "plots", "data")
foreach ($dir in $outputDirs) {
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "✓ Created directory: $dir" -ForegroundColor Green
    }
    else {
        Write-Host "✓ Directory already exists: $dir" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "================================================================" -ForegroundColor Green
Write-Host " SETUP COMPLETED SUCCESSFULLY!" -ForegroundColor Green  
Write-Host "================================================================" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Run the simulation: python mock_simulation.py" -ForegroundColor White
Write-Host "2. Or run analysis script: python simulation_analysis.py" -ForegroundColor White
Write-Host "3. Check results in the 'results' directory" -ForegroundColor White
Write-Host ""
