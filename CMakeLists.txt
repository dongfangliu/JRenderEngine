# CMakeList.txt : CMake project for JRenderEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("JRenderEngine")
find_package(glad REQUIRED )
find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
set(CMAKE_CXX_STANDARD 17)
add_executable (JRenderEngine "JRenderEngine.cpp"
        src/camera.cpp
        src/mesh.cpp
        src/model.cpp
        src/shader.cpp

        src/texture.cpp

        src/geometry.cpp src/light.cpp 
        include/light.h src/transform.cpp 
        include/transform.h src/scene.cpp 
        include/scene.h 
        "include/material.h")
 
target_include_directories(JRenderEngine PUBLIC include)

target_link_libraries(JRenderEngine PUBLIC glad::glad)

target_link_libraries(JRenderEngine PUBLIC assimp::assimp)
target_link_libraries(JRenderEngine PUBLIC glfw)
target_link_libraries(JRenderEngine PUBLIC glm::glm)