#include "RouteManager.h"
#include <iostream>

namespace route_optimizer {

RouteManager::RouteManager(std::shared_ptr<Graph> graph, std::shared_ptr<Router> primaryRouter, RouteScorer scorer)
    : graph_(std::move(graph)), router_(std::move(primaryRouter)), scorer_(std::move(scorer)) {}

void RouteManager::registerShipment(const ActiveShipment& shipment) {
    std::lock_guard<std::mutex> lock(mutex_);
    activeShipments_[shipment.shipmentId] = shipment;
}

void RouteManager::completeShipment(const std::string& shipmentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    activeShipments_.erase(shipmentId);
}

bool RouteManager::isRouteAffected(const RouteResult& route, const std::string& edgeSource, const std::string& edgeDest) const {
    if (route.path.size() < 2) return false;
    for (size_t i = 0; i < route.path.size() - 1; ++i) {
        if (route.path[i] == edgeSource && route.path[i+1] == edgeDest) {
            return true;
        }
    }
    return false;
}

std::vector<ActiveShipment> RouteManager::handleEdgeUpdate(const std::string& sourceId, const std::string& destId, bool isOperational) {
    std::vector<ActiveShipment> updatedShipments;
    
    // 1. Update Graph State
    bool updated = graph_->updateEdgeStatus(sourceId, destId, isOperational);
    if (!updated) {
        return updatedShipments; // Edge doesn't exist
    }

    std::lock_guard<std::mutex> lock(mutex_);
    
    // 2. Identify affected routes
    for (auto& [shipmentId, shipment] : activeShipments_) {
        // If an edge closed, check if the current route used it.
        // If an edge opened, we might want to recalculate to see if a *better* route exists now.
        // For simplicity, we recalculate if the edge closed and broke the path, OR if it opened (to optimize).
        
        bool brokeCurrentPath = !isOperational && isRouteAffected(shipment.currentRoute, sourceId, destId);
        bool newlyOpened = isOperational; // Could trigger a re-check for optimization
        
        if (brokeCurrentPath || newlyOpened) {
            // 3. Recalculate route
            RouteResult newRoute = router_->calculateRoute(*graph_, shipment.source, shipment.destination, scorer_);
            
            // 4. Compare old and new route
            if (newRoute.success && newRoute.path != shipment.currentRoute.path) {
                shipment.currentRoute = newRoute;
                updatedShipments.push_back(shipment);
            }
        }
    }

    // 5. The caller (Node.js integration layer) will receive these updatedShipments
    // and handle Redis cache invalidation, PostgreSQL updates, and Kafka ROUTE_RECALCULATED events.
    return updatedShipments;
}

std::vector<ActiveShipment> RouteManager::getActiveShipments() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ActiveShipment> shipments;
    for (const auto& [id, s] : activeShipments_) {
        shipments.push_back(s);
    }
    return shipments;
}

} // namespace route_optimizer
