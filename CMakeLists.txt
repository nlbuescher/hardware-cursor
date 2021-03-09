cmake_minimum_required(VERSION 3.17)
project(hardware_cursor CXX)

add_executable(hardware_cursor main.cpp stb_image.h)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/include)
target_link_directories(${PROJECT_NAME} PRIVATE /usr/local/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw GL)
