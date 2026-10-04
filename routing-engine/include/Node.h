#pragma once

#include <string>

namespace route_optimizer {

enum class NodeType {
    WAREHOUSE,
    FULFILLMENT_CENTER,
    SORT_CENTER,
    DELIVERY_HUB,
    CUSTOMER
};

struct Node {
    std::string id;
    NodeType type;
    std::string locationName;

    bool operator==(const Node& other) const {
        return id == other.id;
    }
};

} // namespace route_optimizer
