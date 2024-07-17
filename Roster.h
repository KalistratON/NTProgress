#pragma once

#include <string>
#include <memory>

class RosterImpl;

class Roster {

public:
    Roster();
    void UpdateBill (const std::string& theClientName, long theUSDDiff, long theRublesDiff);
    bool GetBill (const std::string& theClientName, long& theUSD, long& theRubles) const;

private:
    std::shared_ptr<RosterImpl> myImpl;
};