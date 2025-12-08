const fs = require('fs');
const MoveDump = require('./movedump.js');

MoveDump().then(module => {
    console.log('Module loaded');
    
    // Redirect stdout to capture output
    let output = '';
    const originalWrite = process.stdout.write;
    process.stdout.write = function(string) {
        output += string;
        return true;
    };
    
    try {
        module.callMain(['movedump', '8/8/8/1s6/8/1SV5/8/r7 w - - 0 1']);
        process.stdout.write = originalWrite;
        console.log('Captured output:', output);
    } catch (e) {
        process.stdout.write = originalWrite;
        console.log('Error but module exists:', e.message.substring(0, 50));
    }
}).catch(e => console.log('Failed to load:', e.message));