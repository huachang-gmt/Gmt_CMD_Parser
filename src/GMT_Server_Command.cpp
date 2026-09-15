#include "command_parser.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <vector>

namespace
{

constexpr std::uint16_t SERVER_PORT = 9999;
constexpr int BACKLOG = 1;

} // namespace

struct ResponseRule
{
    const char* command;
    std::vector<const char*> responses;
    const char* default_response;
};

static const ResponseRule RESPONSE_RULES[] =
{
    {
        "INS",
        {
            ">Connected.\r\n",
            ">Connecting...\r\n",
            ">Connect fail.\r\n"
        },
        ">Connected.\r\n"
    },
    {
        "STP",
        {
            ">STP\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "SAH",
        {
            ">SAH\r\n",
            ">SAH ERR <ErrorCode>\r\n",
            ">homing end\r\n"
        },
        ">SAH\r\n"
    },
    {
        "SHC",
        {
            ">SHC\r\n",
            ">Done\r\n",
            ">SHC ERR [ErrorCode]\r\n"
        },
        ">Done\r\n"
    },
};


static const char* GetDefaultResponse(const char* command)
{
    for (const auto& rule : RESPONSE_RULES)
    {
        if (std::strcmp(rule.command, command) == 0)
        {
            return rule.default_response;
        }
    }

    return "DONE\r\n";
}


int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "GMT Server Command Test" << std::endl;
    std::cout << "========================================" << std::endl;

    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        std::cerr << "[ERROR] socket() failed: "
                  << std::strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "[OK] socket() created. fd="
              << server_fd << std::endl;

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(
            server_fd,
            reinterpret_cast<sockaddr*>(&server_addr),
            sizeof(server_addr)) < 0)
    {
        std::cerr << "[ERROR] bind() failed: "
                  << std::strerror(errno) << std::endl;

        close(server_fd);
        return 1;
    }

    std::cout << "[OK] bind() successful. port="
              << SERVER_PORT << std::endl;

    if (listen(server_fd, BACKLOG) < 0)
    {
        std::cerr << "[ERROR] listen() failed: "
                  << std::strerror(errno) << std::endl;

        close(server_fd);
        return 1;
    }

    std::cout << "[OK] listen() successful." << std::endl;
    std::cout << "[WAIT] Waiting for client connection..."
              << std::endl;

    const int client_fd = accept(
        server_fd,
        nullptr,
        nullptr);

    if (client_fd < 0)
    {
        std::cerr << "[ERROR] accept() failed: "
                  << std::strerror(errno) << std::endl;

        close(server_fd);
        return 1;
    }

    std::cout << "[OK] Client connected. fd="
              << client_fd << std::endl;

    char buffer[1024];


    CommandParser parser;

    while (true)
    {
        const ssize_t received = recv(
            client_fd,
            buffer,
            sizeof(buffer) - 1,
            0);

        if (received < 0)
        {
            std::cerr << "[ERROR] recv() failed: "
                    << std::strerror(errno) << std::endl;
            break;
        }

        if (received == 0)
        {
            std::cout << "[INFO] Client disconnected."
                    << std::endl;
            break;
        }

        buffer[received] = '\0';

        std::cout << "[RX] " << buffer;

        const ParseResult result = parser.parse(buffer);

        const char* response = nullptr;

        if (result.result == ParserResult::VALID)
        {
            response = GetDefaultResponse(result.command.c_str());
        }
        else
        {
            response = result.error.c_str();
        }

        const ssize_t sent = send(
            client_fd,
            response,
            std::strlen(response),
            0);

        if (sent < 0)
        {
            std::cerr << "[ERROR] send() failed: "
                    << std::strerror(errno) << std::endl;
            break;
        }

        std::cout << "[TX] " << response;
    }

    close(client_fd);
    close(server_fd);

    return 0;

}
