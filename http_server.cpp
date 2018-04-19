#include "http_server.h"

#include <fstream>
#include <sstream>
#include <thread>

THttpServer::THttpServer(int32_t port, int32_t backlog)
    : BacklogQueueLimit(backlog)
{
    SocketFd = socket(AF_INET6, SOCK_STREAM, 0);
    if (SocketFd < 0) {
        throw std::runtime_error("Can't create socket.");
    }
    int32_t optOk = 1;
    setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &optOk, sizeof(int32_t));
    bzero(reinterpret_cast<char *>(&Addr), sizeof(Addr));
    Addr.sin6_flowinfo = 0;
    Addr.sin6_family = AF_INET6;
    Addr.sin6_addr = in6addr_any;
    Addr.sin6_port = htons(port);
    if (bind(SocketFd, reinterpret_cast<struct sockaddr *>(&Addr), sizeof(Addr)) < 0) {
        throw std::runtime_error("Binding error");
    }
    listen(SocketFd, BacklogQueueLimit);

    Handlers.emplace("", std::make_unique<TInitHandler>());
    Handlers.emplace("add", std::make_unique<TAddMonetizerHandler>());
    Handlers.emplace("game", std::make_unique<TGameHandler>());

    State = std::make_shared<TGameState>();
}

int32_t THttpServer::Accept(struct sockaddr* clientAddr, socklen_t* clientLen) const {
    return accept(SocketFd, clientAddr, clientLen);
}

void THttpServer::Handler(int clientFd) const {
    char buffer[MaxRequestSize];
    size_t len = recv(clientFd, buffer, MaxRequestSize, 0);
    buffer[len] = '\0';

    TRequest req(State, {buffer, len + 1});
    const auto& currentHandler = req.GetHandler();
    std::string response;
    if (Handlers.find(currentHandler) == Handlers.end()) {
        response = "ERROR! No such handler.\n";
    } else {
        response = Handlers.at(currentHandler)->HandleRequest(req);
    }

    send(clientFd, response.c_str(), response.length(), 0);
    close(clientFd);
}

void THttpServer::Process(int clientFd) const {
    auto thr = std::thread(&THttpServer::Handler, this, clientFd);
    thr.detach();
}

void THttpServer::HandleQuery() const {
    struct sockaddr_in6 clientAddr;
    socklen_t clientLen;
    int clientSocketFd = Accept((struct sockaddr *)&clientAddr, &clientLen);
    if (clientSocketFd < 0) {
        throw std::runtime_error("Accept error");
    }

    Process(clientSocketFd);
}

THttpServer::~THttpServer() {
    close(SocketFd);
}
