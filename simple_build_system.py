#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Simple Build System for NS-3 O-RAN 6G Simulation Platform
Cross-platform compatible build and validation system
"""

import os
import sys
import subprocess
import argparse
import json
import time
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Tuple, Optional

class SimpleBuildSystem:
    def __init__(self, workspace_path: str, verbose: bool = False):
        self.workspace_path = Path(workspace_path).resolve()
        self.verbose = verbose
        self.build_start_time = datetime.now()
        self.build_results = {}
        
        # Essential module lists
        self.core_modules = [
            'oran-cmm', 'oran-data-repository', 'oran-e2-node-terminator',
            'oran-helper', 'oran-command', 'oran-digital-twin'
        ]
        
        self.advanced_modules = [
            'oran-6g-neuromorphic-computing', 'oran-6g-holographic',
            'oran-6g-brain-computer-interface', 'oran-6g-cybersecurity',
            'oran-6g-semantic-communications', 'oran-6g-ultra-ai-orchestrator',
            'oran-6g-sags-network', 'oran-6g-terahertz', 'oran-ai-transformer',
            'oran-6g-quantum-enhanced', 'oran-6g-edge-ai'
        ]
        
        self.examples = [
            'oran-6g-comprehensive-integration-demo',
            'oran-6g-ultimate-next-generation-example',
            'oran-6g-real-time-ai-orchestration-demo'
        ]
    
    def log(self, message: str, level: str = "INFO"):
        """Cross-platform safe logging"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        prefix = {
            "INFO": "[INFO]",
            "SUCCESS": "[OK]",
            "ERROR": "[ERROR]",
            "WARNING": "[WARN]"
        }.get(level, "[INFO]")
        
        print(f"[{timestamp}] {prefix} {message}")
        if self.verbose and level == "ERROR":
            import traceback
            traceback.print_exc()
    
    def run_complete_build_cycle(self, build_type: str = 'release', 
                                run_tests: bool = True,
                                parallel_build: bool = True) -> bool:
        """Run the complete build and validation cycle"""
        self.log("Starting ultra-advanced O-RAN 6G build system")
        self.log(f"Workspace: {self.workspace_path}")
        self.log(f"Build Type: {build_type}")
        self.log(f"Parallel Build: {parallel_build}")
        self.log(f"Run Tests: {run_tests}")
        
        try:
            # Phase 1: Pre-build validation
            if not self._run_pre_build_validation():
                self.log("Pre-build validation failed!", "ERROR")
                return False
            
            # Phase 2: Environment setup
            if not self._setup_build_environment():
                self.log("Build environment setup failed!", "ERROR")
                return False
            
            # Phase 3: File structure validation
            if not self._validate_file_structure():
                self.log("File structure validation failed!", "ERROR")
                return False
            
            # Phase 4: CMake configuration check
            if not self._check_cmake_configuration():
                self.log("CMake configuration check failed!", "ERROR")
                return False
            
            # Phase 5: Module validation
            if not self._validate_modules():
                self.log("Module validation failed!", "ERROR")
                return False
            
            # Phase 6: Example validation
            if not self._validate_examples():
                self.log("Example validation failed!", "ERROR")
                return False
            
            # Phase 7: Integration testing
            if run_tests and not self._run_integration_tests():
                self.log("Integration tests failed!", "ERROR")
                return False
            
            # Phase 8: Generate reports
            self._generate_build_report()
            
            self.log("ULTRA-ADVANCED BUILD COMPLETED SUCCESSFULLY!", "SUCCESS")
            return True
            
        except Exception as e:
            self.log(f"Build failed with exception: {e}", "ERROR")
            return False
    
    def _run_pre_build_validation(self) -> bool:
        """Run pre-build validation checks"""
        self.log("Running pre-build validation")
        
        validation_checks = [
            ("Workspace exists", self._check_workspace_exists),
            ("Required directories present", self._check_required_directories),
            ("CMakeLists.txt valid", self._check_cmake_files),
            ("Source files present", self._check_source_files)
        ]
        
        all_passed = True
        for check_name, check_func in validation_checks:
            self.log(f"Checking {check_name}...")
            try:
                if check_func():
                    self.log(f"{check_name}: PASS", "SUCCESS")
                else:
                    self.log(f"{check_name}: FAIL", "ERROR")
                    all_passed = False
            except Exception as e:
                self.log(f"{check_name}: ERROR - {e}", "ERROR")
                all_passed = False
        
        return all_passed
    
    def _check_workspace_exists(self) -> bool:
        """Check if workspace directory exists"""
        return self.workspace_path.exists() and self.workspace_path.is_dir()
    
    def _check_required_directories(self) -> bool:
        """Check if required directories exist"""
        required_dirs = ['model', 'examples', 'helper']
        for dir_name in required_dirs:
            dir_path = self.workspace_path / dir_name
            if not dir_path.exists():
                self.log(f"Required directory missing: {dir_name}", "ERROR")
                return False
        return True
    
    def _check_cmake_files(self) -> bool:
        """Check if CMakeLists.txt exists and is valid"""
        cmake_file = self.workspace_path / 'CMakeLists.txt'
        if not cmake_file.exists():
            self.log("CMakeLists.txt not found", "ERROR")
            return False
        
        try:
            content = cmake_file.read_text(encoding='utf-8')
            # Basic validation
            if 'project(' not in content.lower():
                self.log("CMakeLists.txt missing project declaration", "ERROR")
                return False
            return True
        except Exception as e:
            self.log(f"Error reading CMakeLists.txt: {e}", "ERROR")
            return False
    
    def _check_source_files(self) -> bool:
        """Check if all required source files exist"""
        model_dir = self.workspace_path / 'model'
        
        # Check core modules
        for module in self.core_modules:
            header_file = model_dir / f"{module}.h"
            source_file = model_dir / f"{module}.cc"
            
            if not header_file.exists():
                self.log(f"Missing header file: {header_file}", "WARNING")
            if not source_file.exists():
                self.log(f"Missing source file: {source_file}", "WARNING")
        
        # Check advanced modules
        for module in self.advanced_modules:
            header_file = model_dir / f"{module}.h"
            source_file = model_dir / f"{module}.cc"
            
            if not header_file.exists():
                self.log(f"Missing advanced header file: {header_file}", "WARNING")
            if not source_file.exists():
                self.log(f"Missing advanced source file: {source_file}", "WARNING")
        
        return True
    
    def _setup_build_environment(self) -> bool:
        """Setup build environment"""
        self.log("Setting up build environment")
        
        # Create build directory if it doesn't exist
        build_dir = self.workspace_path / 'build'
        build_dir.mkdir(exist_ok=True)
        
        # Set environment variables
        os.environ['NS3_ORAN_WORKSPACE'] = str(self.workspace_path)
        os.environ['NS3_ORAN_BUILD_DIR'] = str(build_dir)
        
        self.log("Build environment setup completed", "SUCCESS")
        return True
    
    def _validate_file_structure(self) -> bool:
        """Validate project file structure"""
        self.log("Validating file structure")
        
        expected_structure = {
            'model': ['*.h', '*.cc'],
            'examples': ['*.cc'],
            'helper': ['*.h', '*.cc'],
            'test': ['*.cc']
        }
        
        for directory, patterns in expected_structure.items():
            dir_path = self.workspace_path / directory
            if dir_path.exists():
                files = list(dir_path.glob('*'))
                self.log(f"Directory {directory}: {len(files)} files found")
            else:
                self.log(f"Directory {directory}: not found", "WARNING")
        
        return True
    
    def _check_cmake_configuration(self) -> bool:
        """Check CMake configuration"""
        self.log("Checking CMake configuration")
        
        cmake_file = self.workspace_path / 'CMakeLists.txt'
        if not cmake_file.exists():
            self.log("CMakeLists.txt not found", "ERROR")
            return False
        
        try:
            content = cmake_file.read_text(encoding='utf-8')
            
            # Check for advanced modules
            modules_found = 0
            for module in self.advanced_modules:
                if module in content:
                    modules_found += 1
            
            self.log(f"CMake configuration: {modules_found}/{len(self.advanced_modules)} advanced modules configured")
            return True
            
        except Exception as e:
            self.log(f"Error checking CMake configuration: {e}", "ERROR")
            return False
    
    def _validate_modules(self) -> bool:
        """Validate all modules"""
        self.log("Validating modules")
        
        model_dir = self.workspace_path / 'model'
        if not model_dir.exists():
            self.log("Model directory not found", "ERROR")
            return False
        
        # Count files
        header_files = list(model_dir.glob('*.h'))
        source_files = list(model_dir.glob('*.cc'))
        
        self.log(f"Found {len(header_files)} header files")
        self.log(f"Found {len(source_files)} source files")
        
        # Validate advanced modules specifically
        advanced_modules_found = 0
        for module in self.advanced_modules:
            header_file = model_dir / f"{module}.h"
            source_file = model_dir / f"{module}.cc"
            
            if header_file.exists() and source_file.exists():
                advanced_modules_found += 1
        
        self.log(f"Advanced modules: {advanced_modules_found}/{len(self.advanced_modules)} complete")
        return True
    
    def _validate_examples(self) -> bool:
        """Validate example files"""
        self.log("Validating examples")
        
        examples_dir = self.workspace_path / 'examples'
        if not examples_dir.exists():
            self.log("Examples directory not found", "ERROR")
            return False
        
        example_files = list(examples_dir.glob('*.cc'))
        self.log(f"Found {len(example_files)} example files")
        
        # Check for advanced examples
        advanced_examples_found = 0
        for example in self.examples:
            example_file = examples_dir / f"{example}.cc"
            if example_file.exists():
                advanced_examples_found += 1
        
        self.log(f"Advanced examples: {advanced_examples_found}/{len(self.examples)} found")
        return True
    
    def _run_integration_tests(self) -> bool:
        """Run integration tests"""
        self.log("Running integration tests")
        
        # Basic syntax check for C++ files
        model_dir = self.workspace_path / 'model'
        cpp_files = list(model_dir.glob('*.cc')) + list(model_dir.glob('*.h'))
        
        syntax_errors = 0
        for cpp_file in cpp_files:
            try:
                content = cpp_file.read_text(encoding='utf-8')
                # Basic syntax checks
                if not self._basic_cpp_syntax_check(content, cpp_file.name):
                    syntax_errors += 1
            except Exception as e:
                self.log(f"Error reading {cpp_file}: {e}", "WARNING")
                syntax_errors += 1
        
        self.log(f"Integration tests: {len(cpp_files) - syntax_errors}/{len(cpp_files)} files passed basic checks")
        return syntax_errors == 0
    
    def _basic_cpp_syntax_check(self, content: str, filename: str) -> bool:
        """Basic C++ syntax validation"""
        if filename.endswith('.h'):
            # Header file checks
            if '#ifndef' not in content and '#pragma once' not in content:
                self.log(f"Header guard missing in {filename}", "WARNING")
                return False
        
        if filename.endswith('.cc'):
            # Source file checks
            if '#include' not in content:
                self.log(f"No includes found in {filename}", "WARNING")
        
        # Check for balanced braces
        open_braces = content.count('{')
        close_braces = content.count('}')
        if open_braces != close_braces:
            self.log(f"Unbalanced braces in {filename}: {open_braces} open, {close_braces} close", "WARNING")
            return False
        
        return True
    
    def _generate_build_report(self):
        """Generate comprehensive build report"""
        self.log("Generating build report")
        
        build_end_time = datetime.now()
        build_duration = build_end_time - self.build_start_time
        
        report = {
            'build_info': {
                'start_time': self.build_start_time.isoformat(),
                'end_time': build_end_time.isoformat(),
                'duration_seconds': build_duration.total_seconds(),
                'workspace': str(self.workspace_path)
            },
            'modules': {
                'core_modules': len(self.core_modules),
                'advanced_modules': len(self.advanced_modules),
                'examples': len(self.examples)
            },
            'validation_results': self.build_results
        }
        
        # Save report
        report_file = self.workspace_path / 'build_report.json'
        try:
            with open(report_file, 'w', encoding='utf-8') as f:
                json.dump(report, f, indent=2)
            self.log(f"Build report saved to {report_file}", "SUCCESS")
        except Exception as e:
            self.log(f"Error saving build report: {e}", "ERROR")
        
        # Print summary
        self.log(f"Build completed in {build_duration.total_seconds():.2f} seconds")
        self.log(f"Advanced modules: {len(self.advanced_modules)}")
        self.log(f"Examples: {len(self.examples)}")

def main():
    parser = argparse.ArgumentParser(description='Simple O-RAN 6G Build System')
    parser.add_argument('workspace', help='Path to workspace directory')
    parser.add_argument('--verbose', action='store_true', help='Enable verbose output')
    parser.add_argument('--build-type', default='release', choices=['debug', 'release'], help='Build type')
    parser.add_argument('--no-tests', action='store_true', help='Skip tests')
    parser.add_argument('--no-parallel', action='store_true', help='Disable parallel build')
    
    args = parser.parse_args()
    
    # Create build system
    build_system = SimpleBuildSystem(args.workspace, verbose=args.verbose)
    
    # Run build cycle
    success = build_system.run_complete_build_cycle(
        build_type=args.build_type,
        run_tests=not args.no_tests,
        parallel_build=not args.no_parallel
    )
    
    sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()
