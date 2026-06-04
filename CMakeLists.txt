cmake_minimum_required(VERSION 3.14)
project(openEM)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# 1. Fetch the JSON parser
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# 2. Fetch the Eigen Math Engine
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
)
set(EIGEN_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(eigen)

# 3. Fetch Raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0 
)
FetchContent_MakeAvailable(raylib)

# 4. Build the solver 
# (Removed src/glad.c because Raylib handles OpenGL extensions)
add_executable(openEM src/main.cpp src/TUI.cpp src/Application.cpp src/graphics.cpp)

# Include directories (assuming you have an include/ folder in your project)
target_include_directories(openEM PRIVATE include/)

# 5. Link the libraries to your program
# (Removed glfw, consolidated PRIVATE keyword)
target_link_libraries(openEM 
    PRIVATE 
        nlohmann_json::nlohmann_json 
        Eigen3::Eigen
        raylib
)