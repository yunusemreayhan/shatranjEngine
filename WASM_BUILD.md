# WebAssembly Build Instructions

## Prerequisites

Install Emscripten SDK:

```bash
# Clone the emsdk repository
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Install and activate the latest version
./emsdk install latest
./emsdk activate latest

# Activate PATH and environment variables
source ./emsdk_env.sh
```

## Building

From the shatranjEngine root directory:

```bash
./compile_wasm.sh
```

This will generate:
- `movedump.js` - JavaScript wrapper
- `movedump.wasm` - WebAssembly binary

## Usage

### In Browser
Open `test_movedump.html` in a web browser to test the WebAssembly version.

### In Node.js
```javascript
const MoveDump = require('./movedump.js');

MoveDump().then(module => {
    // Call with FEN string
    module.callMain(['rhfvshfr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSHFR w - - 0 1']);
});
```

## Usage Examples

### Node.js
```javascript
const MoveDump = require('./movedump.js');

MoveDump().then(module => {
    // Use FEN string only (no program name)
    module.callMain(['8/8/8/1s6/8/1SV5/8/r7 w - - 0 1']);
});
```

### Browser
```html
<script src="movedump.js"></script>
<script>
MoveDump().then(module => {
    module.callMain(['rhfvshfr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSHFR w - - 0 1']);
});
</script>
```

## Notes

- The WebAssembly version maintains the same functionality as the native binary
- Uses Shatranj piece notation (PHFRVS)
- Arguments: Pass only the FEN string in array format
- Stack size increased to 1MB for complex positions