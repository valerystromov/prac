#pragma once

#include "base_handler.h"

class TInitHandler final: public IBaseHandler {
public:
    std::string HandleRequest(const TRequest& req) const noexcept override;
};
