#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

static short port = 5555;

namespace Requests
{
    static std::string Registration = "Reg";
    static std::string Buy          = "Buy";
    static std::string Sale         = "Sale";
    static std::string Bill         = "Bill";
}

#endif //CLIENSERVERECN_COMMON_HPP
