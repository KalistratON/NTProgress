#pragma once

#include "Roster.h"

#include <string>
#include <memory>


class OrderBookImpl;

class OrderBook {

public:
    OrderBook();
    void SubmitBuyRequest (const std::string& theClient, size_t theCount, double thePrice);
    void SubmitSaleRequest (const std::string& theClient, size_t theCount, double thePrice);
    Roster& GetRoster();
    void Update();

private:
    std::shared_ptr<OrderBookImpl> myImpl;
};