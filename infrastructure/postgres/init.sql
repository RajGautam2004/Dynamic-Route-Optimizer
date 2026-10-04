-- Enable UUID extension
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- 1. Customers
CREATE TABLE customers (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(255) NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    address TEXT NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- 2. Products
CREATE TABLE products (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    sku VARCHAR(100) UNIQUE NOT NULL,
    name VARCHAR(255) NOT NULL,
    weight_kg DECIMAL(10, 2) NOT NULL,
    dimensions_cm VARCHAR(50), -- e.g., "10x20x30"
    price DECIMAL(10, 2) NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- 3. Network Nodes (Base table for FCs and Hubs)
CREATE TABLE network_nodes (
    id VARCHAR(50) PRIMARY KEY, -- e.g., 'FC_01', 'HUB_04'
    type VARCHAR(50) NOT NULL CHECK (type IN ('WAREHOUSE', 'FULFILLMENT_CENTER', 'SORT_CENTER', 'DELIVERY_HUB', 'CUSTOMER')),
    name VARCHAR(255) NOT NULL,
    location_lat DECIMAL(10, 6),
    location_lng DECIMAL(10, 6),
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- 4. Fulfillment Centers (Extends network_nodes)
CREATE TABLE fulfillment_centers (
    node_id VARCHAR(50) PRIMARY KEY REFERENCES network_nodes(id) ON DELETE CASCADE,
    storage_capacity DECIMAL(10, 2) NOT NULL,
    current_utilization DECIMAL(10, 2) DEFAULT 0
);

-- 5. Hubs (Extends network_nodes)
CREATE TABLE hubs (
    node_id VARCHAR(50) PRIMARY KEY REFERENCES network_nodes(id) ON DELETE CASCADE,
    processing_capacity_per_hour INT NOT NULL
);

-- 6. Inventory
CREATE TABLE inventory (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    fulfillment_center_id VARCHAR(50) REFERENCES fulfillment_centers(node_id),
    product_id UUID REFERENCES products(id),
    quantity INT NOT NULL CHECK (quantity >= 0),
    reserved_quantity INT DEFAULT 0 CHECK (reserved_quantity >= 0),
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(fulfillment_center_id, product_id)
);

-- 7. Network Edges (Roads/Links)
CREATE TABLE network_edges (
    source_id VARCHAR(50) REFERENCES network_nodes(id),
    destination_id VARCHAR(50) REFERENCES network_nodes(id),
    distance_km DECIMAL(10, 2) NOT NULL,
    base_travel_time_min DECIMAL(10, 2) NOT NULL,
    capacity DECIMAL(10, 2) NOT NULL,
    current_load DECIMAL(10, 2) DEFAULT 0,
    congestion_factor DECIMAL(5, 2) DEFAULT 1.0,
    cost DECIMAL(10, 2) NOT NULL,
    is_operational BOOLEAN DEFAULT TRUE,
    last_updated TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (source_id, destination_id)
);

-- 8. Orders
CREATE TABLE orders (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    customer_id UUID REFERENCES customers(id),
    status VARCHAR(50) NOT NULL DEFAULT 'CREATED', -- CREATED, PROCESSING, SHIPPED, DELIVERED, CANCELLED
    priority VARCHAR(20) DEFAULT 'STANDARD', -- HIGH, STANDARD, LOW
    delivery_deadline TIMESTAMP WITH TIME ZONE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- 9. Order Items
CREATE TABLE order_items (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    order_id UUID REFERENCES orders(id) ON DELETE CASCADE,
    product_id UUID REFERENCES products(id),
    quantity INT NOT NULL CHECK (quantity > 0),
    price_at_purchase DECIMAL(10, 2) NOT NULL
);

-- 10. Shipments
CREATE TABLE shipments (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    order_id UUID REFERENCES orders(id),
    source_node_id VARCHAR(50) REFERENCES network_nodes(id),
    destination_node_id VARCHAR(50) REFERENCES network_nodes(id),
    current_node_id VARCHAR(50) REFERENCES network_nodes(id),
    status VARCHAR(50) NOT NULL DEFAULT 'PENDING', -- PENDING, IN_TRANSIT, DELIVERED, DELAYED, FAILED
    estimated_arrival TIMESTAMP WITH TIME ZONE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- 11. Routes (Current Active Route for a Shipment)
CREATE TABLE routes (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    shipment_id UUID UNIQUE REFERENCES shipments(id) ON DELETE CASCADE,
    path_nodes JSONB NOT NULL, -- Array of node IDs e.g. ["FC_01", "HUB_01", "CUSTOMER_01"]
    total_distance_km DECIMAL(10, 2) NOT NULL,
    estimated_time_min DECIMAL(10, 2) NOT NULL,
    total_cost DECIMAL(10, 2) NOT NULL,
    algorithm_used VARCHAR(50),
    calculated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- 12. Route History (Audit log for recalculations)
CREATE TABLE route_history (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    shipment_id UUID REFERENCES shipments(id) ON DELETE CASCADE,
    previous_path JSONB NOT NULL,
    new_path JSONB NOT NULL,
    reason VARCHAR(255), -- e.g., "ROAD_CLOSED: HUB_01 -> HUB_02"
    changed_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- 13. System Events (Dead-letter or audit for Kafka)
CREATE TABLE system_events (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    event_type VARCHAR(100) NOT NULL,
    payload JSONB NOT NULL,
    processed BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Indexes for performance
CREATE INDEX idx_network_edges_operational ON network_edges(is_operational);
CREATE INDEX idx_shipments_status ON shipments(status);
CREATE INDEX idx_orders_customer ON orders(customer_id);
CREATE INDEX idx_route_history_shipment ON route_history(shipment_id);
