import { Kafka, Producer, Consumer, EachMessagePayload } from 'kafkajs';
import dotenv from 'dotenv';
import { query } from '../config/database';
import { RoutingEngineClient } from '../services/RoutingEngineClient';
import { redisCache } from '../services/RedisCache';

dotenv.config();

const kafka = new Kafka({
    clientId: 'dynamic-route-optimizer',
    brokers: [process.env.KAFKA_BROKER || 'localhost:9092']
});

export const producer: Producer = kafka.producer();
export const consumer: Consumer = kafka.consumer({ groupId: 'routing-group' });
const routingClient = new RoutingEngineClient();

export const connectKafka = async () => {
    try {
        await producer.connect();
        await consumer.connect();
        console.log('✅ Kafka connected successfully');

        // Subscribe to relevant topics
        await consumer.subscribe({ topic: 'network-events', fromBeginning: true });
        await consumer.subscribe({ topic: 'order-events', fromBeginning: true });

        // Start listening
        await consumer.run({
            eachMessage: handleKafkaMessage,
        });
    } catch (error) {
        console.error('Failed to connect to Kafka:', error);
    }
};

const handleKafkaMessage = async ({ topic, partition, message }: EachMessagePayload) => {
    if (!message.value) return;

    const eventPayload = JSON.parse(message.value.toString());
    const eventId = eventPayload.eventId; // For idempotency
    const eventType = eventPayload.type;

    try {
        // 1. Idempotency Check
        // Check if event already processed in Postgres
        const existingEvent = await query('SELECT id FROM system_events WHERE id = $1', [eventId]);
        if (existingEvent.rowCount && existingEvent.rowCount > 0) {
            console.log(`[Kafka] Skipping duplicate event ${eventId}`);
            return;
        }

        // Insert as processing
        await query('INSERT INTO system_events (id, event_type, payload) VALUES ($1, $2, $3)', [eventId, eventType, eventPayload]);

        // 2. Route Event Logic
        console.log(`[Kafka] Processing ${eventType}...`);

        if (eventType === 'ROAD_CLOSED' || eventType === 'ROAD_REOPENED') {
            const isOperational = eventType === 'ROAD_REOPENED';
            const { source, destination } = eventPayload.data;

            // Step A: Invalidate Redis Cache
            await redisCache.invalidateRoute(source, destination);

            // Step B: Notify C++ Routing Engine to recalculate affected active shipments
            const result = await routingClient.updateNetworkEdge(source, destination, isOperational);

            if (result.affectedShipments && result.affectedShipments.length > 0) {
                for (const shipment of result.affectedShipments) {
                    // Step C: Update Postgres Route History
                    await query(
                        `INSERT INTO route_history (shipment_id, previous_path, new_path, reason) VALUES ($1, '[]', $2, $3)`,
                        [shipment.shipmentId, JSON.stringify(shipment.newRoute), `${eventType}: ${source}->${destination}`]
                    );

                    // Step D: Publish ROUTE_RECALCULATED event
                    await publishEvent('route-events', 'ROUTE_RECALCULATED', {
                        shipmentId: shipment.shipmentId,
                        newRoute: shipment.newRoute
                    });
                }
            }
        }

        // 3. Mark Processed
        await query('UPDATE system_events SET processed = TRUE WHERE id = $1', [eventId]);

    } catch (error) {
        console.error(`[Kafka] Error processing event ${eventId}:`, error);
        // Do not mark processed, could be retried or sent to DLQ
    }
};

export const publishEvent = async (topic: string, type: string, data: any) => {
    const eventId = require('crypto').randomUUID();
    const message = {
        eventId,
        type,
        timestamp: new Date().toISOString(),
        data
    };

    await producer.send({
        topic,
        messages: [{ value: JSON.stringify(message) }],
    });
    console.log(`[Kafka] Published ${type} to ${topic}`);
};
