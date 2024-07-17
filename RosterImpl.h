#pragma once

#include <unordered_map>
#include <string>

class RosterImpl {

    struct Bill {
        long myUSD;
        long myRubles;
    };

public:
    void UpdateBill (const std::string& theClientName, long theUSDDiff, long theRublesDiff);
    bool GetBill (const std::string& theClientName, long& theUSD, long& theRubles) const;

private:
    std::unordered_map<std::string, Bill> myClientTable;
};