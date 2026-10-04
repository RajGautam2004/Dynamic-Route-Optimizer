export class RoutingEngineClient {
    private readonly baseUrl = 'http://localhost:8080/api/v1';

    async calculateRoute(source: string, destination: string, algorithm: string = 'dijkstra', constraints: any = {}) {
        try {
            const response = await fetch(`${this.baseUrl}/calculate`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ source, destination, algorithm, constraints })
            });
            
            if (!response.ok) {
                throw new Error(`Routing Engine Error: ${response.statusText}`);
            }
            
            return await response.json();
        } catch (error) {
            console.error('Failed to communicate with C++ Routing Engine:', error);
            throw error;
        }
    }

    async updateNetworkEdge(source: string, destination: string, isOperational: boolean) {
        try {
            const response = await fetch(`${this.baseUrl}/network/update`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ source, destination, isOperational })
            });
            return await response.json();
        } catch (error) {
            console.error('Failed to update C++ network:', error);
            throw error;
        }
    }
}
