#pragma once

#include "Node.h"
#include "Edge.h"
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <optional>

namespace route_optimizer {

class Graph {
public:
    Graph() = default;

    // Node operations
    bool addNode(const Node& node);
    bool removeNode(const std::string& nodeId);
    std::optional<Node> getNode(const std::string& nodeId) const;

    // Edge operations
    bool addEdge(const Edge& edge);
    bool removeEdge(const std::string& sourceId, const std::string& destinationId);
    bool updateEdgeStatus(const std::string& sourceId, const std::string& destinationId, bool isOperational);
    std::vector<Edge> getOutgoingEdges(const std::string& nodeId) const;

    // Graph info
    size_t getNodeCount() const;
    size_t getEdgeCount() const;

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, Node> nodes_;
    
    // Adjacency list: sourceId -> vector of edges
    std::unordered_map<std::string, std::vector<Edge>> adjacencyList_;
};

} // namespace route_optimizer
