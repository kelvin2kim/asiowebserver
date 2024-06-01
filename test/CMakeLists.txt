cmake_minimum_required(VERSION 3.16)
project(test)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

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
        )

find_package(PkgConfig REQUIRED)
pkg_check_modules(libpqxx REQUIRED IMPORTED_TARGET libpqxx)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PkgConfig::libpqxx)