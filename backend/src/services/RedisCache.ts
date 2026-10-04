import { createClient, RedisClientType } from 'redis';
import dotenv from 'dotenv';

dotenv.config();

export class RedisCache {
    private client: RedisClientType;

    constructor() {
        this.client = createClient({
            url: process.env.REDIS_URL || 'redis://localhost:6379'
        });

        this.client.on('error', (err) => console.error('Redis Client Error', err));
        this.client.connect().catch(console.error);
    }

    // Cache Hit/Miss mechanism with TTL
    async getCachedRoute(source: string, destination: string): Promise<any | null> {
        const key = `route:${source}:${destination}`;
        const data = await this.client.get(key);
        if (data) {
            console.log(`[Cache Hit] Route ${source} -> ${destination}`);
            return JSON.parse(data);
        }
        console.log(`[Cache Miss] Route ${source} -> ${destination}`);
        return null;
    }

    // Cache setting with configurable TTL (default 1 hour)
    async setCachedRoute(source: string, destination: string, routeData: any, ttlSeconds: number = 3600): Promise<void> {
        const key = `route:${source}:${destination}`;
        await this.client.setEx(key, ttlSeconds, JSON.stringify(routeData));
    }

    // Cache invalidation (e.g. when a road on this route fails)
    async invalidateRoute(source: string, destination: string): Promise<void> {
        const key = `route:${source}:${destination}`;
        await this.client.del(key);
        console.log(`[Cache Invalidation] Cleared ${key}`);
    }

    // Temporary Shipment State
    async setShipmentState(shipmentId: string, state: any, ttlSeconds: number = 300): Promise<void> {
        const key = `shipment_state:${shipmentId}`;
        await this.client.setEx(key, ttlSeconds, JSON.stringify(state));
    }

    async getShipmentState(shipmentId: string): Promise<any | null> {
        const key = `shipment_state:${shipmentId}`;
        const data = await this.client.get(key);
        return data ? JSON.parse(data) : null;
    }
}

export const redisCache = new RedisCache();
