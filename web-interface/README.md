# Shatranj Web Interface

A complete web-based interface for playing Shatranj (ancient chess) with WebAssembly engines.

## Features

- **Interactive Board**: Click-to-move interface with visual feedback
- **Two Game Modes**:
  - **Playground**: Free exploration and move testing
  - **vs Computer**: Play against UCI WebAssembly engine
- **Move Validation**: Uses WebAssembly move generation
- **Undo/Redo**: Full move history support

## Usage

```bash
# Start web server
cd web-interface
python3 -m http.server 8080

# Open browser to http://localhost:8080
```

## Game Controls

- **Click piece** to select and see available moves
- **Click destination** to make move
- **Reset Board** to return to starting position
- **Undo Move** to take back last move
- **Get Hint** (vs Computer mode) for move suggestions

## Shatranj Pieces

- ♔/♚ Shah (King) - moves like chess king
- ♕/♛ Vizier (Queen) - moves 1 square diagonally only
- ♖/♜ Rook - moves like chess rook
- ♗/♝ Fil (Bishop) - jumps 2 squares diagonally
- ♘/♞ Horse (Knight) - moves like chess knight
- ♙/♟ Pawn - no initial 2-square move, no en passant

## Technical Details

- Uses `movedump.wasm` for move generation
- Uses `shatranj_simple_uci.wasm` for computer opponent
- Pure JavaScript interface with WebAssembly backend
- No external dependencies