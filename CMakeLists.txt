project(randomizer-loader)

cmake_minimum_required(VERSION 3.11)

SET(SRC main.cpp
        sd2snes.cpp
        json11.cpp)


add_executable(rl ${SRC})
target_compile_options(rl PRIVATE -std=c++17 -g)
target_link_libraries(rl serialport boost_system stdc++fs pthread docopt fmt restclient-cpp curl ssl crypto)
