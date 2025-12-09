const ShatranjUCI = require('./shatranj_uci.js');

async function testUCI() {
    console.log('=== Loading ShatranjUCI WASM Module ===\n');
    
    const module = await ShatranjUCI({
        print: (text) => console.log('[ENGINE OUTPUT]:', text),
        printErr: (text) => console.error('[ENGINE ERROR]:', text),
        noInitialRun: true
    });
    
    console.log('✓ Module loaded successfully\n');
    
    // Create stdin buffer for commands
    const commands = [
        'uci',
        'isready',
        'ucinewgame',
        'position startpos',
        'go depth 3',
        'position startpos moves e2e4',
        'go depth 3',
        'quit'
    ];
    
    console.log('=== Test Commands ===');
    commands.forEach((cmd, i) => console.log(`${i + 1}. ${cmd}`));
    console.log('\n=== Executing UCI Protocol ===\n');
    
    // Setup stdin with commands
    let commandIndex = 0;
    const stdinBuffer = commands.join('\n') + '\n';
    let stdinPos = 0;
    
    module.FS.init(
        () => {
            if (stdinPos < stdinBuffer.length) {
                const char = stdinBuffer.charCodeAt(stdinPos);
                stdinPos++;
                console.log(`[STDIN] Sending char: ${String.fromCharCode(char)} (${char})`);
                return char;
            }
            return null;
        },
        (char) => {
            process.stdout.write(String.fromCharCode(char));
        },
        (char) => {
            process.stderr.write(String.fromCharCode(char));
        }
    );
    
    console.log('--- Starting Engine ---\n');
    
    try {
        const exitCode = module.callMain([]);
        console.log(`\n--- Engine Exited with code: ${exitCode} ---`);
    } catch (error) {
        console.error('\n[ERROR]:', error.message);
    }
    
    console.log('\n=== Test Complete ===');
}

testUCI().catch(err => {
    console.error('Fatal error:', err);
    process.exit(1);
});
