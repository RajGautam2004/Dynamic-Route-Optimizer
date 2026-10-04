#pragma once

#include "Router.h"
#include <functional>

namespace route_optimizer {

class AStarRouter : public Router {
public:
    // Heuristic function: heuristic(current_node_id, destination_node_id) -> estimated score
    using HeuristicFunction = std::function<double(const std::string&, const std::string&)>;

    explicit AStarRouter(HeuristicFunction heuristic);

    RouteResult calculateRoute(
        const Graph& graph, 
        const std::string& source, 
        const std::string& destination, 
        const RouteScorer& scorer) override;

private:
    HeuristicFunction heuristic_;
};

} // namespace route_optimizer
