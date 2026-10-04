#include "Graph.h"
#include <algorithm>

namespace route_optimizer {

bool Graph::addNode(const Node& node) {
    std::unique_lock lock(mutex_);
    if (nodes_.find(node.id) != nodes_.end()) {
        return false; // Node already exists
    }
    nodes_[node.id] = node;
    adjacencyList_[node.id] = std::vector<Edge>();
    return true;
}

bool Graph::removeNode(const std::string& nodeId) {
    std::unique_lock lock(mutex_);
    if (nodes_.erase(nodeId) == 0) {
        return false;
    }
    adjacencyList_.erase(nodeId);
    // Remove edges pointing to this node
    for (auto& [src, edges] : adjacencyList_) {
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                           [&nodeId](const Edge& e) { return e.destinationId == nodeId; }),
            edges.end());
    }
    return true;
}

std::optional<Node> Graph::getNode(const std::string& nodeId) const {
    std::shared_lock lock(mutex_);
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool Graph::addEdge(const Edge& edge) {
    std::unique_lock lock(mutex_);
    if (nodes_.find(edge.sourceId) == nodes_.end() || nodes_.find(edge.destinationId) == nodes_.end()) {
        return false; // Source or destination node does not exist
    }

    auto& edges = adjacencyList_[edge.sourceId];
    // Check if edge already exists, update it if so
    for (auto& e : edges) {
        if (e.destinationId == edge.destinationId) {
            e = edge;
            return true;
        }
    }
    edges.push_back(edge);
    return true;
}

bool Graph::removeEdge(const std::string& sourceId, const std::string& destinationId) {
    std::unique_lock lock(mutex_);
    auto it = adjacencyList_.find(sourceId);
    if (it == adjacencyList_.end()) return false;
    
    auto& edges = it->second;
    auto initialSize = edges.size();
    edges.erase(
        std::remove_if(edges.begin(), edges.end(),
                       [&destinationId](const Edge& e) { return e.destinationId == destinationId; }),
        edges.end());
        
    return edges.size() < initialSize;
}

bool Graph::updateEdgeStatus(const std::string& sourceId, const std::string& destinationId, bool isOperational) {
    std::unique_lock lock(mutex_);
    auto it = adjacencyList_.find(sourceId);
    if (it == adjacencyList_.end()) return false;

    for (auto& e : it->second) {
        if (e.destinationId == destinationId) {
            e.isOperational = isOperational;
            return true;
        }
    }
    return false;
}

std::vector<Edge> Graph::getOutgoingEdges(const std::string& nodeId) const {
    std::shared_lock lock(mutex_);
    auto it = adjacencyList_.find(nodeId);
    if (it != adjacencyList_.end()) {
        return it->second;
    }
    return {};
}

size_t Graph::getNodeCount() const {
    std::shared_lock lock(mutex_);
    return nodes_.size();
}

size_t Graph::getEdgeCount() const {
    std::shared_lock lock(mutex_);
    size_t count = 0;
    for (const auto& [_, edges] : adjacencyList_) {
        count += edges.size();
    }
    return count;
}

} // namespace route_optimizer
