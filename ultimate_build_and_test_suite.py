#!/usr/bin/env python3
"""
Ultimate O-RAN 6G Build and Test Suite
Comprehensive build and validation system for all ultra-advanced modules.
"""

import os
import sys
import subprocess
import time
import json
import shutil
from pathlib import Path
from datetime import datetime
import concurrent.futures
import threading

class UltimateORANBuilder:
    """Ultimate build and test system for O-RAN 6G modules."""
    
    def __init__(self, workspace_path, build_path=None):
        self.workspace_path = Path(workspace_path)
        self.build_path = Path(build_path) if build_path else self.workspace_path / "build"
        self.results = {}
        self.start_time = datetime.now()
        
        # Define all modules and their dependencies
        self.ultra_advanced_modules = {
            "oran-6g-terahertz": {
                "header": "model/oran-6g-terahertz.h",
                "implementation": "model/oran-6g-terahertz.cc",
                "dependencies": ["ns3-core", "ns3-network"],
                "category": "core"
            },
            "oran-ai-transformer": {
                "header": "model/oran-ai-transformer.h", 
                "implementation": "model/oran-ai-transformer.cc",
                "dependencies": ["ns3-core", "ns3-network"],
                "category": "core"
            },
            "oran-6g-network-slicing": {
                "header": "model/oran-6g-network-slicing.h",
                "implementation": "model/oran-6g-network-slicing.cc", 
                "dependencies": ["ns3-core", "ns3-network"],
                "category": "core"
            },
            "oran-6g-sags-network": {
                "header": "model/oran-6g-sags-network.h",
                "implementation": "model/oran-6g-sags-network.cc",
                "dependencies": ["ns3-core", "ns3-network", "ns3-mobility"],
                "category": "ultra-advanced"
            },
            "oran-6g-quantum-enhanced": {
                "header": "model/oran-6g-quantum-enhanced.h",
                "implementation": "model/oran-6g-quantum-enhanced.cc",
                "dependencies": ["ns3-core", "ns3-network", "ns3-applications"],
                "category": "ultra-advanced"
            },
            "oran-6g-edge-ai": {
                "header": "model/oran-6g-edge-ai.h",
                "implementation": "model/oran-6g-edge-ai.cc",
                "dependencies": ["ns3-core", "ns3-network", "ns3-applications"],
                "category": "ultra-advanced"
            },
            "oran-6g-semantic-communications": {
                "header": "model/oran-6g-semantic-communications.h",
                "implementation": None,  # Header-only
                "dependencies": ["ns3-core", "ns3-network"],
                "category": "ultra-advanced"
            },
            "oran-6g-brain-computer-interface": {
                "header": "model/oran-6g-brain-computer-interface.h",
                "implementation": None,  # Header-only
                "dependencies": ["ns3-core", "ns3-applications"],
                "category": "ultra-advanced"
            },
            "oran-6g-neuromorphic-computing": {
                "header": "model/oran-6g-neuromorphic-computing.h",
                "implementation": None,  # Header-only
                "dependencies": ["ns3-core", "ns3-network"],
                "category": "ultra-advanced"
            },
            "oran-6g-holographic": {
                "header": "model/oran-6g-holographic.h",
                "implementation": None,  # Header-only
                "dependencies": ["ns3-core", "ns3-applications"],
                "category": "advanced"
            },
            "oran-6g-performance-optimizer": {
                "header": "model/oran-6g-performance-optimizer.h",
                "implementation": None,  # Header-only
                "dependencies": ["ns3-core"],
                "category": "advanced"
            },
            "oran-6g-industrial-iot": {
                "header": "model/oran-6g-industrial-iot.h",
                "implementation": None,  # Header-only
                "dependencies": ["ns3-core", "ns3-network", "ns3-applications"],
                "category": "advanced"
            },
            "oran-6g-metaverse": {
                "header": "model/oran-6g-metaverse.h",
                "implementation": None,  # Header-only
                "dependencies": ["ns3-core", "ns3-applications"],
                "category": "advanced"
            },
            "oran-6g-cybersecurity": {
                "header": "model/oran-6g-cybersecurity.h",
                "implementation": None,  # Header-only
                "dependencies": ["ns3-core", "ns3-network"],
                "category": "advanced"
            }
        }
        
        # Define comprehensive examples
        self.comprehensive_examples = {
            "oran-6g-comprehensive-advanced-example": {
                "file": "examples/oran-6g-comprehensive-advanced-example.cc",
                "dependencies": ["liboran", "libnetwork", "liblte", "libmobility", "libapplications"],
                "category": "comprehensive"
            },
            "oran-6g-next-generation-advanced-example": {
                "file": "examples/oran-6g-next-generation-advanced-example.cc",
                "dependencies": ["liboran", "libnetwork", "liblte", "libmobility", "libapplications"],
                "category": "next-generation"
            },
            "oran-6g-ultimate-next-generation-example": {
                "file": "examples/oran-6g-ultimate-next-generation-example.cc",
                "dependencies": ["liboran", "libnetwork", "liblte", "libmobility", "libapplications"],
                "category": "ultimate"
            }
        }

    def log_message(self, message, level="INFO"):
        """Log messages with timestamp."""
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] [{level}] {message}")

    def check_prerequisites(self):
        """Check build prerequisites and dependencies."""
        self.log_message("Checking build prerequisites")
        
        prerequisites = {
            "cmake": ["cmake", "--version"],
            "make": ["make", "--version"],
            "gcc": ["gcc", "--version"],
            "g++": ["g++", "--version"],
            "python": ["python", "--version"]
        }
        
        results = {}
        for tool, cmd in prerequisites.items():
            try:
                result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
                results[tool] = {
                    "available": result.returncode == 0,
                    "version": result.stdout.split('\n')[0] if result.returncode == 0 else None,
                    "error": result.stderr if result.returncode != 0 else None
                }
            except Exception as e:
                results[tool] = {
                    "available": False,
                    "error": str(e)
                }
        
        return results

    def validate_source_files(self):
        """Validate all source files exist and are readable."""
        self.log_message("Validating source files")
        
        results = {}
        
        # Check module files
        for module_name, module_info in self.ultra_advanced_modules.items():
            module_results = {
                "header": self.validate_file(module_info["header"]),
                "implementation": None
            }
            
            if module_info["implementation"]:
                module_results["implementation"] = self.validate_file(module_info["implementation"])
            
            results[module_name] = module_results
        
        # Check example files
        for example_name, example_info in self.comprehensive_examples.items():
            results[example_name] = {
                "file": self.validate_file(example_info["file"])
            }
        
        return results

    def validate_file(self, file_path):
        """Validate a single file."""
        full_path = self.workspace_path / file_path
        
        if not full_path.exists():
            return {"exists": False, "error": "File not found"}
        
        if not full_path.is_file():
            return {"exists": False, "error": "Not a file"}
        
        try:
            # Check if file is readable and has content
            with open(full_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            return {
                "exists": True,
                "size": len(content),
                "lines": len(content.split('\n')),
                "readable": True
            }
        except Exception as e:
            return {
                "exists": True,
                "readable": False,
                "error": str(e)
            }

    def prepare_build_environment(self):
        """Prepare the build environment."""
        self.log_message("Preparing build environment")
        
        # Create build directory
        self.build_path.mkdir(exist_ok=True)
        
        # Check for existing CMakeCache.txt and remove if needed
        cmake_cache = self.build_path / "CMakeCache.txt"
        if cmake_cache.exists():
            self.log_message("Removing existing CMake cache")
            cmake_cache.unlink()
        
        return {"build_dir_created": True, "cache_cleared": True}

    def run_cmake_configure(self):
        """Run CMake configuration."""
        self.log_message("Running CMake configuration")
        
        try:
            # Basic CMake configuration
            cmake_cmd = [
                "cmake",
                "-DCMAKE_BUILD_TYPE=Release",
                "-DENABLE_SQLITE=ON",
                str(self.workspace_path)
            ]
            
            result = subprocess.run(
                cmake_cmd,
                cwd=self.build_path,
                capture_output=True,
                text=True,
                timeout=300  # 5 minutes timeout
            )
            
            return {
                "success": result.returncode == 0,
                "output": result.stdout,
                "error": result.stderr,
                "return_code": result.returncode
            }
        
        except subprocess.TimeoutExpired:
            return {
                "success": False,
                "error": "CMake configuration timed out",
                "timeout": True
            }
        except Exception as e:
            return {
                "success": False,
                "error": str(e)
            }

    def run_build(self, parallel_jobs=None):
        """Run the actual build process."""
        self.log_message("Starting build process")
        
        if parallel_jobs is None:
            import multiprocessing
            parallel_jobs = min(multiprocessing.cpu_count(), 8)
        
        try:
            # Build command
            build_cmd = ["make", f"-j{parallel_jobs}"]
            
            result = subprocess.run(
                build_cmd,
                cwd=self.build_path,
                capture_output=True,
                text=True,
                timeout=1800  # 30 minutes timeout
            )
            
            return {
                "success": result.returncode == 0,
                "output": result.stdout,
                "error": result.stderr,
                "return_code": result.returncode,
                "parallel_jobs": parallel_jobs
            }
        
        except subprocess.TimeoutExpired:
            return {
                "success": False,
                "error": "Build process timed out",
                "timeout": True
            }
        except Exception as e:
            return {
                "success": False,
                "error": str(e)
            }

    def run_syntax_validation(self):
        """Run syntax validation for header files."""
        self.log_message("Running syntax validation")
        
        results = {}
        
        # Test compile header files
        for module_name, module_info in self.ultra_advanced_modules.items():
            if module_info["category"] == "ultra-advanced":
                header_path = self.workspace_path / module_info["header"]
                
                if header_path.exists():
                    syntax_result = self.test_compile_header(header_path)
                    results[module_name] = syntax_result
        
        return results

    def test_compile_header(self, header_path):
        """Test compile a single header file."""
        try:
            # Create temporary test file
            test_content = f"""
#include <iostream>
#include "{header_path.relative_to(self.workspace_path)}"

int main() {{
    std::cout << "Header compilation test passed" << std::endl;
    return 0;
}}
"""
            
            test_file = self.build_path / f"test_{header_path.stem}.cc"
            with open(test_file, 'w') as f:
                f.write(test_content)
            
            # Try to compile
            compile_cmd = [
                "g++", "-c", "-std=c++17",
                f"-I{self.workspace_path}",
                f"-I{self.workspace_path}/build",
                str(test_file)
            ]
            
            result = subprocess.run(
                compile_cmd,
                capture_output=True,
                text=True,
                timeout=60
            )
            
            # Clean up
            if test_file.exists():
                test_file.unlink()
            
            return {
                "success": result.returncode == 0,
                "output": result.stdout,
                "error": result.stderr
            }
        
        except Exception as e:
            return {
                "success": False,
                "error": str(e)
            }

    def run_example_tests(self):
        """Run tests on comprehensive examples."""
        self.log_message("Testing comprehensive examples")
        
        results = {}
        
        for example_name, example_info in self.comprehensive_examples.items():
            example_path = self.workspace_path / example_info["file"]
            
            if example_path.exists():
                # Check if example was built
                binary_path = self.build_path / "examples" / example_name
                
                if binary_path.exists():
                    # Try to run the example with a short simulation
                    test_result = self.test_run_example(binary_path)
                    results[example_name] = test_result
                else:
                    results[example_name] = {
                        "built": False,
                        "error": "Binary not found"
                    }
            else:
                results[example_name] = {
                    "exists": False,
                    "error": "Source file not found"
                }
        
        return results

    def test_run_example(self, binary_path):
        """Test run a single example."""
        try:
            # Run with minimal parameters
            run_cmd = [str(binary_path), "--simulationTime=1"]
            
            result = subprocess.run(
                run_cmd,
                capture_output=True,
                text=True,
                timeout=60  # 1 minute timeout for quick test
            )
            
            return {
                "success": result.returncode == 0,
                "output": result.stdout,
                "error": result.stderr,
                "return_code": result.returncode
            }
        
        except subprocess.TimeoutExpired:
            return {
                "success": False,
                "error": "Example execution timed out",
                "timeout": True
            }
        except Exception as e:
            return {
                "success": False,
                "error": str(e)
            }

    def run_comprehensive_build_and_test(self):
        """Run comprehensive build and test process."""
        self.log_message("Starting Ultimate O-RAN 6G Build and Test Suite")
        
        # Step 1: Check prerequisites
        prereq_results = self.check_prerequisites()
        self.results["prerequisites"] = prereq_results
        
        # Step 2: Validate source files
        source_results = self.validate_source_files()
        self.results["source_validation"] = source_results
        
        # Step 3: Prepare build environment
        build_env_results = self.prepare_build_environment()
        self.results["build_environment"] = build_env_results
        
        # Step 4: Run CMake configuration
        cmake_results = self.run_cmake_configure()
        self.results["cmake_configuration"] = cmake_results
        
        if not cmake_results["success"]:
            self.log_message("CMake configuration failed, stopping build", "ERROR")
            return self.generate_final_report()
        
        # Step 5: Run build
        build_results = self.run_build()
        self.results["build"] = build_results
        
        if not build_results["success"]:
            self.log_message("Build failed, skipping tests", "WARNING")
        else:
            # Step 6: Run syntax validation
            syntax_results = self.run_syntax_validation()
            self.results["syntax_validation"] = syntax_results
            
            # Step 7: Run example tests
            example_results = self.run_example_tests()
            self.results["example_tests"] = example_results
        
        # Generate final report
        return self.generate_final_report()

    def generate_final_report(self):
        """Generate comprehensive final report."""
        end_time = datetime.now()
        duration = end_time - self.start_time
        
        # Calculate summary statistics
        summary = {
            "start_time": self.start_time.isoformat(),
            "end_time": end_time.isoformat(),
            "duration_seconds": duration.total_seconds(),
            "overall_success": self.calculate_overall_success(),
            "phase_results": {}
        }
        
        # Analyze each phase
        if "prerequisites" in self.results:
            prereq_success = sum(1 for r in self.results["prerequisites"].values() if r.get("available", False))
            prereq_total = len(self.results["prerequisites"])
            summary["phase_results"]["prerequisites"] = {
                "success": prereq_success,
                "total": prereq_total,
                "pass_rate": f"{prereq_success/prereq_total*100:.1f}%"
            }
        
        if "source_validation" in self.results:
            source_success = sum(1 for module_result in self.results["source_validation"].values() 
                               for file_result in module_result.values() 
                               if file_result and file_result.get("exists", False))
            source_total = sum(len(module_result) for module_result in self.results["source_validation"].values())
            summary["phase_results"]["source_validation"] = {
                "success": source_success,
                "total": source_total,
                "pass_rate": f"{source_success/source_total*100:.1f}%" if source_total > 0 else "0%"
            }
        
        if "cmake_configuration" in self.results:
            summary["phase_results"]["cmake_configuration"] = {
                "success": self.results["cmake_configuration"]["success"]
            }
        
        if "build" in self.results:
            summary["phase_results"]["build"] = {
                "success": self.results["build"]["success"],
                "parallel_jobs": self.results["build"].get("parallel_jobs", "N/A")
            }
        
        if "syntax_validation" in self.results:
            syntax_success = sum(1 for r in self.results["syntax_validation"].values() if r.get("success", False))
            syntax_total = len(self.results["syntax_validation"])
            summary["phase_results"]["syntax_validation"] = {
                "success": syntax_success,
                "total": syntax_total,
                "pass_rate": f"{syntax_success/syntax_total*100:.1f}%" if syntax_total > 0 else "0%"
            }
        
        if "example_tests" in self.results:
            example_success = sum(1 for r in self.results["example_tests"].values() if r.get("success", False))
            example_total = len(self.results["example_tests"])
            summary["phase_results"]["example_tests"] = {
                "success": example_success,
                "total": example_total,
                "pass_rate": f"{example_success/example_total*100:.1f}%" if example_total > 0 else "0%"
            }
        
        self.results["summary"] = summary
        return self.results

    def calculate_overall_success(self):
        """Calculate overall success based on critical phases."""
        critical_phases = ["cmake_configuration", "build"]
        
        for phase in critical_phases:
            if phase in self.results and not self.results[phase].get("success", False):
                return False
        
        return True

    def print_detailed_report(self):
        """Print detailed build and test report."""
        if not self.results:
            self.log_message("No results available", "ERROR")
            return
        
        print("\n" + "="*80)
        print("ULTIMATE O-RAN 6G BUILD AND TEST REPORT")
        print("="*80)
        
        if "summary" in self.results:
            summary = self.results["summary"]
            print(f"\nOVERALL STATUS: {'✅ SUCCESS' if summary['overall_success'] else '❌ FAILED'}")
            print(f"Duration: {summary['duration_seconds']:.1f} seconds")
            print(f"Start Time: {summary['start_time']}")
            print(f"End Time: {summary['end_time']}")
        
        # Print phase results
        if "phase_results" in self.results.get("summary", {}):
            print(f"\n📋 PHASE RESULTS:")
            for phase, result in self.results["summary"]["phase_results"].items():
                if isinstance(result, dict):
                    if "pass_rate" in result:
                        print(f"   {phase}: {result['success']}/{result['total']} ({result['pass_rate']})")
                    else:
                        status = "✅" if result.get("success", False) else "❌"
                        print(f"   {phase}: {status}")
        
        # Detailed results
        if "prerequisites" in self.results:
            print(f"\n🔧 PREREQUISITES:")
            for tool, result in self.results["prerequisites"].items():
                status = "✅" if result.get("available", False) else "❌"
                version = f" ({result.get('version', 'Unknown')})" if result.get("available") else ""
                print(f"   {status} {tool}{version}")
        
        if "build" in self.results:
            build_result = self.results["build"]
            print(f"\n🔨 BUILD RESULTS:")
            status = "✅" if build_result.get("success", False) else "❌"
            print(f"   Status: {status}")
            if "parallel_jobs" in build_result:
                print(f"   Parallel Jobs: {build_result['parallel_jobs']}")
            if not build_result.get("success", False) and "error" in build_result:
                print(f"   Error: {build_result['error'][:200]}...")
        
        if "syntax_validation" in self.results:
            print(f"\n🔍 SYNTAX VALIDATION:")
            for module, result in self.results["syntax_validation"].items():
                status = "✅" if result.get("success", False) else "❌"
                print(f"   {status} {module}")
        
        if "example_tests" in self.results:
            print(f"\n📝 EXAMPLE TESTS:")
            for example, result in self.results["example_tests"].items():
                status = "✅" if result.get("success", False) else "❌"
                print(f"   {status} {example}")
        
        print("\n" + "="*80)

    def save_results_to_file(self, output_file="ultimate_build_test_results.json"):
        """Save build and test results to file."""
        if not self.results:
            self.log_message("No results to save", "ERROR")
            return
        
        output_path = self.workspace_path / output_file
        try:
            with open(output_path, 'w', encoding='utf-8') as f:
                json.dump(self.results, f, indent=2, default=str)
            
            self.log_message(f"Results saved to: {output_path}")
        except Exception as e:
            self.log_message(f"Error saving results: {str(e)}", "ERROR")

def main():
    """Main function."""
    if len(sys.argv) > 1:
        workspace_path = sys.argv[1]
    else:
        workspace_path = os.getcwd()
    
    if len(sys.argv) > 2:
        build_path = sys.argv[2]
    else:
        build_path = None
    
    builder = UltimateORANBuilder(workspace_path, build_path)
    
    # Run comprehensive build and test
    results = builder.run_comprehensive_build_and_test()
    
    # Print detailed report
    builder.print_detailed_report()
    
    # Save results
    builder.save_results_to_file()
    
    # Return appropriate exit code
    if results.get("summary", {}).get("overall_success", False):
        return 0
    else:
        return 1

if __name__ == "__main__":
    sys.exit(main())
