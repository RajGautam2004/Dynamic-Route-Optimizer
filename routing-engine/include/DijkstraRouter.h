#pragma once

#include "Router.h"

namespace route_optimizer {

class DijkstraRouter : public Router {
public:
    RouteResult calculateRoute(
        const Graph& graph, 
        const std::string& source, 
        const std::string& destination, 
        const RouteScorer& scorer) override;
};

} // namespace route_optimizer
