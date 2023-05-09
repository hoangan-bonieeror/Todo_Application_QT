cmake_minimum_required(VERSION 3.16)

project(todo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

find_package(Qt6 REQUIRED COMPONENTS Network)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(apptodo
    main.cpp
    todo.cpp
    todo.h
    networkcontroller.h
    networkcontroller.cpp
)

qt_add_qml_module(apptodo
    URI todo
    VERSION 1.0
    QML_FILES Main.qml
)

set_target_properties(apptodo PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(apptodo
    PRIVATE Qt6::Quick
)

target_link_libraries(apptodo
    PRIVATE Qt6::Network
)

install(TARGETS apptodo
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
