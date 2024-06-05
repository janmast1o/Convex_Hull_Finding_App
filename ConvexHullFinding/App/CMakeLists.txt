cmake_minimum_required(VERSION 3.14)
project(ConvexHullFinding)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

add_executable(ConvexHullFinding main.cpp mainwindow.cpp pointslog.cpp pairhash.cpp forvis.cpp graham.cpp algorithmthread.cpp)
# add_executable(ConvexHullFinding main.cpp mainwindow.cpp)

target_link_libraries(ConvexHullFinding PRIVATE Qt5::Widgets)
target_link_libraries(ConvexHullFinding PRIVATE Qt5::Core)
target_link_libraries(ConvexHullFinding PRIVATE Qt5::Gui)
target_link_libraries(ConvexHullFinding PRIVATE Qt5::PrintSupport)

target_link_libraries(ConvexHullFinding PRIVATE qcustomplot)

target_compile_definitions(ConvexHullFinding PRIVATE QCUSTOMPLOT_USE_LIBRARY)
