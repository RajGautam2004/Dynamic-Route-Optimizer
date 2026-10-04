#include <iostream>
#include <cassert>
#include "../include/Graph.h"
#include "../include/DijkstraRouter.h"
#include "../include/RouteScorer.h"

using namespace route_optimizer;

void testGraphOperations() {
    Graph graph;
    
    // Test Node Insertion
    assert(graph.addNode({"A", NodeType::WAREHOUSE, "Node A"}));
    assert(graph.addNode({"B", NodeType::DELIVERY_HUB, "Node B"}));
    assert(graph.getNodeCount() == 2);
    
    // Test Duplicate Node
    assert(!graph.addNode({"A", NodeType::WAREHOUSE, "Duplicate"}));
    
    // Test Edge Insertion
    assert(graph.addEdge({"A", "B", 10.0, 15.0, 100, 50, 1.0, 5.0, true}));
    assert(graph.getEdgeCount() == 1);
    
    // Test Disconnected Node Search
    assert(graph.addNode({"C", NodeType::CUSTOMER, "Node C"}));
    
    std::cout << "✅ Graph Operations Tests Passed" << std::endl;
}

void testDijkstraRouting() {
    Graph graph;
    graph.addNode({"FC1", NodeType::FULFILLMENT_CENTER, "FC1"});
    graph.addNode({"HUB1", NodeType::DELIVERY_HUB, "HUB1"});
    graph.addNode({"CUST1", NodeType::CUSTOMER, "CUST1"});
    
    graph.addEdge({"FC1", "HUB1", 10.0, 10.0, 100, 0, 1.0, 10.0, true});
    graph.addEdge({"HUB1", "CUST1", 5.0, 5.0, 100, 0, 1.0, 5.0, true});
    
    DijkstraRouter router;
    RouteScorer scorer;
    
    auto result = router.calculateRoute(graph, "FC1", "CUST1", scorer);
    assert(result.success);
    assert(result.path.size() == 3);
    assert(result.path[0] == "FC1");
    assert(result.path[2] == "CUST1");
    assert(result.totalDistance == 15.0);

    // Test Failure Path (Disconnected)
    auto failResult = router.calculateRoute(graph, "CUST1", "FC1", scorer);
    assert(!failResult.success);

    std::cout << "✅ Dijkstra Routing Tests Passed" << std::endl;
}

int main() {
    std::cout << "Running C++ Routing Engine Unit Tests..." << std::endl;
    testGraphOperations();
    testDijkstraRouting();
    std::cout << "All C++ tests passed successfully! 🎉" << std::endl;
    return 0;
}
