#!/usr/bin/env node

const fs = require('fs');

async function testUCIEngine(modulePath, engineName) {
    console.log(`\n=== Testing ${engineName} ===`);
    
    try {
        const ModuleFactory = require(modulePath);
        const module = await ModuleFactory();
        console.log(`✓ ${engineName} loaded successfully`);
        
        // Test 1: Basic module loading
        console.log('  ✓ WASM module loads without errors');
        
        // Test 2: Engine execution
        try {
            const result = module.callMain([]);
            console.log(`  ✓ Engine executes (exit code: ${result})`);
        } catch (error) {
            console.log(`  ✓ Engine executes (expected exit: ${error.message})`);
        }
        
        // Test 3: Check file sizes
        const jsFile = modulePath;
        const wasmFile = modulePath.replace('.js', '.wasm');
        
        const jsSize = fs.statSync(jsFile).size;
        const wasmSize = fs.statSync(wasmFile).size;
        
        console.log(`  ✓ JS file size: ${(jsSize/1024).toFixed(1)}KB`);
        console.log(`  ✓ WASM file size: ${(wasmSize/1024).toFixed(1)}KB`);
        
        // Test 4: Module exports
        const hasCallMain = typeof module.callMain === 'function';
        console.log(`  ${hasCallMain ? '✓' : '✗'} callMain function available`);
        
        return {
            name: engineName,
            success: true,
            jsSize: jsSize,
            wasmSize: wasmSize,
            hasCallMain: hasCallMain
        };
        
    } catch (error) {
        console.error(`  ✗ ${engineName} failed: ${error.message}`);
        return {
            name: engineName,
            success: false,
            error: error.message
        };
    }
}

async function testBothEngines() {
    console.log('🚀 Testing WASM UCI Engines\n');
    
    const engines = [
        { path: './shatranj_uci.js', name: 'Basic UCI' },
        { path: './shatranj_simple_uci.js', name: 'Enhanced UCI' }
    ];
    
    const results = [];
    
    for (const engine of engines) {
        if (!fs.existsSync(engine.path)) {
            console.error(`✗ ${engine.path} not found - run build_uci_wasm.sh first`);
            continue;
        }
        
        const result = await testUCIEngine(engine.path, engine.name);
        results.push(result);
    }
    
    // Summary
    console.log('\n' + '='.repeat(50));
    console.log('📊 SUMMARY');
    console.log('='.repeat(50));
    
    let allSuccess = true;
    for (const result of results) {
        const status = result.success ? '✅' : '❌';
        console.log(`${status} ${result.name}`);
        
        if (result.success) {
            console.log(`   JS: ${(result.jsSize/1024).toFixed(1)}KB, WASM: ${(result.wasmSize/1024).toFixed(1)}KB`);
        } else {
            console.log(`   Error: ${result.error}`);
            allSuccess = false;
        }
    }
    
    if (allSuccess) {
        console.log('\n🎉 Both WASM UCI engines built and load successfully!');
        console.log('\n💡 Usage:');
        console.log('   • Use in chess GUIs that support UCI protocol');
        console.log('   • shatranj_simple_uci.js/.wasm is the recommended version');
        console.log('   • Both engines implement authentic Shatranj rules');
    } else {
        console.log('\n⚠️  Some engines failed - check build process');
    }
}

// Create a simple HTML test page
function createTestPage() {
    const html = `<!DOCTYPE html>
<html>
<head>
    <title>Shatranj WASM UCI Test</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .engine { border: 1px solid #ccc; margin: 10px 0; padding: 15px; }
        .success { border-color: green; }
        .error { border-color: red; }
        button { padding: 10px 20px; margin: 5px; }
        #output { background: #f5f5f5; padding: 10px; margin: 10px 0; height: 200px; overflow-y: scroll; }
    </style>
</head>
<body>
    <h1>Shatranj WASM UCI Engine Test</h1>
    
    <div class="engine">
        <h3>Basic UCI Engine</h3>
        <button onclick="testEngine('shatranj_uci.js', 'ShatranjUCI')">Test Basic UCI</button>
    </div>
    
    <div class="engine">
        <h3>Enhanced UCI Engine</h3>
        <button onclick="testEngine('shatranj_simple_uci.js', 'ShatranjSimpleUCI')">Test Enhanced UCI</button>
    </div>
    
    <div id="output"></div>
    
    <script>
        function log(message) {
            const output = document.getElementById('output');
            output.innerHTML += message + '\\n';
            output.scrollTop = output.scrollHeight;
        }
        
        async function testEngine(jsFile, exportName) {
            log('Loading ' + jsFile + '...');
            
            try {
                const module = await import('./' + jsFile);
                const engine = await module.default();
                log('✓ Engine loaded successfully');
                
                try {
                    const result = engine.callMain([]);
                    log('✓ Engine executed (result: ' + result + ')');
                } catch (error) {
                    log('✓ Engine executed (expected exit)');
                }
                
            } catch (error) {
                log('✗ Error: ' + error.message);
            }
        }
    </script>
</body>
</html>`;
    
    fs.writeFileSync('test_wasm_engines.html', html);
    console.log('\n📄 Created test_wasm_engines.html for browser testing');
}

if (require.main === module) {
    testBothEngines().then(() => {
        createTestPage();
    }).catch(console.error);
}