#pragma once

#include <string>

namespace route_optimizer {

struct Edge {
    std::string sourceId;
    std::string destinationId;
    double distance;
    double travelTime;
    double capacity;
    double currentLoad;
    double congestionFactor;
    double cost;
    bool isOperational;

    bool operator==(const Edge& other) const {
        return sourceId == other.sourceId && destinationId == other.destinationId;
    }
};

} // namespace route_optimizer
