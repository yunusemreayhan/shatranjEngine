#!/bin/bash

# Direct compilation script for WebAssembly version of movedump
# Requires Emscripten SDK to be installed and activated

if ! command -v emcc &> /dev/null; then
    echo "Emscripten not found. Install with:"
    echo "git clone https://github.com/emscripten-core/emsdk.git"
    echo "cd emsdk && ./emsdk install latest && ./emsdk activate latest"
    echo "source ./emsdk_env.sh"
    exit 1
fi

echo "Compiling movedump to WebAssembly..."

# Collect all source files
SOURCES="src/bin/movedump/main.cpp"
SOURCES="$SOURCES src/lib/shatranj_simple/board.cpp"
SOURCES="$SOURCES src/lib/shatranj_simple/history.cpp"
SOURCES="$SOURCES src/lib/shatranj_simple/piece_group.cpp"
SOURCES="$SOURCES src/lib/shatranj_simple/player.cpp"
SOURCES="$SOURCES src/lib/shatranj_simple/position.cpp"
SOURCES="$SOURCES src/lib/shatranj_simple/shatranc_piece.cpp"
SOURCES="$SOURCES src/lib/shatranj_simple/shatranj.cpp"
SOURCES="$SOURCES src/lib/stockfish/bitboard.cpp"
SOURCES="$SOURCES src/lib/stockfish/memory.cpp"
SOURCES="$SOURCES src/lib/stockfish/misc.cpp"
SOURCES="$SOURCES src/lib/stockfish/movegen.cpp"
SOURCES="$SOURCES src/lib/stockfish/stockfish_position.cpp"
SOURCES="$SOURCES src/lib/stockfish/tt.cpp"
SOURCES="$SOURCES src/lib/stockfish/custom/custom_search.cpp"
SOURCES="$SOURCES src/lib/stockfish/custom/custommovepicker.cpp"
SOURCES="$SOURCES src/lib/stockfish/custom/evaluate.cpp"
SOURCES="$SOURCES src/lib/stockfish/custom/game_over_check.cpp"
SOURCES="$SOURCES src/lib/stockfish/custom/perft.cpp"
SOURCES="$SOURCES src/lib/stockfish/custom/pesto_evaluate.cpp"

# Compile with emscripten
emcc $SOURCES \
    -I src/lib/shatranj_simple \
    -I src/lib/stockfish \
    -I src/lib/stockfish/custom \
    -std=c++20 \
    -O3 \
    -DUSE_POPCNT \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_main"]' \
    -s EXPORTED_RUNTIME_METHODS='["callMain"]' \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="MoveDump" \
    -s ASSERTIONS=1 \
    -s STACK_SIZE=1MB \
    -o movedump.js

echo "WebAssembly compilation complete!"
echo "Generated files: movedump.js, movedump.wasm"