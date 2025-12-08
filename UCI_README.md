# Shatranj Engine UCI Implementation

This repository now includes UCI (Universal Chess Interface) support for the Shatranj engine, making it compatible with chess GUIs and analysis tools.

## Build Instructions

```bash
# Native build
mkdir -p build
cd build
cmake ..
make shatranj_uci                # Basic UCI interface
make shatranj_simple_uci         # Enhanced UCI interface (Recommended)

# WebAssembly build
./build_uci_wasm.sh
```

## UCI Engines

### 1. Basic UCI (`shatranj_uci`)
- Basic UCI implementation using the original Shatranj search
- Minimal features, good for testing

### 2. Enhanced Simple UCI (`shatranj_simple_uci`) - **Recommended**
- Enhanced UCI with better protocol support
- Includes proper option handling and move parsing
- Stable and reliable for chess GUIs
- Uses optimized Shatranj search algorithms

## Supported UCI Commands

### Standard Commands
- `uci` - Initialize UCI mode
- `isready` - Check if engine is ready
- `ucinewgame` - Start new game
- `position startpos [moves ...]` - Set position
- `go [depth N] [movetime MS]` - Start search
- `stop` - Stop current search
- `quit` - Exit engine

### Options
- `Hash` - Hash table size in MB (1-1024, default 16)
- `Threads` - Number of search threads (currently 1)

## Usage Examples

### Command Line Testing
```bash
# Start the engine
./shatranj_stockfish_uci

# Basic UCI session
uci
isready
ucinewgame
position startpos
go depth 6
quit
```

### With Chess GUIs
The engines are compatible with any UCI-supporting chess GUI such as:
- Arena
- ChessBase
- Scid vs. PC
- Cute Chess
- BanksiaGUI

## Shatranj Rules Differences

The engine implements traditional Shatranj rules:
- **Ferz (F)**: Moves one square diagonally (like a weak Queen)
- **Alfil (Elephant, A)**: Jumps exactly two squares diagonally
- **Rukh (R)**: Moves like a Rook
- **Faras (H)**: Moves like a Knight
- **Baidaq (P)**: Moves like a Pawn but no initial two-square move
- **Shah (S)**: Moves like a King

## Testing

Use the provided test script:
```bash
./test_uci.sh
```

## Integration Notes

- The engine maintains both Shatranj and Stockfish position representations
- Move conversion between formats is handled automatically
- Fallback to simple search if Stockfish integration fails
- Supports standard algebraic notation (e.g., "e2e4")

## Performance

The Stockfish-integrated version provides significantly better:
- Search depth and speed
- Position evaluation
- Tactical awareness
- Endgame play

While maintaining the authentic Shatranj piece movement rules.