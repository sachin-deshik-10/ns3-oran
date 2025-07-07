#!/usr/bin/env python3
"""
Ultimate Next-Generation O-RAN 6G Module Validator
Comprehensive validation for all ultra-advanced modules including SAGS, 
semantic communications, brain-computer interface, and neuromorphic computing.
"""

import os
import sys
import subprocess
import json
import time
from pathlib import Path
from datetime import datetime

class UltimateORANValidator:
    def __init__(self, workspace_path):
        self.workspace_path = Path(workspace_path)
        self.validation_results = {}
        self.validation_start_time = datetime.now()
        
        # Define all ultra-advanced modules to validate
        self.ultra_advanced_modules = {
            'sags_network': {
                'header': 'model/oran-6g-sags-network.h',
                'implementation': 'model/oran-6g-sags-network.cc',
                'description': 'Space-Air-Ground-Sea Network Integration'
            },
            'semantic_communications': {
                'header': 'model/oran-6g-semantic-communications.h',
                'implementation': None,  # Header-only
                'description': 'Semantic and Intent-driven Communications'
            },
            'brain_computer_interface': {
                'header': 'model/oran-6g-brain-computer-interface.h',
                'implementation': None,  # Header-only
                'description': 'Brain-Computer Interface Integration'
            },
            'neuromorphic_computing': {
                'header': 'model/oran-6g-neuromorphic-computing.h',
                'implementation': None,  # Header-only
                'description': 'Neuromorphic Computing Platform'
            },
            'quantum_enhanced': {
                'header': 'model/oran-6g-quantum-enhanced.h',
                'implementation': 'model/oran-6g-quantum-enhanced.cc',
                'description': 'Quantum-Enhanced Communications'
            },
            'edge_ai': {
                'header': 'model/oran-6g-edge-ai.h',
                'implementation': 'model/oran-6g-edge-ai.cc',
                'description': 'Advanced AI-ML Edge Computing'
            },
            'holographic': {
                'header': 'model/oran-6g-holographic.h',
                'implementation': None,  # Header-only
                'description': 'Holographic Data Transmission'
            },
            'performance_optimizer': {
                'header': 'model/oran-6g-performance-optimizer.h',
                'implementation': None,  # Header-only
                'description': 'GPU-Accelerated Performance Optimizer'
            },
            'industrial_iot': {
                'header': 'model/oran-6g-industrial-iot.h',
                'implementation': None,  # Header-only
                'description': 'Industrial IoT and Sustainability'
            },
            'metaverse': {
                'header': 'model/oran-6g-metaverse.h',
                'implementation': None,  # Header-only
                'description': '6G Metaverse Integration'
            },
            'cybersecurity': {
                'header': 'model/oran-6g-cybersecurity.h',
                'implementation': None,  # Header-only
                'description': 'Advanced Cybersecurity & Zero Trust'
            }
        }
        
        # Define comprehensive examples to validate
        self.comprehensive_examples = [
            'examples/oran-6g-comprehensive-advanced-example.cc',
            'examples/oran-6g-next-generation-advanced-example.cc',
            'examples/oran-6g-ultimate-next-generation-example.cc'
        ]

    def log_message(self, message, level="INFO"):
        """Log messages with timestamp and level."""
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] [{level}] {message}")

    def validate_file_exists(self, file_path):
        """Validate that a file exists and is readable."""
        full_path = self.workspace_path / file_path
        return full_path.exists() and full_path.is_file()

    def validate_header_file(self, file_path):
        """Validate C++ header file structure and content."""
        full_path = self.workspace_path / file_path
        
        if not self.validate_file_exists(file_path):
            return False, "File does not exist"
        
        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Check for essential header elements
            checks = {
                'header_guard': '#ifndef' in content and '#define' in content and '#endif' in content,
                'includes': '#include' in content,
                'namespace': 'namespace ns3' in content,
                'class_definition': 'class' in content,
                'license_header': 'NIST' in content or 'License' in content or 'Copyright' in content
            }
            
            passed_checks = sum(checks.values())
            total_checks = len(checks)
            
            return passed_checks >= total_checks - 1, f"Passed {passed_checks}/{total_checks} header validation checks"
            
        except Exception as e:
            return False, f"Error reading file: {str(e)}"

    def validate_implementation_file(self, file_path):
        """Validate C++ implementation file structure and content."""
        full_path = self.workspace_path / file_path
        
        if not self.validate_file_exists(file_path):
            return False, "File does not exist"
        
        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Check for essential implementation elements
            checks = {
                'includes': '#include' in content,
                'namespace': 'namespace ns3' in content,
                'constructor': '::' in content,  # Class member implementation
                'license_header': 'NIST' in content or 'License' in content or 'Copyright' in content
            }
            
            passed_checks = sum(checks.values())
            total_checks = len(checks)
            
            return passed_checks >= total_checks - 1, f"Passed {passed_checks}/{total_checks} implementation validation checks"
            
        except Exception as e:
            return False, f"Error reading file: {str(e)}"

    def validate_build_integration(self):
        """Validate that modules are properly integrated into build system."""
        cmake_file = self.workspace_path / 'CMakeLists.txt'
        examples_cmake_file = self.workspace_path / 'examples' / 'CMakeLists.txt'
        
        results = {}
        
        # Check main CMakeLists.txt
        if cmake_file.exists():
            with open(cmake_file, 'r', encoding='utf-8') as f:
                cmake_content = f.read()
            
            # Check for module integration
            module_checks = {
                'sags_network': 'oran-6g-sags-network' in cmake_content,
                'quantum_enhanced': 'oran-6g-quantum-enhanced' in cmake_content,
                'edge_ai': 'oran-6g-edge-ai' in cmake_content,
                'semantic_comms': 'oran-6g-semantic-communications' in cmake_content,
                'bci': 'oran-6g-brain-computer-interface' in cmake_content,
                'neuromorphic': 'oran-6g-neuromorphic-computing' in cmake_content
            }
            
            results['main_cmake'] = {
                'passed': sum(module_checks.values()),
                'total': len(module_checks),
                'details': module_checks
            }
        
        # Check examples CMakeLists.txt
        if examples_cmake_file.exists():
            with open(examples_cmake_file, 'r', encoding='utf-8') as f:
                examples_cmake_content = f.read()
            
            # Check for example integration
            example_checks = {
                'comprehensive_advanced': 'oran-6g-comprehensive-advanced-example' in examples_cmake_content,
                'next_generation': 'oran-6g-next-generation-advanced-example' in examples_cmake_content,
                'ultimate_next_gen': 'oran-6g-ultimate-next-generation-example' in examples_cmake_content
            }
            
            results['examples_cmake'] = {
                'passed': sum(example_checks.values()),
                'total': len(example_checks),
                'details': example_checks
            }
        
        return results

    def validate_example_files(self):
        """Validate comprehensive example files."""
        results = {}
        
        for example_file in self.comprehensive_examples:
            file_name = os.path.basename(example_file)
            
            if self.validate_file_exists(example_file):
                try:
                    full_path = self.workspace_path / example_file
                    with open(full_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                    
                    # Check for comprehensive example elements
                    checks = {
                        'includes': '#include' in content,
                        'main_function': 'int main' in content,
                        'namespace': 'using namespace ns3' in content,
                        'simulation_setup': 'Simulator::' in content,
                        'advanced_features': any(module in content for module in ['SAGS', 'Semantic', 'BCI', 'Neuromorphic', 'Quantum']),
                        'comprehensive_scenario': len(content) > 10000  # Substantial implementation
                    }
                    
                    results[file_name] = {
                        'exists': True,
                        'passed': sum(checks.values()),
                        'total': len(checks),
                        'details': checks,
                        'size': len(content)
                    }
                    
                except Exception as e:
                    results[file_name] = {
                        'exists': True,
                        'error': str(e)
                    }
            else:
                results[file_name] = {'exists': False}
        
        return results

    def run_syntax_validation(self):
        """Run basic C++ syntax validation using compiler checks."""
        results = {}
        
        # Check if we can find a C++ compiler
        try:
            result = subprocess.run(['g++', '--version'], 
                                  capture_output=True, text=True, timeout=10)
            compiler_available = result.returncode == 0
        except:
            compiler_available = False
        
        if not compiler_available:
            try:
                result = subprocess.run(['clang++', '--version'], 
                                      capture_output=True, text=True, timeout=10)
                compiler_available = result.returncode == 0
                compiler_cmd = 'clang++'
            except:
                compiler_available = False
        else:
            compiler_cmd = 'g++'
        
        results['compiler_available'] = compiler_available
        
        if compiler_available:
            # Test compile some header files
            header_files = [
                'model/oran-6g-sags-network.h',
                'model/oran-6g-semantic-communications.h',
                'model/oran-6g-brain-computer-interface.h',
                'model/oran-6g-neuromorphic-computing.h'
            ]
            
            compile_results = {}
            for header in header_files:
                if self.validate_file_exists(header):
                    try:
                        # Create a simple test file
                        test_content = f'#include "{header}"\nint main() {{ return 0; }}'
                        test_file = self.workspace_path / 'temp_test.cc'
                        
                        with open(test_file, 'w') as f:
                            f.write(test_content)
                        
                        # Try to compile
                        result = subprocess.run([
                            compiler_cmd, '-c', '-I.', str(test_file)
                        ], capture_output=True, text=True, timeout=30, 
                           cwd=self.workspace_path)
                        
                        compile_results[header] = {
                            'success': result.returncode == 0,
                            'errors': result.stderr if result.returncode != 0 else None
                        }
                        
                        # Clean up
                        if test_file.exists():
                            test_file.unlink()
                        
                    except Exception as e:
                        compile_results[header] = {
                            'success': False,
                            'error': str(e)
                        }
            
            results['compile_tests'] = compile_results
        
        return results

    def validate_module(self, module_name, module_info):
        """Validate a single ultra-advanced module."""
        self.log_message(f"Validating {module_name}: {module_info['description']}")
        
        result = {
            'name': module_name,
            'description': module_info['description'],
            'header_validation': None,
            'implementation_validation': None,
            'overall_status': 'UNKNOWN'
        }
        
        # Validate header file
        header_valid, header_msg = self.validate_header_file(module_info['header'])
        result['header_validation'] = {
            'valid': header_valid,
            'message': header_msg,
            'file': module_info['header']
        }
        
        # Validate implementation file if exists
        if module_info['implementation']:
            impl_valid, impl_msg = self.validate_implementation_file(module_info['implementation'])
            result['implementation_validation'] = {
                'valid': impl_valid,
                'message': impl_msg,
                'file': module_info['implementation']
            }
        else:
            result['implementation_validation'] = {
                'valid': True,
                'message': 'Header-only module',
                'file': None
            }
        
        # Determine overall status
        if result['header_validation']['valid'] and result['implementation_validation']['valid']:
            result['overall_status'] = 'PASS'
        else:
            result['overall_status'] = 'FAIL'
        
        return result

    def run_comprehensive_validation(self):
        """Run comprehensive validation of all ultra-advanced modules."""
        self.log_message("Starting Ultimate Next-Generation O-RAN 6G Module Validation")
        self.log_message(f"Workspace: {self.workspace_path}")
        
        # Validate all ultra-advanced modules
        module_results = {}
        for module_name, module_info in self.ultra_advanced_modules.items():
            module_results[module_name] = self.validate_module(module_name, module_info)
        
        # Validate build integration
        self.log_message("Validating build system integration")
        build_results = self.validate_build_integration()
        
        # Validate example files
        self.log_message("Validating comprehensive example files")
        example_results = self.validate_example_files()
        
        # Run syntax validation
        self.log_message("Running syntax validation")
        syntax_results = self.run_syntax_validation()
        
        # Compile final results
        self.validation_results = {
            'validation_info': {
                'timestamp': self.validation_start_time.isoformat(),
                'workspace': str(self.workspace_path),
                'validator_version': '1.0.0-ultimate'
            },
            'module_validation': module_results,
            'build_integration': build_results,
            'example_validation': example_results,
            'syntax_validation': syntax_results,
            'summary': self.generate_summary(module_results, build_results, example_results, syntax_results)
        }
        
        return self.validation_results

    def generate_summary(self, module_results, build_results, example_results, syntax_results):
        """Generate comprehensive validation summary."""
        total_modules = len(module_results)
        passed_modules = sum(1 for result in module_results.values() if result['overall_status'] == 'PASS')
        
        total_examples = len(example_results)
        passed_examples = sum(1 for result in example_results.values() if result.get('exists', False))
        
        build_integration_score = 0
        build_integration_total = 0
        for category, results in build_results.items():
            if isinstance(results, dict) and 'passed' in results:
                build_integration_score += results['passed']
                build_integration_total += results['total']
        
        summary = {
            'overall_status': 'PASS' if passed_modules == total_modules else 'PARTIAL' if passed_modules > 0 else 'FAIL',
            'module_validation': {
                'total': total_modules,
                'passed': passed_modules,
                'pass_rate': f"{(passed_modules/total_modules*100):.1f}%" if total_modules > 0 else "0%"
            },
            'build_integration': {
                'score': build_integration_score,
                'total': build_integration_total,
                'pass_rate': f"{(build_integration_score/build_integration_total*100):.1f}%" if build_integration_total > 0 else "0%"
            },
            'example_validation': {
                'total': total_examples,
                'existing': passed_examples,
                'completion_rate': f"{(passed_examples/total_examples*100):.1f}%" if total_examples > 0 else "0%"
            },
            'syntax_validation': {
                'compiler_available': syntax_results.get('compiler_available', False),
                'compile_tests': len(syntax_results.get('compile_tests', {}))
            }
        }
        
        return summary

    def print_detailed_report(self):
        """Print detailed validation report."""
        if not self.validation_results:
            self.log_message("No validation results available. Run validation first.", "ERROR")
            return
        
        print("\n" + "="*80)
        print("ULTIMATE NEXT-GENERATION O-RAN 6G VALIDATION REPORT")
        print("="*80)
        
        # Print summary
        summary = self.validation_results['summary']
        print(f"\nOVERALL STATUS: {summary['overall_status']}")
        print(f"Validation Time: {self.validation_results['validation_info']['timestamp']}")
        
        # Module validation results
        print(f"\n📡 MODULE VALIDATION:")
        print(f"   Modules Validated: {summary['module_validation']['total']}")
        print(f"   Modules Passed: {summary['module_validation']['passed']}")
        print(f"   Pass Rate: {summary['module_validation']['pass_rate']}")
        
        # Detailed module results
        for module_name, result in self.validation_results['module_validation'].items():
            status_icon = "✅" if result['overall_status'] == 'PASS' else "❌"
            print(f"   {status_icon} {module_name}: {result['description']}")
            print(f"      Header: {result['header_validation']['message']}")
            if result['implementation_validation']['file']:
                print(f"      Implementation: {result['implementation_validation']['message']}")
        
        # Build integration results
        print(f"\n🔨 BUILD INTEGRATION:")
        build_results = self.validation_results['build_integration']
        print(f"   Integration Score: {summary['build_integration']['score']}/{summary['build_integration']['total']}")
        print(f"   Pass Rate: {summary['build_integration']['pass_rate']}")
        
        # Example validation results
        print(f"\n📝 EXAMPLE VALIDATION:")
        print(f"   Examples Found: {summary['example_validation']['existing']}/{summary['example_validation']['total']}")
        print(f"   Completion Rate: {summary['example_validation']['completion_rate']}")
        
        for example_name, result in self.validation_results['example_validation'].items():
            if result.get('exists', False):
                checks_passed = result.get('passed', 0)
                checks_total = result.get('total', 0)
                print(f"   ✅ {example_name}: {checks_passed}/{checks_total} checks passed ({result.get('size', 0)} characters)")
            else:
                print(f"   ❌ {example_name}: Not found")
        
        # Syntax validation results
        print(f"\n🔍 SYNTAX VALIDATION:")
        syntax = self.validation_results['syntax_validation']
        compiler_status = "✅" if syntax.get('compiler_available', False) else "❌"
        print(f"   Compiler Available: {compiler_status}")
        
        if 'compile_tests' in syntax:
            print(f"   Compile Tests Run: {len(syntax['compile_tests'])}")
            for header, result in syntax['compile_tests'].items():
                status = "✅" if result.get('success', False) else "❌"
                print(f"   {status} {header}")
        
        print("\n" + "="*80)

    def save_results_to_file(self, output_file="ultimate_validation_results.json"):
        """Save validation results to JSON file."""
        if not self.validation_results:
            self.log_message("No validation results to save.", "ERROR")
            return
        
        output_path = self.workspace_path / output_file
        try:
            with open(output_path, 'w', encoding='utf-8') as f:
                json.dump(self.validation_results, f, indent=2, default=str)
            
            self.log_message(f"Validation results saved to: {output_path}")
        except Exception as e:
            self.log_message(f"Error saving results: {str(e)}", "ERROR")

def main():
    """Main function to run the ultimate validation."""
    if len(sys.argv) > 1:
        workspace_path = sys.argv[1]
    else:
        workspace_path = os.getcwd()
    
    validator = UltimateORANValidator(workspace_path)
    
    # Run comprehensive validation
    validator.run_comprehensive_validation()
    
    # Print detailed report
    validator.print_detailed_report()
    
    # Save results
    validator.save_results_to_file()
    
    # Return appropriate exit code
    summary = validator.validation_results['summary']
    if summary['overall_status'] == 'PASS':
        return 0
    elif summary['overall_status'] == 'PARTIAL':
        return 1
    else:
        return 2

if __name__ == "__main__":
    sys.exit(main())
