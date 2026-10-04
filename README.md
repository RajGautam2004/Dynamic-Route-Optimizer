# Amazon-inspired Dynamic Route Optimizer

## 1. Problem
A system that dynamically determines the best route for moving an order through a network of Fulfillment Centers, Warehouses, Sort Centers, Delivery Hubs, Roads/Network Links, and Customers. The system adapts dynamically when roads fail, hubs become unavailable, or traffic changes.

## 2. Motivation
To build a production-quality backend/system-design project demonstrating real software engineering ability through C++, DSA, graph algorithms, LLD, HLD, Linux, concurrency, distributed systems, databases, caching, event-driven architecture, APIs, fault tolerance, scalability, Docker, and AWS.

## 3. Architecture
Node.js API Layer -> (PostgreSQL, Redis, Kafka) -> C++ Routing Service -> Graph Engine -> Route Optimizer

## 4. Tech Stack
*   **Core Engine:** C++17, STL, Graph algorithms, Multithreading
*   **Backend/API:** Node.js, Express.js, TypeScript
*   **Database:** PostgreSQL
*   **Cache:** Redis
*   **Event Streaming:** Apache Kafka
*   **OS/System:** Linux
*   **Infrastructure:** Docker, Docker Compose, AWS-ready architecture

## 5. Development Phases
1.  ✅ **Phase 1:** C++ graph engine.
2.  ✅ **Phase 2:** Dijkstra/A*/multi-source algorithms. (Added RouteScorer, Router Interface, Dijkstra, A*, Multi-source, and K-Alternative routers)
3.  ✅ **Phase 3:** Dynamic graph updates. (Added RouteManager to handle edge closures and route recalculation)
4.  ✅ **Phase 4:** Node.js API. (Scaffolded Express.js with TypeScript and modular routes)
5.  ✅ **Phase 5:** Node <-> C++ communication. (Implemented lightweight C++ HTTP server and Node.js fetch client for JSON inter-process communication)
6.  ✅ **Phase 6:** PostgreSQL. (Created full relational schema including customers, inventory, routing history, and system events)
7.  ✅ **Phase 7:** Redis. (Implemented RedisCache service with TTL and integrated into Node.js API)
8.  ✅ **Phase 8:** Kafka. (Implemented producer/consumer for asynchronous event-driven architecture, idempotency checks via Postgres, and automated cache/graph invalidation on ROAD_CLOSED)
9.  ✅ **Phase 9:** Concurrency. (Implemented in Phase 1 & 3 using C++ `std::shared_mutex` and `std::lock_guard` for thread-safe graph read/writes)
10. ✅ **Phase 10:** Failure simulation. (Built a CLI script in TS to inject faults like road failures and trigger route recalculations end-to-end)
11. ✅ **Phase 11:** Testing. (Created C++ unit tests for graph logic and Dijkstra, and Node.js Jest/Supertest suite for API endpoints and Kafka mocking)
12. ✅ **Phase 12:** Benchmarking. (Implemented C++ performance suite in `benchmarks/` to compare Dijkstra vs A* execution time on random graphs of size 100, 1000, 10000)
13. ✅ **Phase 13:** Docker. (Created multi-stage Ubuntu Dockerfile for C++, Alpine Dockerfile for Node, and an orchestration `docker-compose.yml` for all 6 microservices)
14. ✅ **Phase 14:** Documentation. (Created HLD, LLD, and API markdown files with Mermaid diagrams)
15. ✅ **Phase 15:** AWS architecture documentation. (Detailed ECS, RDS, MSK, ElastiCache mappings in HLD.md)

---
## 6. How to Run

Since this is a massive microservice backend architecture, the easiest way to launch the entire ecosystem is via Docker Compose.

**1. Start the System:**
Open your terminal in the root directory of this project and run:
```bash
docker compose up --build
```
This single command will compile the C++ engine, install Node.js dependencies, initialize the PostgreSQL schema, and start Kafka, Zookeeper, and Redis.

**2. Watch the Logs:**
You will see a stream of logs from all 6 containers. Look for:
*   `postgres`: `database system is ready to accept connections`
*   `routing-engine`: `Starting C++ Routing Engine HTTP Service on port 8080...`
*   `backend`: `✅ Kafka connected successfully` & `🚀 Dynamic Route Optimizer API is running on port 3000`

## 7. How to Interact & Sample Output

Since there is no frontend UI, you act as the API client to interact with the system.

### Action 1: Ask for a Route (Cache Hit/Miss Demonstration)
Open a new terminal and run:
```bash
curl -X POST http://localhost:3000/api/routes/calculate \
-H "Content-Type: application/json" \
-d '{"source": "FC_01", "destination": "CUSTOMER_01", "algorithm": "dijkstra"}'
```
**Output (Cache Miss -> Computed by C++):**
```json
{
  "source": "computed",
  "data": {
    "success": true,
    "path": ["FC_01", "HUB_01", "CUSTOMER_01"],
    "totalDistance": 25.0,
    "estimatedTime": 35.0,
    "cost": 500.0,
    "algorithm": "Dijkstra"
  }
}
```
*Note: If you run the exact same command again, `"source"` will say `"cache"` because Redis returns it instantly without hitting the C++ engine.*

### Action 2: Simulate a Road Failure (Event-Driven Architecture)
We built a CLI simulator to inject chaos into the network. Run:
```bash
npx ts-node scripts/simulator.ts simulate road-failure FC_01 HUB_01
```
**What happens in the background?**
Watch your `docker compose` logs. You will see the event-driven magic happen live:
1. Node.js outputs: `[Kafka] Published ROAD_CLOSED to network-events`
2. Kafka Consumer outputs: `[Kafka] Processing ROAD_CLOSED...` and instantly clears the affected Redis cache.
3. The C++ Engine receives the update, safely locks the graph (`std::unique_lock`), removes the edge, and recalculates paths for any active shipments.
4. Finally, you will see `[Kafka] Published ROUTE_RECALCULATED to route-events`, proving the system dynamically healed itself and found a new route!

---
**🏆 Project Successfully Completed!**
