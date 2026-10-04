import { Router } from 'express';
import { publishEvent } from '../events/KafkaClient';

export const networkRoutes = Router();

// POST /api/network/roads/close
networkRoutes.post('/roads/close', async (req, res) => {
    const { source, destination } = req.body;
    
    // Publish asynchronous event instead of blocking
    await publishEvent('network-events', 'ROAD_CLOSED', { source, destination });
    
    res.status(202).json({ message: `Road closure event dispatched for ${source} -> ${destination}` });
});

// POST /api/network/roads/reopen
networkRoutes.post('/roads/reopen', async (req, res) => {
    const { source, destination } = req.body;
    
    await publishEvent('network-events', 'ROAD_REOPENED', { source, destination });
    
    res.status(202).json({ message: `Road reopen event dispatched for ${source} -> ${destination}` });
});
