#include "httplib.h"
#include "json.hpp"
#include "Graph.h"
#include "DijkstraRouter.h"
#include "RouteScorer.h"
#include "RouteManager.h"
#include <iostream>
#include <memory>
#include <thread>

using json = nlohmann::json;
using namespace route_optimizer;

void startHttpServer(std::shared_ptr<Graph> graph, std::shared_ptr<RouteManager> routeManager) {
    httplib::Server svr;

    svr.Post("/api/v1/calculate", [graph](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            std::string source = body["source"];
            std::string destination = body["destination"];
            std::string algorithm = body.value("algorithm", "dijkstra");

            RouteScorer scorer;
            DijkstraRouter router;
            
            auto result = router.calculateRoute(*graph, source, destination, scorer);
            
            json response;
            if (result.success) {
                response["route"] = result.path;
                response["distance"] = result.totalDistance;
                response["estimatedTime"] = result.estimatedTime;
                response["cost"] = result.totalCost;
                response["algorithm"] = result.algorithmUsed;
                res.status = 200;
            } else {
                response["error"] = "No route found";
                res.status = 404;
            }
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    svr.Post("/api/v1/network/update", [graph, routeManager](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            std::string source = body["source"];
            std::string dest = body["destination"];
            bool isOperational = body["isOperational"];

            auto affected = routeManager->handleEdgeUpdate(source, dest, isOperational);
            
            json response;
            response["message"] = "Graph updated";
            response["affectedShipments"] = json::array();
            for (const auto& shipment : affected) {
                response["affectedShipments"].push_back({
                    {"shipmentId", shipment.shipmentId},
                    {"newRoute", shipment.currentRoute.path}
                });
            }
            
            res.status = 200;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    std::cout << "Starting C++ Routing Engine HTTP Service on port 8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);
}
