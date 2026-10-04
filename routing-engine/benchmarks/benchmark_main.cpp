#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include "../include/Graph.h"
#include "../include/DijkstraRouter.h"
#include "../include/AStarRouter.h"
#include "../include/RouteScorer.h"

using namespace route_optimizer;

Graph generateRandomGraph(int numNodes, int edgesPerNode) {
    Graph graph;
    
    // Create nodes
    for (int i = 0; i < numNodes; ++i) {
        std::string id = "NODE_" + std::to_string(i);
        graph.addNode({id, NodeType::DELIVERY_HUB, id});
    }

    // Create random edges
    std::mt19937 rng(42); // fixed seed for reproducibility
    std::uniform_int_distribution<int> nodeDist(0, numNodes - 1);
    std::uniform_real_distribution<double> weightDist(1.0, 100.0);

    for (int i = 0; i < numNodes; ++i) {
        std::string src = "NODE_" + std::to_string(i);
        for (int j = 0; j < edgesPerNode; ++j) {
            std::string dest = "NODE_" + std::to_string(nodeDist(rng));
            if (src != dest) {
                double distance = weightDist(rng);
                graph.addEdge({src, dest, distance, distance/2.0, 100, 0, 1.0, distance*1.5, true});
            }
        }
    }
    
    return graph;
}

void runBenchmark(int numNodes, int edgesPerNode) {
    std::cout << "----------------------------------------\n";
    std::cout << "Benchmarking Graph with " << numNodes << " Nodes and ~" << (numNodes * edgesPerNode) << " Edges\n";
    
    auto startGen = std::chrono::high_resolution_clock::now();
    Graph graph = generateRandomGraph(numNodes, edgesPerNode);
    auto endGen = std::chrono::high_resolution_clock::now();
    std::cout << "Graph Generation Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endGen - startGen).count() << " ms\n";

    RouteScorer scorer;
    DijkstraRouter dijkstra;
    
    // A* heuristic (dummy heuristic for random graph)
    auto heuristic = [](const std::string&, const std::string&) -> double { return 0.0; };
    AStarRouter astar(heuristic);

    std::string source = "NODE_0";
    std::string dest = "NODE_" + std::to_string(numNodes / 2); // Pick a middle node to search for

    // Dijkstra Benchmark
    auto startD = std::chrono::high_resolution_clock::now();
    auto resultD = dijkstra.calculateRoute(graph, source, dest, scorer);
    auto endD = std::chrono::high_resolution_clock::now();
    
    std::cout << "Dijkstra Computation Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endD - startD).count() << " ms\n";
    std::cout << "Dijkstra Success: " << (resultD.success ? "YES" : "NO") << " | Path Length: " << resultD.path.size() << "\n";

    // A* Benchmark
    auto startA = std::chrono::high_resolution_clock::now();
    auto resultA = astar.calculateRoute(graph, source, dest, scorer);
    auto endA = std::chrono::high_resolution_clock::now();
    
    std::cout << "A* Computation Time:       " << std::chrono::duration_cast<std::chrono::milliseconds>(endA - startA).count() << " ms\n";
    std::cout << "A* Success: " << (resultA.success ? "YES" : "NO") << " | Path Length: " << resultA.path.size() << "\n";
}

int main() {
    std::cout << "=== ROUTING ALGORITHM BENCHMARKS ===\n\n";
    
    runBenchmark(100, 5);      // Small
    runBenchmark(1000, 10);    // Medium
    runBenchmark(10000, 20);   // Large
    // runBenchmark(100000, 50); // Extra Large (Omitted by default to save execution time, but system supports it)

    std::cout << "\n====================================\n";
    return 0;
}
