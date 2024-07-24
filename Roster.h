#pragma once

#include <string>
#include <memory>
#include <map>

class RosterImpl;

struct OrderRequest {
    size_t myCount;
    std::string myClientName;
};

using StockMarket = std::pair <std::multimap<double, OrderRequest, std::greater<double>>, std::multimap<double, OrderRequest>>;

class Roster {

public:
    Roster();
    void UpdateBill (const std::string& theClientName, long theUSDDiff, long theRublesDiff);
    bool GetBill (const std::string& theClientName, long& theUSD, long& theRubles) const;

private:
    std::shared_ptr<RosterImpl> myImpl;
};