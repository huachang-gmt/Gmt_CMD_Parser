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
    {
        "SHC?",
        {
            ">SHC? M01 1 2 17 400000.0 40000 0 5\r\n"
        },
        ">SHC? M01 1 2 17 400000.0 40000 0 5\r\n"
    },
    {
        "SVO",
        {
            ">SVO\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "SVF",
        {
            ">SVF\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "CAL",
        {
            ">CAL\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "ERR?",
        {
            ">ERR : M1 2048\r\n",
            ">ERR : M5 8192\r\n",
            ">No Error\r\n"
        },
        ">No Error\r\n"
    },
    {
        "DSC",
        {
            ">Disconnected\r\n"
        },
        ">Disconnected\r\n"
    },
    {
        "VLS",
        {
            ">VLS <value>\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "VLS?",
        {
            ">VLS? <value>\r\n"
        },
        ">VLS? <value>\r\n"
    },
    {
        "MOV",
        {
            ">MOV\r\n",
            ">MOV ERR [error code]\r\n",
            ">Excute Result : NORMAL FINISHED\r\n",
            ">Excute Result : ERR STOP\r\n",
            ">Excute Result : ERR NOT ON TARGET\r\n",
            ">Excute Result : ERR\r\n",
            ">Excute Result : ERR OVER GAP SENSOR THRSHOLD\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "MRV",
        {
            ">MRV\r\n",
            ">MRV ERR [error code]\r\n",
            ">Excute Result : NORMAL FINISHED\r\n",
            ">Excute Result : ERR STOP\r\n",
            ">Excute Result : ERR NOT ON TARGET\r\n",
            ">Excute Result : ERR\r\n",
            ">Excute Result : ERR OVER GAP SENSOR THRESHOLD\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "MSV",
        {
            ">MSV\r\n",
            ">MSV ERR [error code]\r\n",
            ">Excute Result : NORMAL FINISHED\r\n",
            ">Excute Result : ERR STOP\r\n",
            ">Excute Result : ERR NOT ON TARGET\r\n",
            ">Excute Result : ERR\r\n",
            ">Excute Result : ERR OVER GAP SENSOR THRESHOLD\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "MSR",
        {
            ">MSR\r\n",
            ">MSR ERR [error code]\r\n",
            ">Excute Result : NORMAL FINISHED\r\n",
            ">Excute Result : ERR STOP\r\n",
            ">Excute Result : ERR NOT ON TARGET\r\n",
            ">Excute Result : ERR\r\n",
            ">Excute Result : ERR OVER GAP SENSOR THRESHOLD\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "MPV",
        {
            ">MPV\r\n",
            ">MPV ERR [ErrorCode]\r\n",
            ">Excute Result : NORMAL FINISHED\r\n",
            ">Excute Result : ERR STOP\r\n",
            ">Excute Result : ERR NOT ON TARGET\r\n",
            ">Excute Result : ERR\r\n",
            ">Excute Result : ERR OVER GAP SENSOR THRESHOLD\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "MPR",
        {
            ">MPR\r\n",
            ">MPR ERR [ErrorCode]\r\n",
            ">Excute Result : NORMAL FINISHED\r\n",
            ">Excute Result : ERR STOP\r\n",
            ">Excute Result : ERR NOT ON TARGET\r\n",
            ">Excute Result : ERR\r\n",
            ">Excute Result : ERR OVER GAP SENSOR THRESHOLD\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "MOV?",
        {
            ">MOV? <status>\r\n",
            ">MOV? 1\r\n",
            ">MOV? 0\r\n"
        },
        ">MOV? 1\r\n"
    },
    {
        "POS?",
        {
            ">POS? <R1 R2 R3 R4 R5 R6>\r\n"
        },
        ">POS? <R1 R2 R3 R4 R5 R6>\r\n"
    },
    {
        "PMS?",
        {
            ">PMS? <R1 R2 R3 R4 R5 R6>\r\n"
        },
        ">PMS? <R1 R2 R3 R4 R5 R6>\r\n"
    },
    {
        "SPI",
        {
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "SPI?",
        {
            ">SPI? R U X Y Z\r\n",
            ">SPI? R V X Y Z\r\n",
            ">SPI? R W X Y Z\r\n",
            ">SPI? L U X Y Z\r\n",
            ">SPI? L V X Y Z\r\n",
            ">SPI? L W X Y Z\r\n"
        },
        ">SPI? R U X Y Z\r\n"
    },
    {
        "FRS",
        {
            ">FRS\r\n",
            ">FRS ERR\r\n",
            ">Excute Result : OVER ADC THRESHOLD\r\n",
            ">Excute Result : NORMAL FINISHED MOVE TO REGION MAX\r\n",
            ">Excute Result : UNDER BKN MOVE TO START POSITION\r\n",
            ">Excute Result : ERR STOP\r\n",
            ">Excute Result : ERR NOT ON TARGET\r\n",
            ">Excute Result : ERR\r\n",
            ">Excute Result : ERR OVER GAP SENSOR THRESHOLD\r\n",
            ">Done\r\n"
        },
        ">Done\r\n"
    },
    {
        "FRS?",
        {
            ">FRS? [List of stored routines]\r\n",
            ">Done\r\n"
        },
        ">FRS? [List of stored routines]\r\n"
    },
    {
        "DFRS",
        {
            ">DFRS\r\n",
            ">Done\r\n",
            ">DFRS ERR: Wrong Parameter.\r\n",
            ">DFRS ERR: Check Controller Connecting Status.\r\n",
            ">DFRS Cannot Find Routine Name.\r\n"
        },
        ">Done\r\n"
    },
    {
        "FDR",
        {
            ">FDR\r\n",
            ">Done [TotalPoints]\r\n",
            ">FDR ERR Wrong numbers of expecting parameters\r\n"
        },
        ">Done [TotalPoints]\r\n"
    },
    {
        "FSM",
        {
            ">FSM\r\n",
            ">Done [TotalPoints]\r\n",
            ">FSM ERR Wrong parameters.\r\n"
        },
        ">Done [TotalPoints]\r\n"
    },
    {
        "FLM",
        {
            ">FLM\r\n",
            ">Done [TotalPoints]\r\n",
            ">FLM ERR Wrong parameters\r\n",
            ">FLM ERR Wrong numbers of expecting parameters\r\n"
        },
        ">Done [TotalPoints]\r\n"
    },
    {
        "FDG",
        {
            ">FDG\r\n",
            ">FDG ERR Wrong parameters\r\n"
        },
        ">FDG\r\n"
    },
    {
        "ADC?",
        {
            ">ADC? ch: [Channel] [Value]\r\n",
            ">ADC? [Value]\r\n"
        },
        ">ADC? ch: [Channel] [Value]\r\n"
    },
    {
        "SAC",
        {
            ">SAC\r\n",
            ">Done\r\n",
            ">SAC ERR Wrong index of ADC.\r\n"
        },
        ">Done\r\n"
    },
    {
        "BKN",
        {
            ">BKN\r\n",
            ">BKN ERR [ErrorCode]\r\n"
        },
        ">BKN\r\n"
    },
    {
        "BKN?",
        {
            ">BKN? : <Noise>\r\n"
        },
        ">BKN? : <Noise>\r\n"
    },
    {
        "WRD",
        {
            ">Done\r\n",
            ">WRD Error.\r\n"
        },
        ">Done\r\n"
    },
    {
        "RRD",
        {
            ">Done\r\n",
            ">RRD Error.\r\n"
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
