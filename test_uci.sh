#!/bin/bash

# Comprehensive UCI test script for native and WASM engines
echo "🚀 Testing Shatranj UCI interfaces..."
echo ""

# Test native engines
if [ -d "build" ]; then
    echo "=== Testing Native Engines ==="
    cd build/src
    
    # Test simple UCI engine
    if [ -f "shatranj_uci" ]; then
        echo "Testing native simple UCI engine..."
        {
            echo "uci"
            sleep 0.1
            echo "isready"
            sleep 0.1
            echo "ucinewgame"
            sleep 0.1
            echo "position startpos"
            sleep 0.1
            echo "go depth 3"
            sleep 2
            echo "quit"
        } | timeout 10s ./shatranj_uci
        echo "✓ Native simple UCI test completed."
    else
        echo "✗ shatranj_uci not found. Build first with ./build_uci.sh"
    fi
    
    echo ""
    
    # Test Enhanced Simple UCI engine
    if [ -f "shatranj_simple_uci" ]; then
        echo "Testing native enhanced UCI engine..."
        {
            echo "uci"
            sleep 0.1
            echo "isready"
            sleep 0.1
            echo "setoption name Hash value 32"
            sleep 0.1
            echo "ucinewgame"
            sleep 0.1
            echo "position startpos moves e2e3 e7e6"
            sleep 0.1
            echo "go depth 4"
            sleep 3
            echo "quit"
        } | timeout 15s ./shatranj_simple_uci
        echo "✓ Native enhanced UCI test completed."
    else
        echo "✗ shatranj_simple_uci not found. Build first with ./build_uci.sh"
    fi
    
    cd ../..
else
    echo "⚠️  Build directory not found. Run ./build_uci.sh to test native engines."
fi

echo ""
echo "=== Testing WASM Engines ==="

# Test WASM engines
if command -v node &> /dev/null; then
    if [ -f "test_wasm_uci_interactive.js" ]; then
        node test_wasm_uci_interactive.js
    else
        echo "✗ WASM test script not found"
    fi
else
    echo "⚠️  Node.js not found. Install Node.js to test WASM engines."
fi

echo ""
echo "🎉 All UCI tests completed!"