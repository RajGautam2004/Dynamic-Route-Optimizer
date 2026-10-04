import { Router } from 'express';

export const orderRoutes = Router();

// POST /api/orders
orderRoutes.post('/', (req, res) => {
    res.status(201).json({ message: 'Order created (stub)' });
});

// GET /api/orders/:id
orderRoutes.get('/:id', (req, res) => {
    res.status(200).json({ message: `Order ${req.params.id} details (stub)` });
});
