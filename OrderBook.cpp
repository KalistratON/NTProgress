#include "OrderBook.h"
#include "OrderBookImpl.h"


OrderBook::OrderBook() :
    myImpl (std::make_shared <OrderBookImpl>())
{
}

void OrderBookImpl::SubmitBuyRequest (const std::string& theClient, size_t theCount, double thePrice)
{
    myClientBuyRequests.emplace (thePrice, OrderRequest { theCount, theClient });
    Update();
}

void OrderBookImpl::SubmitSaleRequest (const std::string& theClient, size_t theCount, double thePrice)
{
    myClientSaleRequests.emplace (thePrice, OrderRequest { theCount, theClient });
    Update();
}

StockMarket OrderBookImpl::GetActiveRequests()
{
    return std::make_pair (myClientBuyRequests, myClientSaleRequests);
}

void OrderBookImpl::Update()
{
    if (!myClientBuyRequests.size() || !myClientSaleRequests.size()) {
        return;
    }

    while (myClientBuyRequests.size() && 
           myClientSaleRequests.size() && 
        myClientBuyRequests.begin()->first >= myClientSaleRequests.begin()->first) {

        auto aBuyIt = myClientBuyRequests.begin();
        auto aSaleIt = myClientSaleRequests.begin();
        auto& aBuyRequestCount = aBuyIt->second.myCount;
        auto& aSaleRequestCount = aSaleIt->second.myCount;

        double aDiffCount = aBuyRequestCount > aSaleRequestCount ? aSaleRequestCount : aBuyRequestCount;
        aSaleRequestCount -= aDiffCount;
        aBuyRequestCount -= aDiffCount;

        myRoster.UpdateBill (aBuyIt->second.myClientName, aDiffCount, -aBuyIt->first * aDiffCount);
        myRoster.UpdateBill (aSaleIt->second.myClientName, -aDiffCount, aBuyIt->first * aDiffCount);

        if (!aSaleRequestCount) {
            myClientSaleRequests.erase (aSaleIt);
        } else {
            myClientBuyRequests.erase (aBuyIt);
        }
    }
}

void OrderBook::SubmitBuyRequest (const std::string& theClient, size_t theCount, double thePrice)
{
    myImpl->SubmitBuyRequest (theClient, theCount, thePrice);
}

void OrderBook::SubmitSaleRequest (const std::string& theClient, size_t theCount, double thePrice)
{
    myImpl->SubmitSaleRequest (theClient, theCount, thePrice);
}

StockMarket OrderBook::GetActiveRequests()
{
    return myImpl->GetActiveRequests();
}

void OrderBook::Update()
{
    myImpl->Update();
}

Roster& OrderBook::GetRoster()
{
    return myImpl->myRoster;
}