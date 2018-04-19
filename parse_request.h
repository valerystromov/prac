#pragma once

#include "state.h"

#include <map>
#include <string>
#include <vector>

class TRequest {
    enum class ERequestType : int8_t {
        Get,
        Post
    };
public:
    TRequest(TGameStatePtr state, const std::string& request);

    void ParseRequest(std::string request);
    void ParseRequestType(std::string& request);
    void ParseQueryArgs(std::string& request);
    void ParseProtocolType(std::string& request);
    void ParseHeaders(std::string& request);

    void DebugOutput() const noexcept;

    std::string GetRawRequest() const noexcept;
    std::string GetHandler() const noexcept;
    const std::map<std::string, std::string>& GetQueryArgs() const noexcept;
    const std::map<std::string, std::string>& GetHeaders() const noexcept;
    TGameStatePtr GetState() const noexcept;

private:
    TGameStatePtr State;
    std::string RawRequest;

    ERequestType RequestType;
    std::string Handler;
    std::string Protocol;
    std::map<std::string, std::string> Headers;
    std::map<std::string, std::string> QueryArgs;
};
