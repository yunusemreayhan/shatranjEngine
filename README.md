# Shatranj Engine

A chess engine implementation for Shatranj (ancient chess variant) with modern Stockfish-based search algorithms.

## Features

- **Authentic Shatranj Rules**: Implements traditional piece movements (Ferz, Alfil, etc.)
- **UCI Protocol Support**: Compatible with chess GUIs and analysis tools
- **Stockfish Integration**: Advanced search algorithms adapted for Shatranj
- **Dual Engine Modes**: Simple and advanced UCI implementations

## Quick Start

### Build UCI Engines
```bash
./build_uci.sh          # Native binaries
./build_uci_wasm.sh     # WebAssembly
```

### Test UCI Interface
```bash
./test_uci.sh
```

### Use with Chess GUI
Point your UCI-compatible chess GUI to:
- `build/src/shatranj_uci` (Basic version)
- `build/src/shatranj_simple_uci` (Enhanced version - Recommended)

## Documentation

- [UCI Implementation Guide](UCI_README.md)
- [Build Instructions](WASM_BUILD.md)

## Development Setup

### For hooks dir activation

```bash
git config --local core.hooksPath .githooks/
```

## Shatranj Rules

Shatranj differs from modern chess in piece movements:
- **Ferz (Queen)**: Moves one square diagonally
- **Alfil (Bishop)**: Jumps exactly two squares diagonally
- **Pawns**: No initial two-square move
- **No castling or en passant**

## Architecture

- `src/lib/shatranj_simple/`: Core Shatranj game logic
- `src/lib/stockfish/`: Adapted Stockfish search algorithms
- `src/lib/shatranj_simple/uci.cpp`: Simple UCI implementation
- `src/lib/shatranj_simple/stockfish_uci.cpp`: Advanced UCI with Stockfish