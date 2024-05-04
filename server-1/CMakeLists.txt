cmake_minimum_required(VERSION 3.16)
project(server1)

set(CMAKE_CXX_STANDARD 17)

#Boost 라이브러리 설치 확인
#find_package(Boost)
find_package(Boost REQUIRED COMPONENTS thread json)

# include_directories(./async-mqtt5/include)
include_directories(../include)

if(Boost_FOUND)
    # include 패스 설정
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries(${Boost_LIBRARIES})
endif()

set (SOURCE_FILES
        main.cpp
        server.cpp
        connection.cpp
        connection_manager.cpp
        request_handler.cpp
        request_parser.cpp
        reply.cpp
        mime_types.cpp
        )


add_executable(asio-http-1 ${SOURCE_FILES})

# target_link_libraries(async_tcp_server Boost::thread)