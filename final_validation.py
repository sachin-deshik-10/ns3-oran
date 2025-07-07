#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Final Validation Script for Ultra-Advanced O-RAN 6G Platform
Complete system validation and readiness assessment
"""

import os
import sys
import subprocess
import json
import time
from pathlib import Path
from datetime import datetime

def print_banner():
    """Print validation banner"""
    print("=" * 80)
    print("ULTRA-ADVANCED O-RAN 6G PLATFORM - FINAL VALIDATION")
    print("=" * 80)
    print(f"Validation Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 80)

def validate_platform():
    """Complete platform validation"""
    print_banner()
    
    validation_results = {
        'timestamp': datetime.now().isoformat(),
        'platform_status': 'VALIDATING',
        'modules': {},
        'tests': {},
        'performance': {},
        'deployment_ready': False
    }
    
    # Phase 1: Module Validation
    print("\n[PHASE 1] Module Validation...")
    workspace = Path('.')
    model_dir = workspace / 'model'
    
    advanced_modules = [
        'oran-6g-neuromorphic-computing',
        'oran-6g-holographic',
        'oran-6g-brain-computer-interface',
        'oran-6g-cybersecurity',
        'oran-6g-semantic-communications',
        'oran-6g-ultra-ai-orchestrator',
        'oran-6g-sags-network',
        'oran-6g-terahertz',
        'oran-ai-transformer',
        'oran-6g-quantum-enhanced',
        'oran-6g-edge-ai'
    ]
    
    modules_complete = 0
    for module in advanced_modules:
        header_file = model_dir / f"{module}.h"
        source_file = model_dir / f"{module}.cc"
        
        if header_file.exists() and source_file.exists():
            modules_complete += 1
            print(f"  ✓ {module}: COMPLETE")
        else:
            print(f"  ✗ {module}: INCOMPLETE")
    
    validation_results['modules'] = {
        'total': len(advanced_modules),
        'complete': modules_complete,
        'status': 'COMPLETE' if modules_complete == len(advanced_modules) else 'INCOMPLETE'
    }
    
    # Phase 2: Examples Validation
    print("\n[PHASE 2] Examples Validation...")
    examples_dir = workspace / 'examples'
    
    advanced_examples = [
        'oran-6g-comprehensive-integration-demo',
        'oran-6g-ultimate-next-generation-example',
        'oran-6g-real-time-ai-orchestration-demo'
    ]
    
    examples_complete = 0
    for example in advanced_examples:
        example_file = examples_dir / f"{example}.cc"
        if example_file.exists():
            examples_complete += 1
            print(f"  ✓ {example}: PRESENT")
        else:
            print(f"  ✗ {example}: MISSING")
    
    validation_results['examples'] = {
        'total': len(advanced_examples),
        'complete': examples_complete,
        'status': 'COMPLETE' if examples_complete == len(advanced_examples) else 'INCOMPLETE'
    }
    
    # Phase 3: Build System Validation
    print("\n[PHASE 3] Build System Validation...")
    try:
        result = subprocess.run([
            sys.executable, 'simple_build_system.py', '.', '--no-tests'
        ], capture_output=True, text=True, timeout=60)
        
        if result.returncode == 0:
            print("  ✓ Build System: OPERATIONAL")
            validation_results['build_system'] = 'OPERATIONAL'
        else:
            print("  ✗ Build System: FAILED")
            validation_results['build_system'] = 'FAILED'
    except Exception as e:
        print(f"  ✗ Build System: ERROR - {e}")
        validation_results['build_system'] = 'ERROR'
    
    # Phase 4: Test System Validation
    print("\n[PHASE 4] Test System Validation...")
    try:
        result = subprocess.run([
            sys.executable, 'simple_comprehensive_test.py', '.'
        ], capture_output=True, text=True, timeout=120)
        
        if result.returncode == 0:
            print("  ✓ Test System: ALL TESTS PASSED")
            validation_results['test_system'] = 'ALL_TESTS_PASSED'
        else:
            print("  ✗ Test System: SOME TESTS FAILED")
            validation_results['test_system'] = 'TESTS_FAILED'
    except Exception as e:
        print(f"  ✗ Test System: ERROR - {e}")
        validation_results['test_system'] = 'ERROR'
    
    # Phase 5: Documentation Validation
    print("\n[PHASE 5] Documentation Validation...")
    docs_dir = workspace / 'docs'
    required_docs = [
        'API_REFERENCE.md',
        'ARCHITECTURE.md',
        'USER_GUIDE.md',
        'QUICK_SETUP.md'
    ]
    
    docs_present = 0
    for doc in required_docs:
        doc_file = docs_dir / doc
        if doc_file.exists():
            docs_present += 1
            print(f"  ✓ {doc}: PRESENT")
        else:
            print(f"  ✗ {doc}: MISSING")
    
    validation_results['documentation'] = {
        'total': len(required_docs),
        'present': docs_present,
        'status': 'COMPLETE' if docs_present == len(required_docs) else 'INCOMPLETE'
    }
    
    # Phase 6: Platform Readiness Assessment
    print("\n[PHASE 6] Platform Readiness Assessment...")
    
    readiness_score = 0
    max_score = 5
    
    # Module completeness
    if validation_results['modules']['status'] == 'COMPLETE':
        readiness_score += 1
        print("  ✓ All advanced modules implemented")
    else:
        print("  ✗ Some modules missing")
    
    # Example completeness
    if validation_results['examples']['status'] == 'COMPLETE':
        readiness_score += 1
        print("  ✓ All advanced examples present")
    else:
        print("  ✗ Some examples missing")
    
    # Build system
    if validation_results['build_system'] == 'OPERATIONAL':
        readiness_score += 1
        print("  ✓ Build system operational")
    else:
        print("  ✗ Build system issues")
    
    # Test system
    if validation_results['test_system'] == 'ALL_TESTS_PASSED':
        readiness_score += 1
        print("  ✓ All tests passing")
    else:
        print("  ✗ Test failures detected")
    
    # Documentation
    if validation_results['documentation']['status'] == 'COMPLETE':
        readiness_score += 1
        print("  ✓ Documentation complete")
    else:
        print("  ✗ Documentation incomplete")
    
    # Final Assessment
    print("\n" + "=" * 80)
    print("FINAL PLATFORM ASSESSMENT")
    print("=" * 80)
    
    readiness_percentage = (readiness_score / max_score) * 100
    validation_results['readiness_score'] = readiness_score
    validation_results['max_score'] = max_score
    validation_results['readiness_percentage'] = readiness_percentage
    
    if readiness_score == max_score:
        validation_results['platform_status'] = 'DEPLOYMENT_READY'
        validation_results['deployment_ready'] = True
        print("🎉 PLATFORM STATUS: DEPLOYMENT READY")
        print(f"✅ Readiness Score: {readiness_score}/{max_score} ({readiness_percentage:.1f}%)")
        print("✅ All systems operational and validated")
        print("✅ Ready for next-generation 6G research and deployment")
    else:
        validation_results['platform_status'] = 'NEEDS_ATTENTION'
        validation_results['deployment_ready'] = False
        print("⚠️  PLATFORM STATUS: NEEDS ATTENTION")
        print(f"⚠️  Readiness Score: {readiness_score}/{max_score} ({readiness_percentage:.1f}%)")
        print("⚠️  Some issues need to be addressed before deployment")
    
    # Save validation results
    validation_file = workspace / 'FINAL_VALIDATION_RESULTS.json'
    try:
        with open(validation_file, 'w', encoding='utf-8') as f:
            json.dump(validation_results, f, indent=2)
        print(f"\n📄 Validation results saved to: {validation_file}")
    except Exception as e:
        print(f"\n❌ Error saving validation results: {e}")
    
    print("\n" + "=" * 80)
    print("ULTRA-ADVANCED O-RAN 6G PLATFORM VALIDATION COMPLETE")
    print("=" * 80)
    
    return validation_results['deployment_ready']

if __name__ == '__main__':
    try:
        success = validate_platform()
        sys.exit(0 if success else 1)
    except KeyboardInterrupt:
        print("\n\nValidation interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"\n\nValidation failed with error: {e}")
        sys.exit(1)
