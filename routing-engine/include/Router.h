#pragma once

#include "Graph.h"
#include "RouteResult.h"
#include "RouteScorer.h"
#include <string>

namespace route_optimizer {

// Base Strategy Interface for all routing algorithms
class Router {
public:
    virtual ~Router() = default;

    virtual RouteResult calculateRoute(
        const Graph& graph, 
        const std::string& source, 
        const std::string& destination, 
        const RouteScorer& scorer) = 0;
};

} // namespace route_optimizer
