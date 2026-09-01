#include "tcp_server.h"

#include <iostream>

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "GMT Command Parser" << std::endl;
    std::cout << "========================================" << std::endl;

    TcpServer server;

    if (!server.start())
    {
        std::cerr << "[ERROR] Failed to start TCP server." << std::endl;
        return 1;
    }

    std::cout << "[OK] TCP server started." << std::endl;

    server.run();

    return 0;
}