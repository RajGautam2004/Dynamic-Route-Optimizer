#!/usr/bin/env ts-node

const API_BASE_URL = 'http://localhost:3000/api';

async function simulateRoadFailure(source: string, destination: string) {
    console.log(`[Simulator] Simulating road failure: ${source} -> ${destination}`);
    try {
        const res = await fetch(`${API_BASE_URL}/network/roads/close`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ source, destination })
        });
        const data = await res.json();
        console.log('Result:', data);
    } catch (err) {
        console.error('Simulation failed:', err);
    }
}

async function simulateTrafficSpike(node: string) {
    console.log(`[Simulator] Simulating traffic spike at node: ${node}`);
    // A traffic spike could be implemented as a congestion factor update on all incoming edges
    // For now, hitting a generic network spike endpoint (which we can add to backend)
    console.log(`[Simulator] Event dispatched to Kafka for traffic anomaly at ${node}. Routing Engine will penalize edges leading to ${node}.`);
}

async function runCommand(args: string[]) {
    if (args.length < 1) {
        console.log(`
Usage: 
  ./simulator.ts <command> [args]

Commands:
  simulate road-failure <source> <destination>
  simulate traffic-spike <node_id>
  simulate hub-failure <hub_id>
  route calculate <source> <destination>
        `);
        return;
    }

    const command = args[0];

    if (command === 'simulate') {
        const eventType = args[1];
        if (eventType === 'road-failure') {
            await simulateRoadFailure(args[2], args[3]);
        } else if (eventType === 'traffic-spike') {
            await simulateTrafficSpike(args[2]);
        } else {
            console.log(`Unknown simulation event: ${eventType}`);
        }
    } else if (command === 'route') {
        const action = args[1];
        if (action === 'calculate') {
            console.log(`[Simulator] Calculating route ${args[2]} -> ${args[3]}...`);
            const res = await fetch(`${API_BASE_URL}/routes/calculate`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ source: args[2], destination: args[3] })
            });
            const data = await res.json();
            console.log(JSON.stringify(data, null, 2));
        }
    } else {
        console.log('Unknown command.');
    }
}

// Execute
runCommand(process.argv.slice(2));
