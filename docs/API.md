# API Documentation

## Node.js REST API

### 1. Calculate Route
*   **URL:** `/api/routes/calculate`
*   **Method:** `POST`
*   **Description:** Calculates the optimal route. Checks Redis cache first.
*   **Body:**
    ```json
    {
      "source": "FC_01",
      "destination": "CUSTOMER_01",
      "algorithm": "dijkstra"
    }
    ```
*   **Response:** `200 OK`
    ```json
    {
      "source": "computed",
      "data": {
        "success": true,
        "path": ["FC_01", "HUB_01", "CUSTOMER_01"],
        "totalDistance": 25.0
      }
    }
    ```

### 2. Simulate Road Closure
*   **URL:** `/api/network/roads/close`
*   **Method:** `POST`
*   **Description:** Publishes a ROAD_CLOSED event to Kafka.
*   **Body:**
    ```json
    {
      "source": "FC_01",
      "destination": "HUB_01"
    }
    ```
*   **Response:** `202 Accepted`

---

## C++ Internal RPC API

*(Used internally by the Node.js layer over port 8080)*

### 1. Update Network Edge
*   **URL:** `http://routing-engine:8080/api/v1/network/update`
*   **Method:** `POST`
*   **Body:**
    ```json
    {
      "source": "FC_01",
      "destination": "HUB_01",
      "isOperational": false
    }
    ```
*   **Response:** `200 OK`
    ```json
    {
      "message": "Graph updated",
      "affectedShipments": [
        {
          "shipmentId": "SHIP_123",
          "newRoute": ["FC_01", "HUB_02", "CUSTOMER_01"]
        }
      ]
    }
    ```
