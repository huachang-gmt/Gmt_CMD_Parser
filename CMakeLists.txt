cmake_minimum_required(VERSION 3.16)

project(Gmt_CMD_Parser
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Gmt_CMD_Parser
    src/main.cpp
    src/tcp_server.cpp
    src/command_parser.cpp
)

add_executable(parser_test
    src/parser_test.cpp
    src/command_parser.cpp
)

add_executable(parser_parameter_test
    src/parser_parameter_test.cpp
    src/command_parser.cpp
)

target_include_directories(Gmt_CMD_Parser
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(parser_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(parser_parameter_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)