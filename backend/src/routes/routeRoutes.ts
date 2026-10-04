import { Router } from 'express';
import { RoutingEngineClient } from '../services/RoutingEngineClient';
import { redisCache } from '../services/RedisCache';

export const routeRoutes = Router();
const routingClient = new RoutingEngineClient();

// POST /api/routes/calculate
routeRoutes.post('/calculate', async (req, res) => {
    try {
        const { source, destination, algorithm, constraints } = req.body;
        
        // 1. Check Redis Cache
        const cachedRoute = await redisCache.getCachedRoute(source, destination);
        if (cachedRoute) {
            return res.status(200).json({ source: 'cache', data: cachedRoute });
        }

        // 2. Cache Miss -> Call C++ Engine
        const result = await routingClient.calculateRoute(source, destination, algorithm, constraints);
        
        // 3. Save to Redis Cache (TTL 1 hour)
        if (result.success || result.route) {
            await redisCache.setCachedRoute(source, destination, result);
        }
        
        return res.status(200).json({ source: 'computed', data: result });
    } catch (error: any) {
        return res.status(500).json({ error: 'Failed to calculate route', details: error.message });
    }
});

// GET /api/routes/:id/alternatives
routeRoutes.get('/:id/alternatives', (req, res) => {
    res.status(200).json({ message: `Alternative routes for ${req.params.id} (stub)` });
});
