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

static std::string ProcessRegistration (tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    SendMessage (aSocket, "0", Requests::Registration, name);
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

        std::string my_id = ProcessRegistration (aSocket);

        while (true)
        {
            std::cout <<    "Menu:\n"
                            "1) Buy Request\n"
                            "2) Sale Request\n"
                            "3) Bill Request\n"
                            "0) Exit\n"
                         << std::endl;

            short menu_option_num;
            std::cin >> menu_option_num;
            switch (menu_option_num)
            {
                case 1:
                {
                    {
                        std::string aPrice, aCount;
                        std::cout << "Enter a price of USD : " << std::endl;
                        std::cin >> aPrice;
                        std::cout << "Enter a count of USDs : " << std::endl;
                        std::cin >> aCount;
                        SendMessage (aSocket, my_id, Requests::Buy, aPrice, aCount);
                    }

                    std::cout << ReadMessage (aSocket);
                    break;
                }
                case 2:
                {
                    {
                        std::string aPrice, aCount;
                        std::cout << "Enter a price of USD : " << std::endl;
                        std::cin >> aPrice;
                        std::cout << "Enter a count of USDs : " << std::endl;
                        std::cin >> aCount;
                        SendMessage (aSocket, my_id, Requests::Sale, aPrice, aCount);
                    }

                    std::cout << ReadMessage (aSocket);
                    break;
                }
                case 3:
                {
                    SendMessage (aSocket, my_id, Requests::Bill, "");
                    std::cout << ReadMessage (aSocket);
                    break;
                }
                case 0:
                {
                    exit(0);
                    break;
                }
                default:
                {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    }
    catch (std::exception& theException)
    {
        std::cerr << "Exception: " << theException.what() << "\n";
    }

    return 0;
}