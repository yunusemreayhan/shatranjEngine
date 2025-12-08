const MoveDump = require('./movedump.js');

MoveDump().then(module => {
    console.log('WebAssembly module loaded successfully');
    
    // Test with proper argument format
    try {
        const result = module.callMain(['movedump', 'rhfvshfr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSHFR w - - 0 1']);
        console.log('Result:', result);
    } catch (error) {
        console.error('Error:', error.message);
    }
}).catch(error => {
    console.error('Failed to load module:', error);
});