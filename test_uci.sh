#!/bin/bash

# UCI test script for both engines
echo "Testing Shatranj UCI interfaces..."

# Build the UCI engines
if [ ! -d "build" ]; then
    echo "Build directory not found. Run ./build_uci.sh first."
    exit 1
fi

cd build/src

# Test simple UCI engine
if [ -f "shatranj_uci" ]; then
    echo "Testing simple UCI engine..."
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
    } | ./shatranj_uci
    echo "Simple UCI test completed."
else
    echo "shatranj_uci not found. Build first with ./build_uci.sh"
fi

echo ""

# Test Enhanced Simple UCI engine
if [ -f "shatranj_simple_uci" ]; then
    echo "Testing enhanced simple UCI engine..."
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
    } | ./shatranj_simple_uci
    echo "Enhanced simple UCI test completed."
else
    echo "shatranj_simple_uci not found. Build first with ./build_uci.sh"
fi

echo "All UCI tests completed."