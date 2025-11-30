cmake_minimum_required(VERSION 3.16)

project(qtmcserver VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Gui Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui Network)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    licensedialog.cpp licensedialog.h licensedialog.ui
    aboutdialog.cpp aboutdialog.h aboutdialog.ui
    settingsdialog.cpp settingsdialog.h settingsdialog.ui
    downloaddialog.cpp downloaddialog.h downloaddialog.ui
    qtmcserver.rc
)

set(RESOURCE
    "qtmcserver.qrc"
)

qt5_add_resources(PROJECT_RESOURCES ${RESOURCE})

add_executable(qtmcserver
    ${PROJECT_SOURCES}
    ${PROJECT_RESOURCES}
)

target_link_libraries(qtmcserver PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
)

set_target_properties(qtmcserver PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS qtmcserver
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
