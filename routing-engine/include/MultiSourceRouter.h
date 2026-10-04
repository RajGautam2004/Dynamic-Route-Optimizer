#pragma once

#include "Router.h"
#include <vector>

namespace route_optimizer {

class MultiSourceRouter {
public:
    // Finds the best source among multiple sources to reach the destination.
    // Returns the route from the optimal source.
    RouteResult calculateBestSourceRoute(
        const Graph& graph, 
        const std::vector<std::string>& sources, 
        const std::string& destination, 
        const RouteScorer& scorer);
};

} // namespace route_optimizer
