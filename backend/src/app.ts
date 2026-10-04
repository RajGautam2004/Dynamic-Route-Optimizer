import express from 'express';
import cors from 'cors';
import { orderRoutes } from './routes/orderRoutes';
import { routeRoutes } from './routes/routeRoutes';
import { networkRoutes } from './routes/networkRoutes';

const app = express();

app.use(cors());
app.use(express.json());

// Basic health check
app.get('/health', (req, res) => {
    res.status(200).json({ status: 'ok', timestamp: new Date().toISOString() });
});

// API Routes
app.use('/api/orders', orderRoutes);
app.use('/api/routes', routeRoutes);
app.use('/api/network', networkRoutes);

// Global Error Handler
app.use((err: any, req: express.Request, res: express.Response, next: express.NextFunction) => {
    console.error('Unhandled Error:', err);
    res.status(500).json({ error: 'Internal Server Error', message: err.message });
});

export default app;
