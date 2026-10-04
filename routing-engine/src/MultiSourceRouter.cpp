#include "MultiSourceRouter.h"
#include <queue>
#include <unordered_map>
#include <algorithm>

namespace route_optimizer {

RouteResult MultiSourceRouter::calculateBestSourceRoute(
    const Graph& graph, 
    const std::vector<std::string>& sources, 
    const std::string& destination, 
    const RouteScorer& scorer) 
{
    RouteResult result;
    result.algorithmUsed = "Multi-Source Dijkstra";
    result.success = false;

    if (!graph.getNode(destination) || sources.empty()) {
        return result; 
    }

    using P = std::pair<double, std::string>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    
    std::unordered_map<std::string, double> minScores;
    std::unordered_map<std::string, std::string> previousNode;
    std::unordered_map<std::string, Edge> edgeToReach;
    std::unordered_map<std::string, std::string> originSource; // Tracks which source this path started from

    for (const auto& source : sources) {
        if (graph.getNode(source)) {
            pq.push({0.0, source});
            minScores[source] = 0.0;
            originSource[source] = source;
        }
    }

    while (!pq.empty()) {
        auto [currentScore, currentId] = pq.top();
        pq.pop();

        if (currentId == destination) {
            break; // Found the shortest path to destination from ONE of the sources
        }

        if (currentScore > minScores[currentId]) continue;

        auto edges = graph.getOutgoingEdges(currentId);
        for (const auto& edge : edges) {
            if (!edge.isOperational) continue;

            double newScore = currentScore + scorer.calculateScore(edge);

            if (minScores.find(edge.destinationId) == minScores.end() || newScore < minScores[edge.destinationId]) {
                minScores[edge.destinationId] = newScore;
                previousNode[edge.destinationId] = currentId;
                edgeToReach[edge.destinationId] = edge;
                originSource[edge.destinationId] = originSource[currentId];
                pq.push({newScore, edge.destinationId});
            }
        }
    }

    if (previousNode.find(destination) == previousNode.end()) {
        return result; 
    }

    std::string current = destination;
    std::string bestSource = originSource[destination];
    
    while (current != bestSource) {
        result.path.push_back(current);
        const Edge& e = edgeToReach[current];
        result.totalDistance += e.distance;
        result.estimatedTime += e.travelTime;
        result.totalCost += e.cost;
        current = previousNode[current];
    }
    result.path.push_back(bestSource);
    std::reverse(result.path.begin(), result.path.end());
    result.success = true;

    return result;
}

} // namespace route_optimizer
