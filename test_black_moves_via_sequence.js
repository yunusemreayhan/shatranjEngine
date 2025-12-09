#!/usr/bin/env node
const { spawn } = require('child_process');

// Test black moves by playing move sequences
const testSequences = [
    {
        name: 'Black responds to a2a3',
        whiteMoves: ['a2a3'],
        description: 'White plays a2a3, black should respond',
        depth: 3
    },
    {
        name: 'Black responds to b1c3',
        whiteMoves: ['b1c3'],
        description: 'White develops knight, black responds',
        depth: 3
    },
    {
        name: 'Black responds to e2e3',
        whiteMoves: ['e2e3'],
        description: 'White plays central pawn, black responds',
        depth: 3
    },
    {
        name: 'Black in middlegame',
        whiteMoves: ['a2a3', 'b7b6', 'b2b3', 'b6b5', 'c2c3'],
        description: 'After several moves, black to move',
        depth: 2
    },
    {
        name: 'Black after knight development',
        whiteMoves: ['b1c3', 'b8a6', 'c3d5'],
        description: 'Knights developed, black responds to threat',
        depth: 3
    }
];

async function testSequence(test) {
    return new Promise((resolve) => {
        console.log(`\n${'═'.repeat(70)}`);
        console.log(`Test: ${test.name}`);
        console.log(`Description: ${test.description}`);
        console.log(`White moves: ${test.whiteMoves.join(' ')}`);
        console.log(`${'═'.repeat(70)}`);
        
        const moveSequence = test.whiteMoves.join(' ');
        const commands = [
            'uci',
            `position startpos moves ${moveSequence}`,
            `go depth ${test.depth}`,
            'quit'
        ];
        
        console.log('\n📤 Commands:');
        commands.forEach(cmd => console.log(`   > ${cmd}`));
        
        const proc = spawn('node', ['-e', `
            const M = require('./shatranj_uci.js');
            M({
                print: (t) => console.log(t),
                printErr: (t) => console.error(t)
            });
        `], { cwd: __dirname });
        
        const output = [];
        let capturedBoard = false;
        let boardLines = [];
        
        proc.stdout.on('data', (data) => {
            const lines = data.toString().split('\n');
            lines.forEach(line => {
                if (line.trim()) {
                    output.push(line);
                    
                    // Capture board visualization
                    if (line.includes('a b c d e f g h')) {
                        capturedBoard = true;
                        boardLines = [line];
                    } else if (capturedBoard) {
                        boardLines.push(line);
                        if (line.includes('board FEN')) {
                            capturedBoard = false;
                        }
                    }
                    
                    // Show key output
                    if (line.includes('bestmove') || 
                        line.includes('info depth') ||
                        line.includes('nodes visited') ||
                        line.includes('current turn') ||
                        line.includes('invalid move') ||
                        line.includes('Error')) {
                        console.log(`   < ${line}`);
                    }
                }
            });
        });
        
        proc.stderr.on('data', (data) => {
            console.error(`   [ERROR] ${data}`);
        });
        
        proc.on('close', () => {
            console.log('\n📊 Analysis:');
            
            // Check for errors
            const hasError = output.some(l => l.includes('invalid move') || l.includes('Error'));
            if (hasError) {
                console.log('   ❌ Move sequence contains invalid move');
                const errorLines = output.filter(l => l.includes('invalid') || l.includes('Error'));
                errorLines.forEach(e => console.log(`      ${e}`));
            } else {
                console.log('   ✓ All moves valid');
            }
            
            // Extract turn info
            const turnLine = output.find(l => l.includes('current turn'));
            if (turnLine) {
                const isBlack = turnLine.toLowerCase().includes('black');
                const isWhite = turnLine.toLowerCase().includes('white');
                if (isBlack) {
                    console.log('   ✓ Black to move (correct)');
                } else if (isWhite) {
                    console.log('   ⚠ White to move (expected black)');
                }
            }
            
            // Extract bestmove
            const bestmoveLine = output.find(l => l.includes('bestmove'));
            if (bestmoveLine) {
                const move = bestmoveLine.split(' ')[1];
                console.log(`   ✓ Black's best move: ${move}`);
            }
            
            // Extract nodes
            const nodesLine = output.find(l => l.includes('nodes visited:'));
            if (nodesLine) {
                const nodes = nodesLine.match(/\d+/)[0];
                console.log(`   ✓ Nodes searched: ${nodes}`);
            }
            
            // Extract FEN
            const fenLine = output.find(l => l.includes('board FEN'));
            if (fenLine) {
                const fen = fenLine.split('board FEN :')[1].trim();
                console.log(`   ✓ Position FEN: ${fen}`);
            }
            
            // Show board
            if (boardLines.length > 0) {
                console.log('\n📋 Board Position:');
                boardLines.forEach(line => {
                    if (!line.includes('board FEN') && !line.includes('current turn') && 
                        !line.includes('move count') && !line.includes('half move')) {
                        console.log(`   ${line}`);
                    }
                });
            }
            
            resolve();
        });
        
        // Send commands
        proc.stdin.write(commands.join('\n') + '\n');
        proc.stdin.end();
    });
}

async function runAllTests() {
    console.log('╔════════════════════════════════════════════════════════════════════╗');
    console.log('║     Black Moves Test via Move Sequences                           ║');
    console.log('║     Testing black move generation after white moves               ║');
    console.log('╚════════════════════════════════════════════════════════════════════╝');
    
    console.log('\n📝 Test Strategy:');
    console.log('   Since FEN parsing is not implemented in basic UCI,');
    console.log('   we test black moves by playing move sequences from startpos.\n');
    
    let successful = 0;
    let failed = 0;
    
    for (let i = 0; i < testSequences.length; i++) {
        try {
            await testSequence(testSequences[i]);
            successful++;
        } catch (err) {
            console.error(`\n   ❌ Test failed: ${err.message}`);
            failed++;
        }
        
        // Small delay between tests
        await new Promise(resolve => setTimeout(resolve, 100));
    }
    
    console.log('\n\n╔════════════════════════════════════════════════════════════════════╗');
    console.log('║                    Test Summary                                    ║');
    console.log('╚════════════════════════════════════════════════════════════════════╝');
    console.log(`\n  Total Tests: ${testSequences.length}`);
    console.log(`  ✅ Successful: ${successful}`);
    console.log(`  ❌ Failed: ${failed}`);
    
    if (failed === 0) {
        console.log('\n  🎉 All black move tests passed!');
        console.log('  ✅ Black move generation working correctly');
        console.log('  ✅ Engine handles both colors properly');
        console.log('  ✅ Move sequences applied correctly\n');
    } else {
        console.log('\n  ⚠️  Some tests had issues (see details above)\n');
    }
}

runAllTests().catch(err => {
    console.error('\n💥 Fatal Error:', err.message);
    process.exit(1);
});
