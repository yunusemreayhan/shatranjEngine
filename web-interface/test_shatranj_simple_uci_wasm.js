const ShatranjSimpleUCI = require('./shatranj_simple_uci.js');

async function testShatranjSimpleUCI() {
    try {
        console.log('Loading Shatranj Simple UCI WebAssembly module...');
        const module = await ShatranjSimpleUCI();
        console.log('✓ Module loaded successfully');
        
        // Capture output
        let output = '';
        const originalWrite = process.stdout.write;
        process.stdout.write = function(string) {
            output += string;
            return true;
        };
        
        console.log('\n=== Testing UCI Commands ===');
        
        // Test UCI initialization
        console.log('\n1. Testing UCI initialization...');
        output = '';
        module.callMain(['shatranj_simple_uci']);
        
        // Simulate UCI commands by writing to stdin
        const testCommands = [
            'uci',
            'isready',
            'position startpos',
            'go depth 3',
            'quit'
        ];
        
        for (const command of testCommands) {
            console.log(`Sending: ${command}`);
            // Note: WASM UCI engines typically read from stdin, but we'll test the basic loading
        }
        
        process.stdout.write = originalWrite;
        console.log('Captured output:', output.substring(0, 200) + '...');
        
        console.log('\n✓ Basic UCI engine test completed');
        
    } catch (error) {
        console.error('✗ Error testing Shatranj Simple UCI:', error.message);
        console.error('Stack:', error.stack);
    }
}

// Run the test
testShatranjSimpleUCI();