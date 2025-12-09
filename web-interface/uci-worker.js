importScripts('shatranj_simple_uci.js');

self.onmessage = async function(e) {
    const { moveSeq, thinkTime, depth } = e.data;
    
    console.log(`[UCI Worker] Starting engine for position: ${moveSeq || 'startpos'}`);
    
    const goCommand = depth ? `go depth ${depth}` : `go movetime ${thinkTime * 1000}`;
    console.log(`[UCI Worker] Search: ${depth ? `depth ${depth}` : `${thinkTime}s`}`);
    
    const commands = [
        'uci',
        moveSeq ? `position startpos moves ${moveSeq}` : 'position startpos',
        goCommand,
        'quit'
    ].join('\n') + '\n';
    
    let stdinPos = 0;
    const startTime = Date.now();
    
    try {
        await ShatranjSimpleUCI({
            print: (text) => {
                console.log(`[UCI Engine] ${text}`);
                if (text.includes('bestmove')) {
                    const elapsed = ((Date.now() - startTime) / 1000).toFixed(1);
                    console.log(`[UCI Worker] Received bestmove after ${elapsed}s`);
                    const parts = text.split(' ');
                    const moveStr = parts[1];
                    if (moveStr && moveStr.length >= 4) {
                        self.postMessage({
                            from: moveStr.substring(0, 2),
                            to: moveStr.substring(2, 4)
                        });
                        self.close();
                    }
                }
            },
            printErr: (text) => console.error(`[UCI Error] ${text}`),
            stdin: () => {
                if (stdinPos < commands.length) {
                    return commands.charCodeAt(stdinPos++);
                }
                return null;
            }
        });
    } catch (e) {
        console.error(`[UCI Worker] Error: ${e.message}`);
        self.postMessage(null);
        self.close();
    }
};
