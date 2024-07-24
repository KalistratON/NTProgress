#include "json.hpp"
#include "Common.hpp"
#include "OrderBook.h"
#include "Roster.h"

#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>


using boost::asio::ip::tcp;

class Core
{
public:
    std::string RegisterNewUser (const std::string& theUserName)
    {
        size_t newUserId = mUsers.size();
        mUsers[newUserId] = theUserName;
        myOrderBook.GetRoster().UpdateBill (theUserName, 0, 0);
        return std::to_string (newUserId);
    }

    std::string GetUserName (const std::string& aUserId)
    {
        const auto userIt = mUsers.find (std::stoi (aUserId));
        if (userIt == mUsers.cend()) {
            return "Error! Unknown User";
        } else {
            return userIt->second;
        }
    }

    void SubmitBuyRequest (const std::string& theClient, const std::string& theCount, const std::string& thePrice)
    {
        myOrderBook.SubmitBuyRequest (theClient, std::stoull (theCount), std::stod (thePrice));
    }

    void SubmitSaleRequest (const std::string& theClient, const std::string& theCount, const std::string& thePrice)
    {
        myOrderBook.SubmitSaleRequest (theClient, std::stoull (theCount), std::stod (thePrice));
    }

    std::string GetBill (const std::string& theClient)
    {
        long anUSD, aRubles;
        if (!myOrderBook.GetRoster().GetBill(theClient, anUSD, aRubles)) {
            return "Client " + theClient + " was not found!\n";
        }

        return std::to_string(anUSD) + ' ' + std::to_string(aRubles) + '\n';
    }

    std::string GetActiveRequests()
    {
        std::stringstream aStream;

        auto anActiveRequests = myOrderBook.GetActiveRequests();

        aStream << "---------------------------------------\n";
        aStream << "Buy requests\n";
        aStream << "---------------------------------------\n";
        const auto& anActiveBuyRequests = anActiveRequests.first;
        std::for_each (anActiveBuyRequests.crbegin(), anActiveBuyRequests.crend(), [&](const auto& aRequest) {
            aStream << aRequest.first << std::setw (10) 
                    << aRequest.second.myCount << std::setw (10) 
                    << aRequest.second.myClientName << std::setw (10) << '\n';
        });

        aStream << "---------------------------------------\n";
        aStream << "---------------------------------------\n";

        const auto& anActiveSaleRequests = anActiveRequests.second;
        std::for_each (anActiveSaleRequests.cbegin(), anActiveSaleRequests.cend(), [&](const auto& aRequest) {
            aStream << aRequest.first << std::setw (10) 
                    << aRequest.second.myCount << std::setw (10) 
                    << aRequest.second.myClientName << std::setw (10) << '\n';
        });
        aStream << "---------------------------------------\n";
        aStream << "Sale requests\n";
        aStream << "---------------------------------------\n";
        return aStream.str();
    }

private:
    std::map<size_t, std::string> mUsers;
    OrderBook myOrderBook;
};

static Core& GetCore()
{
    static Core core;
    return core;
}

class Session
{
public:
    Session (boost::asio::io_service& io_service)
        : socket_ (io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some (boost::asio::buffer (data_, max_length),
            boost::bind (&Session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handle_read (const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error) {
            data_[bytes_transferred] = '\0';

            auto j = nlohmann::json::parse (data_);
            auto reqType = j["ReqType"];

            std::string reply = "Error! Unknown request type";
            if (reqType == Requests::Registration) {
                reply = GetCore().RegisterNewUser (j["Message"]);
            } else if (reqType == Requests::Buy) {
                GetCore().SubmitBuyRequest (GetCore().GetUserNameA (j["UserId"]), j["Count"], j["Price"]);
                reply.clear();
            } else if (reqType == Requests::Sale) {
                GetCore().SubmitSaleRequest (GetCore().GetUserNameA (j["UserId"]), j["Count"], j["Price"]);
                reply.clear();
            } else if (reqType == Requests::Bill) {
                std::string aName = GetCore().GetUserNameA (j["UserId"]);
                reply = GetCore().GetBill (aName);
            } else if (reqType == Requests::ActiveRequests) {
                reply = GetCore().GetActiveRequests();
            }

            boost::asio::async_write (socket_,
                boost::asio::buffer (reply.c_str(), reply.size()),
                boost::bind (&Session::handle_write, this,
                    boost::asio::placeholders::error));
        } else {
            delete this;
        }
    }

    void handle_write (const boost::system::error_code& error)
    {
        if (!error) {
            socket_.async_read_some (boost::asio::buffer (data_, max_length),
                boost::bind (&Session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        } else {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class Server
{
public:
    Server (boost::asio::io_service& io_service)
        : io_service_ (io_service),
        acceptor_ (io_service, tcp::endpoint (tcp::v4(), port))
    {
        std::cout << "Server started! Listen " << port << " port" << std::endl;

        Session* new_session = new Session (io_service_);
        acceptor_.async_accept (new_session->socket(),
            boost::bind (&Server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    }

    void handle_accept (Session* new_Session,
        const boost::system::error_code& error)
    {
        if (!error) {
            new_Session->start();
            new_Session = new Session (io_service_);
            acceptor_.async_accept (new_Session->socket(),
                boost::bind (&Server::handle_accept, this, new_Session,
                    boost::asio::placeholders::error));
        } else {
            delete new_Session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        static Core core;

        Server server (io_service);

        io_service.run();
    }
    catch (std::exception& theException)
    {
        std::cerr << "Exception: " << theException.what() << "\n";
    }

    return 0;
}