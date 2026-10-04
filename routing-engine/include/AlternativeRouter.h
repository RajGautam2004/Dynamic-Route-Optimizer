#pragma once

#include "Router.h"
#include <vector>

namespace route_optimizer {

struct AlternativeRoutesResult {
    RouteResult primaryRoute;
    std::vector<RouteResult> alternativeRoutes;
    bool success = false;
};

class AlternativeRouter {
public:
    // Calculates the primary route and K alternative routes.
    // Uses a simplified approach (e.g., removing critical edges from primary route to find alternatives).
    AlternativeRoutesResult calculateAlternativeRoutes(
        Graph& graph, // non-const because we temporarily modify edges for alternatives
        const std::string& source, 
        const std::string& destination, 
        const RouteScorer& scorer,
        int numAlternatives = 2);
};

} // namespace route_optimizer
