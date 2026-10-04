#include <iostream>
#include <memory>
#include "Graph.h"
#include "RouteManager.h"
#include "DijkstraRouter.h"
#include "RouteScorer.h"

using namespace route_optimizer;

// Forward declaration
void startHttpServer(std::shared_ptr<Graph> graph, std::shared_ptr<RouteManager> routeManager);

int main() {
    std::cout << "Dynamic Route Optimizer - Routing Engine Started" << std::endl;

    auto graph = std::make_shared<Graph>();
    
    // Seed some basic data
    graph->addNode({"FC_01", NodeType::FULFILLMENT_CENTER, "Seattle FC"});
    graph->addNode({"HUB_01", NodeType::DELIVERY_HUB, "Bellevue Hub"});
    graph->addNode({"CUSTOMER_01", NodeType::CUSTOMER, "Redmond Customer"});
    
    graph->addEdge({"FC_01", "HUB_01", 15.0, 20.0, 1000.0, 500.0, 1.0, 5.0, true});
    graph->addEdge({"HUB_01", "CUSTOMER_01", 10.0, 15.0, 100.0, 50.0, 1.2, 8.0, true});
    
    auto primaryRouter = std::make_shared<DijkstraRouter>();
    RouteScorer scorer;
    auto routeManager = std::make_shared<RouteManager>(graph, primaryRouter, scorer);

    // Start the REST API blocking loop
    startHttpServer(graph, routeManager);

    return 0;
}
