set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_C_COMPILER "/usr/bin/clang")

file(GLOB SOURCES "src/*.cpp")

add_executable(dag_main main.cpp ${SOURCES})
