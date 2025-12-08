# MoveDump Binary

A utility binary for dumping all available legal moves for a given board position in Shatranj (ancient chess).

## Usage

```bash
./movedump "<fen>"
```

## Example

```bash
./movedump "rhfvshfr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSHFR w - - 0 1"
```

## Shatranj Piece Notation

This binary uses Shatranj piece notation:
- `P/p` - Pawn (Baidaq)
- `H/h` - Horse (Faras) - moves like a Knight
- `F/f` - Elephant (Fil) - moves diagonally 2 squares, can jump
- `R/r` - Rook (Rukh) - moves like a Rook
- `V/v` - Vizier (Firzan) - moves diagonally 1 square only
- `S/s` - Shah (King) - moves like a King

Uppercase letters represent White pieces, lowercase represent Black pieces.

## Output Format

The binary outputs:
1. The input FEN string
2. Total number of available moves
3. List of all legal moves in algebraic notation (from-to format)

Example output:
```
Available moves for FEN: rhfvshfr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSHFR w - - 0 1
Total moves: 15
a2-a3
b2-b3
...
```

## WebAssembly Version

A WebAssembly version is also available for browser/Node.js usage:

```bash
# Build WASM version
./compile_wasm.sh

# Test in Node.js
node -e "require('./movedump.js')().then(m => m.callMain(['8/8/8/1s6/8/1SV5/8/r7 w - - 0 1']))"
```

See `WASM_BUILD.md` for complete WebAssembly build instructions.