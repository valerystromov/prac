#include "http_server.h"

#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: http_server [port]" << std::endl;
        return 0;
    }
    auto server = std::make_unique<THttpServer>(std::atol(argv[1]));

    while (true) {
        server->HandleQuery();
    }
    return 0;
}
