#include "add_monetizer_handler.h"

#include <iostream>
#include <sstream>

std::string TAddMonetizerHandler::HandleRequest(const TRequest& req) const noexcept {
    auto state = req.GetState();
    const auto& args = req.GetQueryArgs();
    state->AddMonetizer(args.at("invest-name"), args.at("invest-type"), args.at("invest-profit"), args.at("invest-risk"));
    std::stringstream response;
    response
        << "HTTP/1.1 302 Found\r\n"
        << "Version: HTTP/1.1\r\n"
        << "Location: http://bs-dev08.search.yandex.net:9090\r\n"
        << "\r\n\r\n";

    return response.str();
}
