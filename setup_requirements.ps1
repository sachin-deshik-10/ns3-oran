#!/usr/bin/env powershell
<#
.SYNOPSIS
    Setup script for O-RAN module requirements on Windows

.DESCRIPTION
    This script installs all necessary dependencies for running the O-RAN module simulation
    including Python requirements, SQLite, and other dependencies.

.EXAMPLE
    .\setup_requirements.ps1
#>

Write-Host "================================================================" -ForegroundColor Green
Write-Host " O-RAN MODULE SETUP - INSTALLING REQUIREMENTS" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Green

# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if (-not $isAdmin) {
    Write-Warning "This script should be run as Administrator for best results."
}

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
    "scikit-learn>=0.24.0",
    "torch>=1.8.0",
    "torchvision>=0.9.0",
    "onnx>=1.8.0",
    "onnxruntime>=1.7.0",
    "jupyterlab>=3.0.0",
    "plotly>=5.0.0",
    "sqlalchemy>=1.4.0"
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

# Check for SQLite
Write-Host "Checking SQLite installation..." -ForegroundColor Yellow
try {
    $sqliteVersion = sqlite3 -version 2>&1
    Write-Host "Found SQLite: $sqliteVersion" -ForegroundColor Green
}
catch {
    Write-Warning "SQLite not found in PATH. You may need to install SQLite manually."
    Write-Host "Download from: https://www.sqlite.org/download.html" -ForegroundColor Yellow
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
