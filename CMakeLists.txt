cmake_minimum_required(VERSION 3.15)
project(ArthaAI)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    Crow/include
    /opt/homebrew/include
    /opt/homebrew/opt/mysql-client/include
)

link_directories(
    /opt/homebrew/opt/mysql-client/lib
)

add_executable(artha_ai backend/main.cpp)

target_link_libraries(artha_ai mysqlclient pthread)