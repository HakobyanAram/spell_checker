cmake_minimum_required(VERSION 2.8)
project(spell_checker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -W -Wextra")

set(APP_SOURCES
   ./main.cpp
   ./spell_checker.cpp
)

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    boost_system
    boost_filesystem
    boost_program_options
)









