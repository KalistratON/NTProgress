#include "Roster.h"
#include "RosterImpl.h"


void RosterImpl::UpdateBill (const std::string& theClientName, long theUSDDiff, long theRublesDiff)
{
    if (!myClientTable.insert (std::make_pair (theClientName, Bill { theUSDDiff, theRublesDiff })).second) {
        myClientTable [theClientName].myUSD += theUSDDiff;
        myClientTable [theClientName].myRubles += theRublesDiff;
    }
}

bool RosterImpl::GetBill (const std::string& theClientName, long& theUSD, long& theRubles) const
{
    auto anIt = myClientTable.find (theClientName);
    if (anIt == myClientTable.end()) {
        return false;
    }

    theUSD = anIt->second.myUSD;
    theRubles = anIt->second.myRubles;
    return true;
}

Roster::Roster() :
    myImpl (std::make_shared <RosterImpl>())
{
}

void Roster::UpdateBill (const std::string& theClientName, long theUSDDiff, long theRublesDiff)
{
    myImpl->UpdateBill (theClientName, theUSDDiff, theRublesDiff);
}

bool Roster::GetBill (const std::string& theClientName, long& theUSD, long& theRubles) const
{
    return myImpl->GetBill (theClientName, theUSD, theRubles);
}