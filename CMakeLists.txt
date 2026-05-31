cmake_minimum_required(VERSION 3.14)
project(openEM)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# 1. Fetch the JSON parser
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(glfw 
  GIT_REPOSITORY https://github.com/glfw/glfw.git)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 2. Fetch the Eigen Math Engine
FetchContent_Declare(eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
)
set(EIGEN_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(eigen)

# 3. Build the solver
add_executable(openEM src/main.cpp src/TUI.cpp)

target_include_directories(openEM PRIVATE include/)

# 4. Link the libraries to your program
target_link_libraries(openEM 
    PRIVATE nlohmann_json::nlohmann_json 
    PRIVATE Eigen3::Eigen
    PRIVATE glfw
)