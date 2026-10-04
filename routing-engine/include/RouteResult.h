#pragma once

#include <vector>
#include <string>

namespace route_optimizer {

struct RouteResult {
    std::vector<std::string> path;
    double totalDistance = 0.0;
    double estimatedTime = 0.0;
    double totalCost = 0.0;
    std::string algorithmUsed;
    bool success = false;
};

} // namespace route_optimizer
