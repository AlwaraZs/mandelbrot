cmake_minimum_required(VERSION 3.17)
project(mandelbrot)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fsanitize=address")

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(mandelbrot main.cpp RenderWindow.cpp RenderWindow.h GuardQImage.h MandelbrotDrawer.cpp MandelbrotDrawer.h MandelbrotDrawer.cpp MandelbrotDrawer.h RenderWindow.cpp RenderWindow.h)
target_link_libraries(mandelbrot Qt5::Widgets)