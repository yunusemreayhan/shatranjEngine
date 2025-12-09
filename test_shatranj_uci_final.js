#!/usr/bin/env node
const ShatranjUCI = require('./shatranj_uci.js');

async function verboseTest() {
    console.log('╔════════════════════════════════════════════════════════╗');
    console.log('║     ShatranjUCI WASM Verbose Test Suite              ║');
    console.log('╚════════════════════════════════════════════════════════╝\n');
    
    console.log('📋 Source: main_uci.cpp');
    console.log('   - Implements basic UCI protocol');
    console.log('   - Uses shatranj::UCI class from uci.cpp');
    console.log('   - Supports: uci, isready, position, go, quit\n');
    
    const testCases = [
        {
            name: 'UCI Identification',
            commands: ['uci'],
            expected: ['id name ShatranjEngine', 'id author', 'uciok'],
            description: 'Engine should identify itself and confirm UCI protocol'
        },
        {
            name: 'Ready Check',
            commands: ['uci', 'isready'],
            expected: ['readyok'],
            description: 'Engine should respond ready after initialization'
        },
        {
            name: 'New Game Setup',
            commands: ['uci', 'ucinewgame', 'isready'],
            expected: ['readyok'],
            description: 'Engine should reset board state for new game'
        },
        {
            name: 'Position Setup',
            commands: ['uci', 'position startpos', 'isready'],
            expected: ['readyok'],
            description: 'Engine should accept starting position'
        },
        {
            name: 'Search Depth 3',
            commands: ['uci', 'position startpos', 'go depth 3'],
            expected: ['info depth 3', 'bestmove'],
            description: 'Engine should search and return best move'
        },
        {
            name: 'Position with Moves',
            commands: ['uci', 'position startpos moves e2e4', 'go depth 2'],
            expected: ['bestmove'],
            description: 'Engine should handle position with move history'
        }
    ];
    
    let passed = 0;
    let failed = 0;
    
    for (let i = 0; i < testCases.length; i++) {
        const test = testCases[i];
        console.log(`\n${'─'.repeat(60)}`);
        console.log(`Test ${i + 1}/${testCases.length}: ${test.name}`);
        console.log(`Description: ${test.description}`);
        console.log(`${'─'.repeat(60)}`);
        
        const input = test.commands.join('\n') + '\nquit\n';
        console.log('\n📤 Input Commands:');
        test.commands.forEach(cmd => console.log(`   > ${cmd}`));
        
        console.log('\n📥 Engine Output:');
        
        const output = [];
        await new Promise((resolve) => {
            const { spawn } = require('child_process');
            const proc = spawn('node', ['-e', `
                const M = require('./shatranj_uci.js');
                M({
                    print: (t) => console.log(t),
                    printErr: (t) => console.error(t)
                });
            `], { cwd: __dirname });
            
            proc.stdout.on('data', (data) => {
                const lines = data.toString().split('\n').filter(l => l.trim());
                lines.forEach(line => {
                    console.log(`   < ${line}`);
                    output.push(line);
                });
            });
            
            proc.stderr.on('data', (data) => {
                console.error(`   [ERROR] ${data}`);
            });
            
            proc.on('close', () => resolve());
            
            proc.stdin.write(input);
            proc.stdin.end();
        });
        
        console.log('\n🔍 Validation:');
        let testPassed = true;
        for (const exp of test.expected) {
            const found = output.some(line => line.includes(exp));
            const status = found ? '✓' : '✗';
            console.log(`   ${status} Expected: "${exp}" - ${found ? 'FOUND' : 'NOT FOUND'}`);
            if (!found) testPassed = false;
        }
        
        if (testPassed) {
            console.log('\n✅ Test PASSED');
            passed++;
        } else {
            console.log('\n❌ Test FAILED');
            failed++;
        }
    }
    
    console.log('\n\n╔════════════════════════════════════════════════════════╗');
    console.log('║                    Test Summary                        ║');
    console.log('╚════════════════════════════════════════════════════════╝');
    console.log(`\n  Total Tests: ${testCases.length}`);
    console.log(`  ✅ Passed: ${passed}`);
    console.log(`  ❌ Failed: ${failed}`);
    console.log(`  Success Rate: ${((passed/testCases.length)*100).toFixed(1)}%\n`);
    
    if (failed === 0) {
        console.log('🎉 All tests passed! shatranj_uci.wasm is working correctly.\n');
    } else {
        console.log('⚠️  Some tests failed. Review output above for details.\n');
        process.exit(1);
    }
}

verboseTest().catch(err => {
    console.error('\n💥 Fatal Error:', err.message);
    process.exit(1);
});
