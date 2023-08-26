cmake_minimum_required(VERSION 3.16)

Project (yjengine)


# enable c++17
set(CMAKE_CXX_STANDARD 17)

# set build flags
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -fPIC -g")

# set bin output path
file(MAKE_DIRECTORY bin)
file(MAKE_DIRECTORY bin/db)
set(EXECUTABLE_OUTPUT_PATH bin)

# set lib ouput path
file(MAKE_DIRECTORY lib)
set(LIBRARY_OUTPUT_PATH lib)

# add boost include
include_directories(${BOOST_DIR})

# add boost link
link_directories(${BOOST_DIR}/stage/lib)


# build lib engine
aux_source_directory(engine ENGINE_SRC_LIST)
add_library(engine STATIC ${ENGINE_SRC_LIST})
target_link_libraries(engine boost_thread pthread)


# add engine include
include_directories(.)

# build game exe
aux_source_directory(game GAME_SRC_LIST)
aux_source_directory(game/framework GAME_SRC_LIST)
aux_source_directory(game/gameplay/components GAME_SRC_LIST)
aux_source_directory(game/gameplay/entities GAME_SRC_LIST)
aux_source_directory(game/gameplay/entities/stubs GAME_SRC_LIST)
add_executable(game ${GAME_SRC_LIST})
target_link_libraries(game engine boost_thread pthread)

# build gate exe
aux_source_directory(gate GATE_SRC_LIST)
aux_source_directory(gate/framework GATE_SRC_LIST)
add_executable(gate ${GATE_SRC_LIST})
target_link_libraries(gate engine boost_thread pthread)

# build client exe
aux_source_directory(client CLIENT_SRC_LIST)
aux_source_directory(client/framework CLIENT_SRC_LIST)
aux_source_directory(client/gameplay/components CLIENT_SRC_LIST)
aux_source_directory(client/gameplay/entities CLIENT_SRC_LIST)
add_executable(client ${CLIENT_SRC_LIST})
target_link_libraries(client engine boost_thread pthread)


