#pragma once

#include "Graph.h"
#include "Router.h"
#include "AlternativeRouter.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>

namespace route_optimizer {

struct ActiveShipment {
    std::string shipmentId;
    std::string source;
    std::string destination;
    RouteResult currentRoute;
};

// Orchestrates dynamic graph updates and route recalculations.
class RouteManager {
public:
    RouteManager(std::shared_ptr<Graph> graph, std::shared_ptr<Router> primaryRouter, RouteScorer scorer);

    // Registers an active shipment so it can be monitored for route failures
    void registerShipment(const ActiveShipment& shipment);
    
    // Unregisters a shipment (e.g. when delivered)
    void completeShipment(const std::string& shipmentId);

    // Receives a ROAD_CLOSED or ROAD_REOPENED event.
    // Returns a list of shipment IDs that were affected and their new recalculated routes.
    std::vector<ActiveShipment> handleEdgeUpdate(const std::string& sourceId, const std::string& destId, bool isOperational);

    std::vector<ActiveShipment> getActiveShipments() const;

private:
    std::shared_ptr<Graph> graph_;
    std::shared_ptr<Router> router_;
    RouteScorer scorer_;
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ActiveShipment> activeShipments_;

    bool isRouteAffected(const RouteResult& route, const std::string& edgeSource, const std::string& edgeDest) const;
};

} // namespace route_optimizer
