#include "Common.hpp"
#include "json.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <iostream>


using boost::asio::ip::tcp;

static void SendMessage (
    tcp::socket& theSocket,
    const std::string& theId,
    const std::string& theRequestType,
    const std::string& theMessage)
{
    nlohmann::json aReq;
    aReq["UserId"] = theId;
    aReq["ReqType"] = theRequestType;
    aReq["Message"] = theMessage;

    std::string aRequest = aReq.dump();
    boost::asio::write (theSocket, boost::asio::buffer (aRequest, aRequest.size()));
}

static void SendMessage (
    tcp::socket& theSocket,
    const std::string& theId,
    const std::string& theRequestType,
    const std::string& thePrice,
    const std::string& theCount)
{
    nlohmann::json aReq;
    aReq["UserId"] = theId;
    aReq["ReqType"] = theRequestType;
    aReq["Price"] = thePrice;
    aReq["Count"] = theCount;

    std::string aRequest = aReq.dump();
    boost::asio::write (theSocket, boost::asio::buffer (aRequest, aRequest.size()));
}

static std::string ReadMessage (tcp::socket& theSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until (theSocket, b, "\0");
    std::istream is (&b);
    std::string line (std::istreambuf_iterator<char> (is), {});
    return line;
}

static std::string ProcessRegistration (tcp::socket& aSocket, const std::string& theName)
{
    SendMessage (aSocket, "0", Requests::Registration, theName);
    return ReadMessage(aSocket);
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver (io_service);
        tcp::resolver::query query (tcp::v4(), "127.0.0.1", std::to_string (port));
        tcp::resolver::iterator iterator = resolver.resolve (query);

        tcp::socket aSocket (io_service);
        aSocket.connect (*iterator);

        std::string anUser_1 = ProcessRegistration (aSocket, "User_1");
        std::string anUser_2 = ProcessRegistration (aSocket, "User_2");
        std::string anUser_3 = ProcessRegistration (aSocket, "User_3");
        std::string anUser_4 = ProcessRegistration (aSocket, "User_4");
        std::string anUser_5 = ProcessRegistration (aSocket, "User_5");
        std::string anUser_6 = ProcessRegistration (aSocket, "User_6");
        std::string anUser_7 = ProcessRegistration (aSocket, "User_7");

        // buy request processing
        {
            size_t i = 1;
            std::vector<std::array <std::string, 2>> aPricesAndCounts 
                { { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" }, { "5", "5" }, { "6", "6" }, { "7", "7" }};

            for (const auto& [aPrice, aCount] : aPricesAndCounts) {
                SendMessage (aSocket, anUser_1, Requests::Buy, aPrice, aCount);
                SendMessage (aSocket, anUser_1, Requests::Bill, "");

                std::string aResponse = ReadMessage (aSocket);

                assert (aResponse == "0 0" && "wrong buy request processing test #" + i);
                ++i;
            }
        }

        // sale request processing
        {
            size_t i = 1;
            std::vector<std::array <std::string, 2>> aPricesAndCounts 
                { { "1000", "1" }, { "1001", "2" }, { "1002", "3" }, { "1003", "4" }, { "1004", "5" }, { "1005", "6" }, { "1006", "7" }};

            for (const auto& [aPrice, aCount] : aPricesAndCounts) {
                SendMessage (aSocket, anUser_2, Requests::Sale, aPrice, aCount);
                SendMessage (aSocket, anUser_2, Requests::Bill, "");

                std::string aResponse = ReadMessage (aSocket);

                assert (aResponse == "0 0" && "wrong sale request processing test #" + i);
                ++i;
            }
        }

        // partial execution processing
        {
            std::vector<std::array <std::string, 2>> aBuyPricesAndCounts 
                { { "150", "10" }, { "200", "30" } };
            std::vector<std::array <std::string, 2>> aSalePricesAndCounts 
                { { "140", "60" } };

            for (const auto& [aPrice, aCount] : aBuyPricesAndCounts) {
                SendMessage (aSocket, anUser_3, Requests::Buy, aPrice, aCount);
            }

            const auto& aPrice = aSalePricesAndCounts [0][0];
            const auto& aCount = aSalePricesAndCounts [0][1];
            SendMessage (aSocket, anUser_4, Requests::Sale, aPrice, aCount);

            SendMessage (aSocket, anUser_3, Requests::Bill, "");
            std::string aResponse = ReadMessage (aSocket);
            assert (aResponse == "40 -7500" && "wrong partial execution processing test (buy) #1");

            SendMessage (aSocket, anUser_4, Requests::Bill, "");
            aResponse = ReadMessage (aSocket);
            assert(aResponse == "-40 7500" && "wrong partial execution processing test (sale) #1");
        }

        // execution with multiple applications
        {
            std::vector<std::array <std::string, 2>> aBuyPricesAndCounts 
                { { "130", "30" }, { "130", "30" } };
            std::vector<std::array <std::string, 2>> aSalePricesAndCounts 
                { { "120", "50" } };


            SendMessage (aSocket, anUser_5, Requests::Buy, aBuyPricesAndCounts[0][0], aBuyPricesAndCounts[0][1]);
            SendMessage (aSocket, anUser_6, Requests::Buy, aBuyPricesAndCounts[1][0], aBuyPricesAndCounts[1][1]);
            SendMessage (aSocket, anUser_7, Requests::Sale, aSalePricesAndCounts [0][0], aSalePricesAndCounts [0][1]);


            SendMessage (aSocket, anUser_5, Requests::Bill, "");
            std::string aResponse = ReadMessage (aSocket);
            assert (aResponse == "30 -3900" && "wrong execution with multiple applications test (buy_1) #1");

            SendMessage (aSocket, anUser_6, Requests::Bill, "");
            aResponse = ReadMessage (aSocket);
            assert (aResponse == "20 -2600" && "wrong execution with multiple applications test (buy_2) #1");

            SendMessage (aSocket, anUser_7, Requests::Bill, "");
            aResponse = ReadMessage (aSocket);
            assert (aResponse == "-50 6500" && "wrong execution with multiple applications test (sale) #1");
        }
    }
    catch (std::exception& theException)
    {
        std::cerr << "Exception: " << theException.what() << "\n";
    }

    return 0;
}