const ShatranjUCI = require('./shatranj_uci.js');
const readline = require('readline');

async function testUCI() {
    console.log('=== ShatranjUCI WASM Test ===\n');
    
    let outputBuffer = [];
    
    const module = await ShatranjUCI({
        print: (text) => {
            console.log('[OUT]:', text);
            outputBuffer.push(text);
        },
        printErr: (text) => {
            console.error('[ERR]:', text);
        },
        noInitialRun: false
    });
    
    console.log('\n=== Module loaded and running ===');
    console.log('Note: The engine is now waiting for UCI commands via stdin');
    console.log('Output captured:', outputBuffer.length, 'lines\n');
}

testUCI().catch(err => {
    console.error('Fatal error:', err);
    process.exit(1);
});
