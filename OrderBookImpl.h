#pragma once

#include "Roster.h"

#include <unordered_map>
#include <map>
#include <memory>
#include <string>


class OrderBookImpl {

    struct OrderRequest {
        size_t myCount;
        std::string myClientName;
    };

public:
    void SubmitBuyRequest (const std::string& theClient, size_t theCount, double thePrice);
    void SubmitSaleRequest (const std::string& theClient, size_t theCount, double thePrice);
    void Update();

    Roster myRoster;

private:
    std::multimap<double, OrderRequest, std::greater<double>> myClientBuyRequests;
    std::multimap<double, OrderRequest> myClientSaleRequests;
    std::unordered_map<std::string, double> myClientInfo;
};