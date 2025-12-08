const MoveDump = require('./movedump.js');

MoveDump().then(module => {
    console.log('Testing argument passing...');
    
    // Test with different argument formats
    const tests = [
        ['8/8/8/1s6/8/1SV5/8/r7 w - - 0 1'],
        ['movedump', '8/8/8/1s6/8/1SV5/8/r7 w - - 0 1'],
        ['./movedump', '8/8/8/1s6/8/1SV5/8/r7 w - - 0 1']
    ];
    
    for (let i = 0; i < tests.length; i++) {
        try {
            console.log(`Test ${i+1}: args =`, tests[i]);
            module.callMain(tests[i]);
            console.log('Success!');
            break;
        } catch (e) {
            console.log(`Test ${i+1} failed:`, e.message.substring(0, 50));
        }
    }
}).catch(e => console.log('Load failed:', e));