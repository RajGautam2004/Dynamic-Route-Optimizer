#include "AStarRouter.h"
#include <queue>
#include <unordered_map>
#include <algorithm>

namespace route_optimizer {

AStarRouter::AStarRouter(HeuristicFunction heuristic) : heuristic_(std::move(heuristic)) {}

RouteResult AStarRouter::calculateRoute(
    const Graph& graph, 
    const std::string& source, 
    const std::string& destination, 
    const RouteScorer& scorer) 
{
    RouteResult result;
    result.algorithmUsed = "A*";
    result.success = false;

    if (!graph.getNode(source) || !graph.getNode(destination)) {
        return result; 
    }

    // Min-heap priority queue: {fScore, nodeId}
    using P = std::pair<double, std::string>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    
    // gScore: cost from source to node
    std::unordered_map<std::string, double> gScores;
    std::unordered_map<std::string, std::string> previousNode;
    std::unordered_map<std::string, Edge> edgeToReach;

    gScores[source] = 0.0;
    double initialHeuristic = heuristic_(source, destination);
    pq.push({initialHeuristic, source});

    while (!pq.empty()) {
        auto [currentFScore, currentId] = pq.top();
        pq.pop();

        if (currentId == destination) {
            break; // Found the path
        }

        auto edges = graph.getOutgoingEdges(currentId);
        for (const auto& edge : edges) {
            if (!edge.isOperational) continue;

            double tentativeGScore = gScores[currentId] + scorer.calculateScore(edge);

            if (gScores.find(edge.destinationId) == gScores.end() || tentativeGScore < gScores[edge.destinationId]) {
                gScores[edge.destinationId] = tentativeGScore;
                double fScore = tentativeGScore + heuristic_(edge.destinationId, destination);
                
                previousNode[edge.destinationId] = currentId;
                edgeToReach[edge.destinationId] = edge;
                pq.push({fScore, edge.destinationId});
            }
        }
    }

    // Reconstruct path
    if (previousNode.find(destination) == previousNode.end()) {
        return result; // No path found
    }

    std::string current = destination;
    while (current != source) {
        result.path.push_back(current);
        const Edge& e = edgeToReach[current];
        result.totalDistance += e.distance;
        result.estimatedTime += e.travelTime;
        result.totalCost += e.cost;
        current = previousNode[current];
    }
    result.path.push_back(source);
    std::reverse(result.path.begin(), result.path.end());
    result.success = true;

    return result;
}

} // namespace route_optimizer
