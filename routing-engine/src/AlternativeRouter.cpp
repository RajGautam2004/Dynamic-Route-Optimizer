#include "AlternativeRouter.h"
#include "DijkstraRouter.h"

namespace route_optimizer {

AlternativeRoutesResult AlternativeRouter::calculateAlternativeRoutes(
    Graph& graph, 
    const std::string& source, 
    const std::string& destination, 
    const RouteScorer& scorer,
    int numAlternatives) 
{
    AlternativeRoutesResult result;
    DijkstraRouter dijkstra;
    
    // 1. Calculate Primary Route
    result.primaryRoute = dijkstra.calculateRoute(graph, source, destination, scorer);
    if (!result.primaryRoute.success) {
        return result; // Can't even find primary
    }
    result.success = true;

    // 2. Find alternatives by temporarily disabling edges along the primary route
    const auto& path = result.primaryRoute.path;
    
    for (size_t i = 0; i < path.size() - 1 && result.alternativeRoutes.size() < numAlternatives; ++i) {
        std::string u = path[i];
        std::string v = path[i+1];

        // Temporarily disable edge
        graph.updateEdgeStatus(u, v, false);

        // Calculate new route
        RouteResult altRoute = dijkstra.calculateRoute(graph, source, destination, scorer);
        altRoute.algorithmUsed = "Alternative Route (Edge Disabled)";

        // If a valid alternative is found and it's not identical to existing ones
        if (altRoute.success) {
            bool isDuplicate = false;
            for (const auto& existing : result.alternativeRoutes) {
                if (existing.path == altRoute.path) {
                    isDuplicate = true;
                    break;
                }
            }
            if (!isDuplicate) {
                result.alternativeRoutes.push_back(altRoute);
            }
        }

        // Restore edge
        graph.updateEdgeStatus(u, v, true);
    }

    return result;
}

} // namespace route_optimizer
