const ShatranjSimpleUCI = require('./shatranj_simple_uci.js');

async function testUCIInteractive() {
    try {
        console.log('Loading Shatranj Simple UCI WebAssembly module...');
        const module = await ShatranjSimpleUCI();
        console.log('✓ Module loaded successfully');
        
        // Test basic UCI protocol
        console.log('\n=== Testing UCI Protocol ===');
        
        // Start the UCI engine
        console.log('Starting UCI engine...');
        
        // Test with empty args (should start UCI mode)
        try {
            const result = module.callMain([]);
            console.log('Engine started with result:', result);
        } catch (error) {
            console.log('Engine execution:', error.message);
        }
        
        // Test with explicit UCI argument
        try {
            console.log('\nTesting with explicit UCI argument...');
            const result = module.callMain(['shatranj_simple_uci']);
            console.log('UCI mode result:', result);
        } catch (error) {
            console.log('UCI mode execution:', error.message);
        }
        
        console.log('\n✓ UCI engine test completed');
        console.log('\nNote: For full UCI testing, use an interactive UCI client or chess GUI');
        
    } catch (error) {
        console.error('✗ Error:', error.message);
    }
}

testUCIInteractive();