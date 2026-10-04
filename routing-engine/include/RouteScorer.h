#pragma once

#include "Edge.h"

namespace route_optimizer {

struct RouteScoringWeights {
    double timeWeight = 1.0;
    double distanceWeight = 1.0;
    double congestionWeight = 1.0;
    double costWeight = 1.0;
};

class RouteScorer {
public:
    RouteScorer(const RouteScoringWeights& weights = RouteScoringWeights{}) : weights_(weights) {}
    
    // Calculates a blended score for traversing an edge based on configurable weights.
    // Lower score is better.
    double calculateScore(const Edge& edge) const {
        if (!edge.isOperational) {
            return 1e9; // Essentially infinity if not operational
        }
        
        return (edge.travelTime * weights_.timeWeight) +
               (edge.distance * weights_.distanceWeight) +
               (edge.congestionFactor * weights_.congestionWeight) +
               (edge.cost * weights_.costWeight);
    }

private:
    RouteScoringWeights weights_;
};

} // namespace route_optimizer
