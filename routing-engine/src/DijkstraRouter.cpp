#include "DijkstraRouter.h"
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>

namespace route_optimizer {

RouteResult DijkstraRouter::calculateRoute(
    const Graph& graph, 
    const std::string& source, 
    const std::string& destination, 
    const RouteScorer& scorer) 
{
    RouteResult result;
    result.algorithmUsed = "Dijkstra";
    result.success = false;

    if (!graph.getNode(source) || !graph.getNode(destination)) {
        return result; // Source or dest doesn't exist
    }

    // Min-heap priority queue: {score, nodeId}
    using P = std::pair<double, std::string>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    
    std::unordered_map<std::string, double> minScores;
    std::unordered_map<std::string, std::string> previousNode;
    std::unordered_map<std::string, Edge> edgeToReach;

    pq.push({0.0, source});
    minScores[source] = 0.0;

    while (!pq.empty()) {
        auto [currentScore, currentId] = pq.top();
        pq.pop();

        if (currentId == destination) {
            break; // Found the shortest path
        }

        // If we found a shorter path already, skip
        if (currentScore > minScores[currentId]) {
            continue;
        }

        auto edges = graph.getOutgoingEdges(currentId);
        for (const auto& edge : edges) {
            if (!edge.isOperational) continue;

            double edgeScore = scorer.calculateScore(edge);
            double newScore = currentScore + edgeScore;

            if (minScores.find(edge.destinationId) == minScores.end() || newScore < minScores[edge.destinationId]) {
                minScores[edge.destinationId] = newScore;
                previousNode[edge.destinationId] = currentId;
                edgeToReach[edge.destinationId] = edge;
                pq.push({newScore, edge.destinationId});
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
