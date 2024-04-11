cmake_minimum_required(VERSION 3.5)

project(Sunrise VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/vitaliykolbasiuk/Qt/6.6.0/gcc_64/lib/cmake")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        ToolBox.cpp
        ToolBox.h
        Diagram.cpp
        Diagram.h
        ToolBoxModel.cpp
        ToolBoxModel.h
        DrawElements.h
        DragObject.cpp
        DragObject.h
        Python.h
        ElementText.cpp
        ElementText.h
        ElementText.ui
        ConnectionPoint.cpp
        ConnectionPoint.h
        DiagramElement.cpp
        DiagramElement.h
        Utils.h
)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(SYSTEM cereal/include)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Sunrise
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Python.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Sunrise APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Sunrise SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Sunrise
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Sunrise PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Sunrise)
endif()
set_target_properties(Sunrise PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Sunrise
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Sunrise)
endif()
