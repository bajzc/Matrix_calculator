cmake_minimum_required(VERSION 3.5)

project(GUI_calculator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
qt_add_lupdate(GUI_calculator TS_FILES Translate_zh_CN.ts Translate_en.ts)

if(WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message("Using MSVC")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /bigobj")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message("Using GCC")
        add_compile_options(-O2 -Wa,-mbig-obj)
    endif()
    message("Using windows")
    include_directories(C:/msys64/mingw64/include/eigen3)
endif(WIN32)

if(UNIX)
    message("Using UNIX")
    find_package (Eigen3 REQUIRED)
    include_directories(/usr/include/eigen3)
    add_compile_options(-O2 -pthread)
endif(UNIX)




set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        matrixeditor.h matrixeditor.cpp matrixeditor.ui
        exprtk.hpp
        myexprtk.h myexprtk.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GUI_calculator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}      
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GUI_calculator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GUI_calculator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GUI_calculator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GUI_calculator
    ${Eigen_LIBRARIES}
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(GUI_calculator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS GUI_calculator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GUI_calculator)
endif()


