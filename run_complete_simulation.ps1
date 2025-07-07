#!/usr/bin/env powershell
<#
.SYNOPSIS
    Complete O-RAN simulation runner and analyzer

.DESCRIPTION
    This script runs the complete O-RAN simulation pipeline including:
    - Mock simulation execution
    - Data generation and collection
    - Performance analysis
    - Report generation
    - Visualization creation

.EXAMPLE
    .\run_complete_simulation.ps1
#>

Write-Host "================================================================" -ForegroundColor Green
Write-Host " O-RAN ENHANCED MODULE - COMPLETE SIMULATION PIPELINE" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Green
Write-Host ""

# Check Python installation
Write-Host "🔍 Checking Python installation..." -ForegroundColor Yellow
try {
    $pythonVersion = python --version 2>&1
    Write-Host "✅ Found: $pythonVersion" -ForegroundColor Green
} catch {
    Write-Error "❌ Python not found! Please install Python 3.6+ from https://python.org"
    exit 1
}

# Check pip
Write-Host "🔍 Checking pip installation..." -ForegroundColor Yellow
try {
    $pipVersion = pip --version 2>&1
    Write-Host "✅ Found: $pipVersion" -ForegroundColor Green
} catch {
    Write-Error "❌ pip not found! Please ensure Python was installed with pip."
    exit 1
}

# Install requirements
Write-Host "📦 Installing Python requirements..." -ForegroundColor Yellow
try {
    pip install -r requirements.txt --quiet
    Write-Host "✅ Requirements installed successfully" -ForegroundColor Green
} catch {
    Write-Warning "⚠️ Some packages may have failed to install - continuing anyway"
}

# Create directories
Write-Host "📁 Creating output directories..." -ForegroundColor Yellow
$directories = @("output", "results", "logs", "results\plots", "results\reports", "results\data")
foreach ($dir in $directories) {
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "✅ Created: $dir" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host " RUNNING O-RAN SIMULATION PIPELINE" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host ""

# Step 1: Run simulation
Write-Host "🚀 [1/2] Running O-RAN enhanced simulation..." -ForegroundColor Blue
try {
    python mock_simulation.py
    Write-Host "✅ Simulation completed successfully" -ForegroundColor Green
} catch {
    Write-Error "❌ Simulation failed"
    exit 1
}

Write-Host ""

# Step 2: Run analysis  
Write-Host "📊 [2/2] Running comprehensive analysis..." -ForegroundColor Blue
try {
    python simulation_analysis.py
    Write-Host "✅ Analysis completed successfully" -ForegroundColor Green
} catch {
    Write-Error "❌ Analysis failed"
    exit 1
}

Write-Host ""
Write-Host "================================================================" -ForegroundColor Green
Write-Host " 🎉 SIMULATION PIPELINE COMPLETED SUCCESSFULLY!" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Green
Write-Host ""

# Display results summary
Write-Host "📊 Generated Results:" -ForegroundColor Yellow
Write-Host "   📄 Simulation Data  : output/" -ForegroundColor White
Write-Host "   📈 Visualizations   : results/plots/" -ForegroundColor White
Write-Host "   📑 Analysis Report  : results/reports/" -ForegroundColor White
Write-Host "   💾 Processed Data   : results/data/" -ForegroundColor White
Write-Host ""

Write-Host "🔍 Key Findings:" -ForegroundColor Yellow

# Try to read and display key metrics if available
$resultsPath = "results\data"
if (Test-Path $resultsPath) {
    $latestData = Get-ChildItem "$resultsPath\*.json" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if ($latestData) {
        try {
            $data = Get-Content $latestData.FullName | ConvertFrom-Json
            Write-Host "   🎯 Simulation Duration  : $($data.simulation_metadata.simulation_time) seconds" -ForegroundColor White
            Write-Host "   📡 Network Elements     : $($data.simulation_metadata.num_enbs) eNBs, $($data.simulation_metadata.num_ues) UEs" -ForegroundColor White
            Write-Host "   🔄 Total Handovers      : $($data.simulation_metadata.total_handovers)" -ForegroundColor White
            Write-Host "   📊 Measurement Points   : $($data.simulation_metadata.total_measurements)" -ForegroundColor White
        } catch {
            Write-Host "   📊 Data files generated successfully" -ForegroundColor White
        }
    }
}

Write-Host ""
Write-Host "📋 Next Steps:" -ForegroundColor Yellow
Write-Host "   1. Review analysis report in results/reports/" -ForegroundColor White
Write-Host "   2. Examine performance plots in results/plots/" -ForegroundColor White
Write-Host "   3. Analyze raw data in results/data/" -ForegroundColor White
Write-Host "   4. Open SQLite database for custom queries" -ForegroundColor White
Write-Host ""

Write-Host "🔬 Research Applications:" -ForegroundColor Yellow
Write-Host "   • AI-driven handover optimization" -ForegroundColor White
Write-Host "   • Digital twin predictive analytics" -ForegroundColor White
Write-Host "   • Edge computing orchestration" -ForegroundColor White
Write-Host "   • Cloud-native network management" -ForegroundColor White
Write-Host ""

# Try to open results folder
try {
    if (Test-Path "results") {
        Write-Host "📂 Opening results folder..." -ForegroundColor Green
        Invoke-Item "results"
    }
} catch {
    Write-Host "📂 Results saved to 'results' folder" -ForegroundColor Green
}

Write-Host "================================================================" -ForegroundColor Green
Write-Host " Enhanced O-RAN Module - Ready for Research!" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Green
