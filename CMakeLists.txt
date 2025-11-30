cmake_minimum_required(VERSION 3.19)
project(todo-list LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(todo-list
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/widget/mainwindow.cpp
    src/widget/mainwindow.h
    src/widget/mainwindow.ui
    src/widget/tododialog.ui
    src/model/todoitem.cpp
    src/model/todoitem.h
    src/repo/databasemanager.cpp
    src/repo/databasemanager.h
    src/widget/tododialog.cpp
    src/widget/tododialog.h
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
