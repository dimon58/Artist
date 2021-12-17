cmake_minimum_required(VERSION 3.16)
project(Artist)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

add_subdirectory(libs/tgbot-cpp)
add_subdirectory(libs/cpp-dotenv)
add_executable(Artist main.cpp services/upscale.cpp services/upscale.h settings.h services/utils.h services/utils.cpp handlers/handlers.h handlers/handlers.cpp handlers/base.h handlers/base.cpp handlers/nns.h handlers/nns.cpp)

target_link_libraries(Artist TgBot cpp_dotenv ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})