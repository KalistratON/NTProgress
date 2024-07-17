CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(ClienServerEcn)

FIND_PACKAGE(Boost 1.40 REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

SET(CMAKE_CXX_STANDARD 17)

ADD_EXECUTABLE(Server OrderBookImpl.h OrderBook.h OrderBook.cpp RosterImpl.h Roster.h Roster.cpp Server.cpp Common.hpp json.hpp)
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(Client Client.cpp Common.hpp json.hpp)
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(TestLib Common.hpp json.hpp TestLib.cpp)
TARGET_LINK_LIBRARIES(TestLib PRIVATE Threads::Threads ${Boost_LIBRARIES})