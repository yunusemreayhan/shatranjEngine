const ShatranjUCI = require('./shatranj_uci.js');
const { spawn } = require('child_process');

async function testUCI() {
    console.log('=== Interactive ShatranjUCI WASM Test ===\n');
    
    // Test commands
    const testCommands = [
        { cmd: 'uci', expect: ['id name', 'id author', 'uciok'] },
        { cmd: 'isready', expect: ['readyok'] },
        { cmd: 'ucinewgame', expect: [] },
        { cmd: 'position startpos', expect: [] },
        { cmd: 'go depth 3', expect: ['info depth', 'bestmove'] },
        { cmd: 'quit', expect: [] }
    ];
    
    console.log('Test sequence:');
    testCommands.forEach((t, i) => console.log(`  ${i + 1}. ${t.cmd}`));
    console.log('\n--- Starting Test ---\n');
    
    // Run node with the module
    const proc = spawn('node', ['-e', `
        const ShatranjUCI = require('./shatranj_uci.js');
        ShatranjUCI({
            print: (text) => console.log(text),
            printErr: (text) => console.error(text)
        });
    `], { cwd: __dirname });
    
    let currentTest = 0;
    let outputLines = [];
    
    proc.stdout.on('data', (data) => {
        const lines = data.toString().split('\n').filter(l => l.trim());
        lines.forEach(line => {
            console.log(`[ENGINE] ${line}`);
            outputLines.push(line);
            
            // Check expectations
            if (currentTest < testCommands.length) {
                const test = testCommands[currentTest];
                const allExpected = test.expect.every(exp => 
                    outputLines.some(l => l.includes(exp))
                );
                
                if (allExpected && test.expect.length > 0) {
                    console.log(`✓ Test ${currentTest + 1} passed: ${test.cmd}\n`);
                    currentTest++;
                    
                    // Send next command
                    if (currentTest < testCommands.length) {
                        const nextCmd = testCommands[currentTest].cmd;
                        console.log(`[SEND] ${nextCmd}`);
                        proc.stdin.write(nextCmd + '\n');
                        outputLines = [];
                    }
                }
            }
        });
    });
    
    proc.stderr.on('data', (data) => {
        console.error(`[ERROR] ${data}`);
    });
    
    proc.on('close', (code) => {
        console.log(`\n--- Engine exited with code ${code} ---`);
        console.log(`\nTests passed: ${currentTest}/${testCommands.length}`);
    });
    
    // Send first command
    setTimeout(() => {
        console.log(`[SEND] ${testCommands[0].cmd}`);
        proc.stdin.write(testCommands[0].cmd + '\n');
    }, 500);
    
    // Timeout
    setTimeout(() => {
        proc.kill();
        console.log('\n[TIMEOUT] Test terminated');
    }, 10000);
}

testUCI().catch(err => {
    console.error('Fatal error:', err);
    process.exit(1);
});
