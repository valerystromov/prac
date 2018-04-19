#include "parse_request.h"

#include <iostream>
#include <sstream>

static std::string UrlDecode(const std::string& url) {
    std::string result;
    for (size_t idx = 0; idx < url.length(); idx++) {
        if (url[idx] != '%') {
            if (url[idx] == '+') {
                result += ' ';
            } else {
                result += url[idx];
            }
        } else {
            int ascii;
            sscanf(url.substr(idx + 1, 2).c_str(), "%x", &ascii);
            char sym = static_cast<char>(ascii);
            result += sym;
            idx += 2;
        }
    }
    return result;
}

TRequest::TRequest(TGameStatePtr state, const std::string& request)
    : State(state)
    , RawRequest(request)
{
    ParseRequest(RawRequest);
};

void TRequest::ParseRequest(std::string request) {
    ParseRequestType(request);
    ParseQueryArgs(request);
    ParseProtocolType(request);
    ParseHeaders(request);
};

void TRequest::ParseRequestType(std::string& request) {
    std::stringstream stream(request);
    std::string type;
    stream >> type;
    RequestType = type == "GET" ? ERequestType::Get : ERequestType::Post;
    request.erase(0, request.find('/'));
}

void TRequest::ParseQueryArgs(std::string& request) {
    std::stringstream stream(request);
    std::string url;
    stream >> url;
    if (url.find('?') == std::string::npos) {
        Handler = url.length() > 1 ? url.substr(1, url.length() - 1) : "";
        return;
    }

    Handler = url.substr(1, url.find('?') - 1);
    url.erase(0, request.find('?') + 1);

    while (url.find('=') != std::string::npos) {
        std::string key = url.substr(0, url.find('='));
        url.erase(0, url.find('=') + 1);
        std::string value = "";
        if (url.find('&') == std::string::npos) {
            value = url;
            url = "";
        } else {
            value = url.substr(0, url.find('&'));
            url.erase(0, url.find('&') + 1);
        }
        QueryArgs[key] = UrlDecode(value);
    }

    request.erase(0, request.find(' ') + 1);
}

void TRequest::ParseProtocolType(std::string& request) {
    Protocol = request.substr(0, request.find('\n'));
    request.erase(0, request.find('\n') + 1);
}

void TRequest::ParseHeaders(std::string& request) {
    while (request.find(':') != std::string::npos) {
        std::string key = request.substr(0, request.find(':'));
        request.erase(0, request.find(':') + 1);
        std::string value = request.substr(0, request.find('\n'));
        request.erase(0, request.find('\n') + 1);
        Headers[key] = UrlDecode(value);
    }
}

void TRequest::DebugOutput() const noexcept {
    std::cout << "Request type: " << static_cast<std::underlying_type_t<ERequestType>>(RequestType) << std::endl;
    std::cout << "Handler: " << Handler << std::endl;
    for (const auto& q : QueryArgs) {
        std::cout << q.first << " = " << q.second << std::endl;
    }
    std::cout << std::endl;
    for (const auto& h : Headers) {
        std::cout << h.first << " : " << h.second << std::endl;
    }
    std::cout << std::endl;
}

std::string TRequest::GetHandler() const noexcept {
    return Handler;
}

const std::map<std::string, std::string>& TRequest::GetQueryArgs() const noexcept {
    return QueryArgs;
}

const std::map<std::string, std::string>& TRequest::GetHeaders() const noexcept {
    return Headers;
}

std::string TRequest::GetRawRequest() const noexcept {
    return RawRequest;
}

TGameStatePtr TRequest::GetState() const noexcept {
    return State;
}
