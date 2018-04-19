#pragma once

#include "parse_request.h"

#include <memory>
#include <string>

class IBaseHandler {
public:
    virtual std::string HandleRequest(const TRequest& req) const noexcept = 0;
};

using IBaseHandlerPtr = std::unique_ptr<IBaseHandler>;
