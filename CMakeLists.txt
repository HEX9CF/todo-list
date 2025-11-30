cmake_minimum_required(VERSION 3.19)
project(todo-list LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(todo-list
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/controller/mainwindow.cpp
    src/controller/mainwindow.h
    src/view/mainwindow.ui
    src/model/todoitem.cpp
    src/model/todoitem.h
    src/repository/databasemanager.cpp
    src/repository/databasemanager.h
)

target_link_libraries(todo-list
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
)

include(GNUInstallDirs)

install(TARGETS todo-list
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET todo-list
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
