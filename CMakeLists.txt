cmake_minimum_required(VERSION 3.10)

project(FACEDETECT)

set(OpenCV_DIR C:/Users/diniz/trabalhodederzu/opencv/build)
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})