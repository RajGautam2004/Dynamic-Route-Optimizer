import app from './app';
import dotenv from 'dotenv';
import { connectKafka } from './events/KafkaClient';

dotenv.config();

const PORT = process.env.PORT || 3000;

const startServer = async () => {
    // Start Kafka consumer
    await connectKafka();

    app.listen(PORT, () => {
        console.log(`🚀 Dynamic Route Optimizer API is running on port ${PORT}`);
    });
};

startServer();
