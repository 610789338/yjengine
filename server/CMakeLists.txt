cmake_minimum_required(VERSION 3.16)

Project (yjengine)


# enable c++17
set(CMAKE_CXX_STANDARD 17)

# set build flags
set(CMAKE_BUILD_TYPE "Debug")
IF (UNIX)
	option(ASAN "build project on address sanitizer" OFF)
	IF (ASAN)
		set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -fPIC -g -Wl,--eh-frame-hdr -fsanitize=address")
	ELSE ()
		set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -fPIC -g -Wl,--eh-frame-hdr")
	ENDIF ()
ELSEIF (WIN32)
	set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS}")
ENDIF ()

option(GPERFTOOLS "build project on gperftools" OFF)

IF (GPERFTOOLS)
	add_compile_definitions(GPERFTOOLS)
ENDIF ()

# set bin output path
file(MAKE_DIRECTORY bin)
IF (UNIX)
	file(MAKE_DIRECTORY bin/linux)
	file(MAKE_DIRECTORY bin/linux/db)
	set(EXECUTABLE_OUTPUT_PATH bin/linux)
ELSEIF (WIN32)
	file(MAKE_DIRECTORY bin/windows)
	file(MAKE_DIRECTORY bin/windows/db)
	set(EXECUTABLE_OUTPUT_PATH bin/windows)
ENDIF ()

# set lib ouput path
file(MAKE_DIRECTORY lib)
set(LIBRARY_OUTPUT_PATH lib)

# add include
include_directories(${BOOST_DIR})
include_directories(.)
include_directories(include)

# add link
link_directories(${BOOST_DIR}/stage/lib lib)


# build lib engine
file(GLOB ENGINE "engine/*.h" "engine/*.cpp")
source_group(engine FILES ${ENGINE})
add_library(engine STATIC ${ENGINE})
IF (UNIX)
	target_link_libraries(engine boost_thread pthread tcmalloc profiler)
ELSEIF (WIN32)
	target_link_libraries(engine)
ENDIF ()

# build game exe
file(GLOB GAME "game/*.h" "game/*.cpp")
source_group(game FILES ${GAME})

file(GLOB FRAMEWORK "game/framework/*.h" "game/framework/*.cpp")
source_group(game\\framework FILES ${FRAMEWORK})

file(GLOB GAMEPLAY "game/gameplay/*.h" "game/gameplay/*.cpp")
source_group(game\\gameplay FILES ${GAMEPLAY})

file(GLOB COMPONENTS "game/gameplay/components/*.h" "game/gameplay/components/*.cpp")
source_group(game\\gameplay\\components FILES ${COMPONENTS})

file(GLOB ENTITIES "game/gameplay/entities/*.h" "game/gameplay/entities/*.cpp")
source_group(game\\gameplay\\entities FILES ${ENTITIES})

file(GLOB STUBS "game/gameplay/entities/stubs/*.h" "game/gameplay/entities/stubs/*.cpp")
source_group(game\\gameplay\\entities\\stubs FILES ${STUBS})

add_executable(game ${GAME} ${FRAMEWORK} ${GAMEPLAY} ${COMPONENTS} ${ENTITIES} ${STUBS})
target_link_libraries(game engine)
set_target_properties(game PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY bin/windows)

# build gate exe
file(GLOB GATE "gate/*.h" "gate/*.cpp")
source_group(gate FILES ${GATE})

file(GLOB FRAMEWORK "gate/framework/*.h" "gate/framework/*.cpp")
source_group(gate\\framework FILES ${FRAMEWORK})

add_executable(gate ${GATE} ${FRAMEWORK})
target_link_libraries(gate engine)
set_target_properties(gate PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY bin/windows)

# build client exe
file(GLOB CLIENT "client/*.h" "client/*.cpp")
source_group(client FILES ${CLIENT})

file(GLOB FRAMEWORK "client/framework/*.h" "client/framework/*.cpp")
source_group(client\\framework FILES ${FRAMEWORK})

file(GLOB GAMEPLAY "client/gameplay/*.h" "client/gameplay/*.cpp")
source_group(client\\gameplay FILES ${GAMEPLAY})

file(GLOB COMPONENTS "client/gameplay/components/*.h" "client/gameplay/components/*.cpp")
source_group(client\\gameplay\\components FILES ${COMPONENTS})

file(GLOB ENTITIES "client/gameplay/entities/*.h" "client/gameplay/entities/*.cpp")
source_group(client\\gameplay\\entities FILES ${ENTITIES})

add_executable(client ${CLIENT} ${FRAMEWORK} ${GAMEPLAY} ${COMPONENTS} ${ENTITIES})
target_link_libraries(client engine)
set_target_properties(client PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY bin/windows)

set(CMAKE_SUPPRESS_REGENERATION TRUE)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)
