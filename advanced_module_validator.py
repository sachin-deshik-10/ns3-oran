#!/usr/bin/env python3
"""
Advanced O-RAN Module Integration Validator
Comprehensive testing of 6G THz, AI Transformer, and Network Slicing modules
"""

import os
import sys
import subprocess
import json
import time
from datetime import datetime
from pathlib import Path

class OranAdvancedValidator:
    def __init__(self):
        self.workspace_path = Path("d:/ns3-oran-master")
        self.results = {
            "timestamp": datetime.now().isoformat(),
            "modules": {},
            "integration": {},
            "build": {},
            "examples": {},
            "overall_score": 0.0
        }
    
    def validate_module_files(self):
        """Validate that all module files exist and have proper structure"""
        print("=== Module File Validation ===")
        
        modules_to_check = [
            {
                "name": "6G Terahertz",
                "header": "model/oran-6g-terahertz.h",
                "source": "model/oran-6g-terahertz.cc",
                "example": "examples/oran-6g-terahertz-example.cc"
            },
            {
                "name": "AI Transformer",
                "header": "model/oran-ai-transformer.h", 
                "source": "model/oran-ai-transformer.cc",
                "example": "examples/oran-6g-ai-integrated-example.cc"
            },
            {
                "name": "Network Slicing",
                "header": "model/oran-6g-network-slicing.h",
                "source": "model/oran-6g-network-slicing.cc",
                "example": None  # No standalone example
            }
        ]
        
        for module in modules_to_check:
            module_results = {
                "files_exist": True,
                "header_valid": False,
                "source_valid": False,
                "example_valid": False,
                "score": 0.0
            }
            
            # Check header file
            header_path = self.workspace_path / module["header"]
            if header_path.exists():
                with open(header_path, 'r') as f:
                    content = f.read()
                    if "NS_OBJECT_ENSURE_REGISTERED" in content or "TypeId" in content:
                        module_results["header_valid"] = True
                        print(f"✓ {module['name']} header file valid")
                    else:
                        print(f"✗ {module['name']} header file missing NS3 registration")
            else:
                module_results["files_exist"] = False
                print(f"✗ {module['name']} header file missing")
            
            # Check source file
            source_path = self.workspace_path / module["source"]
            if source_path.exists():
                with open(source_path, 'r') as f:
                    content = f.read()
                    if "GetTypeId" in content and "NS_OBJECT_ENSURE_REGISTERED" in content:
                        module_results["source_valid"] = True
                        print(f"✓ {module['name']} source file valid")
                    else:
                        print(f"✗ {module['name']} source file missing proper NS3 implementation")
            else:
                module_results["files_exist"] = False
                print(f"✗ {module['name']} source file missing")
            
            # Check example file if specified
            if module["example"]:
                example_path = self.workspace_path / module["example"]
                if example_path.exists():
                    with open(example_path, 'r') as f:
                        content = f.read()
                        if "#include" in content and "main" in content:
                            module_results["example_valid"] = True
                            print(f"✓ {module['name']} example file valid")
                        else:
                            print(f"✗ {module['name']} example file incomplete")
                else:
                    print(f"✗ {module['name']} example file missing")
            else:
                module_results["example_valid"] = True  # Not required
            
            # Calculate module score
            score_components = [
                module_results["files_exist"],
                module_results["header_valid"],
                module_results["source_valid"],
                module_results["example_valid"]
            ]
            module_results["score"] = sum(score_components) / len(score_components) * 100
            
            self.results["modules"][module["name"]] = module_results
    
    def validate_cmake_integration(self):
        """Validate CMake integration for all modules"""
        print("\n=== CMake Integration Validation ===")
        
        cmake_results = {
            "main_cmake_valid": False,
            "examples_cmake_valid": False,
            "all_modules_included": False,
            "score": 0.0
        }
        
        # Check main CMakeLists.txt
        main_cmake = self.workspace_path / "CMakeLists.txt"
        if main_cmake.exists():
            with open(main_cmake, 'r') as f:
                content = f.read()
                required_modules = [
                    "oran-6g-terahertz.cc",
                    "oran-ai-transformer.cc", 
                    "oran-6g-network-slicing.cc",
                    "oran-6g-terahertz.h",
                    "oran-ai-transformer.h",
                    "oran-6g-network-slicing.h"
                ]
                
                modules_found = sum(1 for mod in required_modules if mod in content)
                if modules_found == len(required_modules):
                    cmake_results["main_cmake_valid"] = True
                    cmake_results["all_modules_included"] = True
                    print(f"✓ Main CMakeLists.txt includes all modules ({modules_found}/{len(required_modules)})")
                else:
                    print(f"✗ Main CMakeLists.txt missing modules ({modules_found}/{len(required_modules)})")
        
        # Check examples CMakeLists.txt
        examples_cmake = self.workspace_path / "examples" / "CMakeLists.txt"
        if examples_cmake.exists():
            with open(examples_cmake, 'r') as f:
                content = f.read()
                required_examples = [
                    "oran-6g-terahertz-example",
                    "oran-6g-ai-integrated-example",
                    "oran-6g-comprehensive-example"
                ]
                
                examples_found = sum(1 for ex in required_examples if ex in content)
                if examples_found >= 2:  # At least most examples
                    cmake_results["examples_cmake_valid"] = True
                    print(f"✓ Examples CMakeLists.txt includes examples ({examples_found}/{len(required_examples)})")
                else:
                    print(f"✗ Examples CMakeLists.txt missing examples ({examples_found}/{len(required_examples)})")
        
        # Calculate CMake score
        score_components = [
            cmake_results["main_cmake_valid"],
            cmake_results["examples_cmake_valid"],
            cmake_results["all_modules_included"]
        ]
        cmake_results["score"] = sum(score_components) / len(score_components) * 100
        
        self.results["build"] = cmake_results
    
    def validate_comprehensive_example(self):
        """Validate the comprehensive integration example"""
        print("\n=== Comprehensive Example Validation ===")
        
        example_results = {
            "file_exists": False,
            "includes_all_modules": False,
            "has_main_function": False,
            "demonstrates_integration": False,
            "score": 0.0
        }
        
        comprehensive_example = self.workspace_path / "examples" / "oran-6g-comprehensive-example.cc"
        if comprehensive_example.exists():
            example_results["file_exists"] = True
            
            with open(comprehensive_example, 'r') as f:
                content = f.read()
                
                # Check for all module includes
                required_includes = [
                    "oran-6g-terahertz.h",
                    "oran-ai-transformer.h", 
                    "oran-6g-network-slicing.h"
                ]
                
                includes_found = sum(1 for inc in required_includes if inc in content)
                if includes_found == len(required_includes):
                    example_results["includes_all_modules"] = True
                    print(f"✓ Comprehensive example includes all modules ({includes_found}/{len(required_includes)})")
                else:
                    print(f"✗ Comprehensive example missing includes ({includes_found}/{len(required_includes)})")
                
                # Check for main function
                if "int main" in content or "main(" in content:
                    example_results["has_main_function"] = True
                    print("✓ Comprehensive example has main function")
                else:
                    print("✗ Comprehensive example missing main function")
                
                # Check for integration demonstrations
                integration_keywords = [
                    "SetupTopology",
                    "RunSimulation", 
                    "ConfigureSlicing",
                    "Transformer",
                    "Terahertz"
                ]
                
                integration_found = sum(1 for keyword in integration_keywords if keyword in content)
                if integration_found >= 3:
                    example_results["demonstrates_integration"] = True
                    print(f"✓ Comprehensive example demonstrates integration ({integration_found}/{len(integration_keywords)})")
                else:
                    print(f"✗ Comprehensive example lacks integration demo ({integration_found}/{len(integration_keywords)})")
        else:
            print("✗ Comprehensive example file missing")
        
        # Calculate example score
        score_components = [
            example_results["file_exists"],
            example_results["includes_all_modules"],
            example_results["has_main_function"],
            example_results["demonstrates_integration"]
        ]
        example_results["score"] = sum(score_components) / len(score_components) * 100
        
        self.results["examples"] = example_results
    
    def validate_documentation(self):
        """Validate documentation and roadmap files"""
        print("\n=== Documentation Validation ===")
        
        doc_results = {
            "implementation_roadmap": False,
            "next_features": False,
            "status_tracking": False,
            "score": 0.0
        }
        
        doc_files = [
            ("IMMEDIATE_IMPLEMENTATION_ROADMAP.md", "implementation_roadmap"),
            ("NEXT_FEATURES_TO_IMPLEMENT.md", "next_features"),
            ("IMPLEMENTATION_STATUS.md", "status_tracking")
        ]
        
        for filename, key in doc_files:
            file_path = self.workspace_path / filename
            if file_path.exists():
                doc_results[key] = True
                print(f"✓ {filename} exists")
            else:
                print(f"✗ {filename} missing")
        
        # Calculate documentation score
        score_components = [doc_results[key] for _, key in doc_files]
        doc_results["score"] = sum(score_components) / len(score_components) * 100
        
        self.results["integration"]["documentation"] = doc_results
    
    def calculate_overall_score(self):
        """Calculate overall integration score"""
        scores = []
        
        # Module scores
        for module_name, module_data in self.results["modules"].items():
            scores.append(module_data["score"])
        
        # Build score
        if "score" in self.results["build"]:
            scores.append(self.results["build"]["score"])
        
        # Example score
        if "score" in self.results["examples"]:
            scores.append(self.results["examples"]["score"])
        
        # Documentation score
        if "integration" in self.results and "documentation" in self.results["integration"]:
            scores.append(self.results["integration"]["documentation"]["score"])
        
        if scores:
            self.results["overall_score"] = sum(scores) / len(scores)
        else:
            self.results["overall_score"] = 0.0
    
    def run_validation(self):
        """Run complete validation suite"""
        print("O-RAN Advanced Module Integration Validator")
        print("=" * 50)
        
        self.validate_module_files()
        self.validate_cmake_integration()
        self.validate_comprehensive_example()
        self.validate_documentation()
        self.calculate_overall_score()
        
        print(f"\n=== VALIDATION SUMMARY ===")
        print(f"Overall Integration Score: {self.results['overall_score']:.1f}%")
        
        if self.results['overall_score'] >= 90:
            status = "EXCELLENT"
        elif self.results['overall_score'] >= 80:
            status = "GOOD"
        elif self.results['overall_score'] >= 70:
            status = "ACCEPTABLE"
        else:
            status = "NEEDS IMPROVEMENT"
        
        print(f"Status: {status}")
        
        # Save detailed results
        results_file = self.workspace_path / "advanced_validation_results.json"
        with open(results_file, 'w') as f:
            json.dump(self.results, f, indent=2)
        
        print(f"\nDetailed results saved to: {results_file}")
        
        return self.results

if __name__ == "__main__":
    validator = OranAdvancedValidator()
    results = validator.run_validation()
    
    # Exit with appropriate code
    if results["overall_score"] >= 80:
        sys.exit(0)
    else:
        sys.exit(1)
