import { Pool } from 'pg';
import dotenv from 'dotenv';

dotenv.config();

// Uses connection pooling for scalability
export const dbPool = new Pool({
    user: process.env.PG_USER || 'postgres',
    host: process.env.PG_HOST || 'localhost',
    database: process.env.PG_DATABASE || 'route_optimizer',
    password: process.env.PG_PASSWORD || 'postgres',
    port: parseInt(process.env.PG_PORT || '5432', 10),
    max: 20, // Max number of connections in the pool
    idleTimeoutMillis: 30000,
});

// Helper for executing queries with automatic error logging
export const query = async (text: string, params?: any[]) => {
    const start = Date.now();
    try {
        const res = await dbPool.query(text, params);
        const duration = Date.now() - start;
        console.log('Executed query', { text, duration, rows: res.rowCount });
        return res;
    } catch (error) {
        console.error('Database query error:', { text, error });
        throw error;
    }
};

// Helper for Transactions
export const getTransactionClient = async () => {
    const client = await dbPool.connect();
    return client;
};
