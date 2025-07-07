@echo off
echo ================================================================
echo  O-RAN ENHANCED MODULE - AUTOMATED EXECUTION
echo ================================================================
echo.

REM Check if Python is installed
python --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.6+ from https://python.org
    pause
    exit /b 1
)

REM Check if pip is available
pip --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: pip is not available
    echo Please ensure Python was installed with pip
    pause
    exit /b 1
)

echo Installing Python requirements...
pip install -r requirements.txt --quiet
if %ERRORLEVEL% neq 0 (
    echo WARNING: Some packages may have failed to install
    echo Continuing anyway...
)

echo.
echo Creating output directories...
if not exist "output" mkdir output
if not exist "results" mkdir results
if not exist "logs" mkdir logs

echo.
echo ================================================================
echo  RUNNING O-RAN SIMULATION
echo ================================================================
echo.

REM Run the mock simulation
echo [1/2] Running O-RAN simulation mock...
python mock_simulation.py
if %ERRORLEVEL% neq 0 (
    echo ERROR: Simulation failed
    pause
    exit /b 1
)

echo.
echo [2/2] Running comprehensive analysis...
python simulation_analysis.py
if %ERRORLEVEL% neq 0 (
    echo ERROR: Analysis failed
    pause
    exit /b 1
)

echo.
echo ================================================================
echo  EXECUTION COMPLETED SUCCESSFULLY!
echo ================================================================
echo.
echo Generated files:
echo   - output/           : Simulation raw data
echo   - results/plots/    : Performance visualizations  
echo   - results/reports/  : Comprehensive analysis report
echo   - results/data/     : Processed datasets
echo.
echo Next steps:
echo   1. Open results/reports/ to view the analysis report
echo   2. Check results/plots/ for performance graphs
echo   3. Examine results/data/ for detailed datasets
echo.
pause
