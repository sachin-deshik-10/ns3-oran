# O-RAN NS-3 Simulation Setup Script for Windows
# This script installs Python dependencies and sets up the environment

Write-Host "================================================================" -ForegroundColor Green
Write-Host " O-RAN NS-3 SIMULATION SETUP" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Green
Write-Host ""

# Check if Python is installed
Write-Host "Checking Python installation..." -ForegroundColor Yellow
try {
    $pythonVersion = python --version 2>&1
    Write-Host "Found Python: $pythonVersion" -ForegroundColor Green
}
catch {
    Write-Host "ERROR: Python not found. Please install Python 3.7 or later." -ForegroundColor Red
    Write-Host "Download from: https://www.python.org/downloads/" -ForegroundColor Yellow
    exit 1
}

# Check if pip is available
Write-Host "Checking pip installation..." -ForegroundColor Yellow
try {
    $pipVersion = pip --version 2>&1
    Write-Host "Found pip: $pipVersion" -ForegroundColor Green
}
catch {
    Write-Host "ERROR: pip not found. Please ensure pip is installed." -ForegroundColor Red
    exit 1
}

# Upgrade pip
Write-Host "Upgrading pip..." -ForegroundColor Yellow
pip install --upgrade pip --quiet

# Install required packages
Write-Host "Installing Python packages..." -ForegroundColor Yellow
$requirements = @(
    "numpy>=1.21.0",
    "pandas>=1.3.0", 
    "matplotlib>=3.4.0",
    "seaborn>=0.11.0",
    "scikit-learn>=1.0.0",
    "scipy>=1.7.0",
    "networkx>=2.6.0",
    "tqdm>=4.62.0",
    "openpyxl>=3.0.0",
    "xlsxwriter>=3.0.0",
    "Pillow>=8.3.0",
    "requests>=2.26.0",
    "psutil>=5.8.0",
    "pyyaml>=5.4.0",
    "jsonschema>=3.2.0",
    "jupyter>=1.0.0",
    "notebook>=6.4.0",
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
