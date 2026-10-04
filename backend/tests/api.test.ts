import request from 'supertest';
import app from '../src/app';
import { redisCache } from '../src/services/RedisCache';
import { producer, consumer } from '../src/events/KafkaClient';

// Mock the external services to ensure unit tests run in isolation
jest.mock('../src/services/RedisCache', () => ({
    redisCache: {
        getCachedRoute: jest.fn(),
        setCachedRoute: jest.fn(),
        invalidateRoute: jest.fn()
    }
}));

jest.mock('../src/events/KafkaClient', () => ({
    publishEvent: jest.fn(),
    connectKafka: jest.fn()
}));

jest.mock('../src/services/RoutingEngineClient', () => {
    return {
        RoutingEngineClient: jest.fn().mockImplementation(() => {
            return {
                calculateRoute: jest.fn().mockResolvedValue({
                    success: true,
                    path: ['FC_01', 'HUB_01', 'CUSTOMER_01'],
                    totalDistance: 25.0
                })
            };
        })
    };
});

describe('Backend API Routes', () => {
    beforeEach(() => {
        jest.clearAllMocks();
    });

    it('should pass health check', async () => {
        const res = await request(app).get('/health');
        expect(res.statusCode).toEqual(200);
        expect(res.body.status).toEqual('ok');
    });

    it('POST /api/routes/calculate should return a computed route', async () => {
        const payload = { source: 'FC_01', destination: 'CUSTOMER_01', algorithm: 'dijkstra' };
        
        // Simulate cache miss
        (redisCache.getCachedRoute as jest.Mock).mockResolvedValue(null);

        const res = await request(app)
            .post('/api/routes/calculate')
            .send(payload);
            
        expect(res.statusCode).toEqual(200);
        expect(res.body.source).toEqual('computed');
        expect(res.body.data.success).toEqual(true);
        expect(res.body.data.path).toContain('FC_01');
    });

    it('POST /api/network/roads/close should dispatch Kafka event', async () => {
        const payload = { source: 'FC_01', destination: 'HUB_01' };
        
        const res = await request(app)
            .post('/api/network/roads/close')
            .send(payload);
            
        expect(res.statusCode).toEqual(202);
        expect(res.body.message).toMatch(/Road closure event dispatched/);
        
        // Verify kafka producer was called
        const { publishEvent } = require('../src/events/KafkaClient');
        expect(publishEvent).toHaveBeenCalledWith('network-events', 'ROAD_CLOSED', payload);
    });
});
