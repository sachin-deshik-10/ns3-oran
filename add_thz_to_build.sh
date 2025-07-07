#!/bin/bash
# 6G THz Module Quick Compilation Guide
# This script helps add the new THz module to the NS-3 build system

echo "🚀 Adding 6G THz Module to NS-3 Build System"
echo "=============================================="

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: Run this script from the ns3-oran-master directory"
    exit 1
fi

echo "📁 Current directory: $(pwd)"

# Backup existing CMakeLists.txt files
echo "💾 Backing up CMakeLists.txt files..."
cp model/CMakeLists.txt model/CMakeLists.txt.backup 2>/dev/null || echo "⚠️  model/CMakeLists.txt not found"
cp examples/CMakeLists.txt examples/CMakeLists.txt.backup 2>/dev/null || echo "⚠️  examples/CMakeLists.txt not found"

# Check if THz files exist
echo "🔍 Checking THz module files..."
if [ -f "model/oran-6g-terahertz.h" ] && [ -f "model/oran-6g-terahertz.cc" ]; then
    echo "✅ THz module files found"
else
    echo "❌ THz module files missing. Please ensure:"
    echo "   - model/oran-6g-terahertz.h exists"
    echo "   - model/oran-6g-terahertz.cc exists"
    exit 1
fi

# Check if example files exist
echo "🔍 Checking example files..."
if [ -f "examples/oran-6g-terahertz-example.cc" ]; then
    echo "✅ THz example found"
else
    echo "⚠️  THz example not found (optional)"
fi

if [ -f "examples/oran-6g-ai-integrated-example.cc" ]; then
    echo "✅ AI-integrated example found"
else
    echo "⚠️  AI-integrated example not found (optional)"
fi

# Manual instructions for CMakeLists.txt modification
echo ""
echo "🔧 MANUAL STEPS REQUIRED:"
echo "========================="
echo ""
echo "1. Add THz module to model/CMakeLists.txt:"
echo "   Add this line to the source files list:"
echo "   oran-6g-terahertz.cc"
echo ""
echo "2. Add examples to examples/CMakeLists.txt (if they exist):"
echo "   Add these lines to the examples list:"
echo "   oran-6g-terahertz-example.cc"
echo "   oran-6g-ai-integrated-example.cc"
echo ""
echo "3. Build the project:"
echo "   ./waf configure"
echo "   ./waf build"
echo ""
echo "4. Test the THz module:"
echo "   ./waf --run oran-6g-terahertz-example"
echo ""

# Create a simple test script
echo "📝 Creating test compilation script..."
cat > test_thz_compilation.sh << 'EOF'
#!/bin/bash
echo "🧪 Testing THz Module Compilation"
echo "=================================="

# Try to build
echo "🔨 Building project..."
if ./waf build; then
    echo "✅ Build successful!"
    
    # Try to run examples
    echo "🧪 Testing examples..."
    
    if [ -f "build/examples/ns3-dev-oran-6g-terahertz-example-debug" ] || [ -f "build/examples/oran-6g-terahertz-example" ]; then
        echo "✅ THz example compiled successfully"
        echo "🚀 Run with: ./waf --run oran-6g-terahertz-example"
    else
        echo "⚠️  THz example not found in build output"
    fi
    
    if [ -f "build/examples/ns3-dev-oran-6g-ai-integrated-example-debug" ] || [ -f "build/examples/oran-6g-ai-integrated-example" ]; then
        echo "✅ AI-integrated example compiled successfully"
        echo "🚀 Run with: ./waf --run oran-6g-ai-integrated-example"
    else
        echo "⚠️  AI-integrated example not found in build output"
    fi
    
else
    echo "❌ Build failed. Check the error messages above."
    echo "💡 Common issues:"
    echo "   - Missing dependencies"
    echo "   - Syntax errors in THz module"
    echo "   - CMakeLists.txt not properly updated"
fi
EOF

chmod +x test_thz_compilation.sh

echo "✅ Test script created: test_thz_compilation.sh"
echo ""
echo "🎯 NEXT STEPS:"
echo "1. Manually edit CMakeLists.txt files as shown above"
echo "2. Run: ./test_thz_compilation.sh"
echo "3. If successful, test with: ./waf --run oran-6g-terahertz-example"
echo ""
echo "📊 Expected THz module features:"
echo "   - 100+ GHz frequency operation"
echo "   - Atmospheric absorption modeling"
echo "   - Ultra-massive MIMO (2048+ antennas)"
echo "   - Intelligent reflecting surfaces"
echo "   - 100+ Gbps theoretical throughput"
