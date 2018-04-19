#pragma once

#include "add_monetizer_handler.h"
#include "init_handler.h"
#include "game_handler.h"
#include "parse_request.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unordered_map>
#include <string>
#include <vector>

class THttpServer {
    using THandler = std::string(*)(const TRequest&);
public:
    THttpServer() = default;

    THttpServer(int32_t port = 8080, int32_t backlog = 10);

    int32_t Accept(struct sockaddr* clientAddr, socklen_t* clientLen) const;

    void Handler(int32_t clientFd) const;

    void HandleQuery() const;

    void Process(int32_t clientFd) const;

    ~THttpServer();

private:
    int32_t SocketFd;
    int BacklogQueueLimit;
    struct sockaddr_in6 Addr;

    std::unordered_map<std::string, IBaseHandlerPtr> Handlers;
    TGameStatePtr State;

    static constexpr int MaxRequestSize = 1000;
};
