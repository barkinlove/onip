cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(onip VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/forms")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (WIN32)
    set(OPENNI2_LIB_PATH "${CMAKE_SOURCE_DIR}/external/vcpkg/packages/openni2_x86-windows")
else()
    set(OPENNI2_LIB_PATH "{CMAKE_SOURCE_DIR}/external/OpenNI-Linux-x64-2.3")
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL)

set(INTERNAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/include")

set(PROJECT_SOURCES
        "${CMAKE_SOURCE_DIR}/src/main.cpp"
        "${CMAKE_SOURCE_DIR}/src/MainWindow.cpp"
        "${CMAKE_SOURCE_DIR}/forms/MainWindow.ui"
        "${CMAKE_SOURCE_DIR}/src/Application.cpp"
        "${CMAKE_SOURCE_DIR}/src/GLWindow.cpp"
        "${CMAKE_SOURCE_DIR}/src/OniDevice.cpp"
        "${CMAKE_SOURCE_DIR}/src/OniVideoStream.cpp"
        "${INTERNAL_INCLUDE_DIR}/Application.hpp"
        "${INTERNAL_INCLUDE_DIR}/MainWindow.hpp"
        "${INTERNAL_INCLUDE_DIR}/OniDevice.hpp"
        "${INTERNAL_INCLUDE_DIR}/OniVideoStream.hpp"
        "${INTERNAL_INCLUDE_DIR}/GLWindow.hpp"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES} ${UI}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET onip APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
            vcpkg.json
        )
    endif()
endif()

if (WIN32)
    set(OPENNI2_LIB_FILE "${OPENNI2_LIB_PATH}/lib/OpenNI2.lib")
else()
    set(OPENNI2_LIB_FILE "${OPENNI2_LIB_PATH}}/Redist/libOpenNI2.so")
endif()

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    ${OPENG_LIBRARIES}
    GLUT::GLUT
    ${OPENNI2_LIB_FILE}
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${INTERNAL_INCLUDE_DIR}
    "${OPENNI2_LIB_PATH}/Include"
)

target_compile_options(
  ${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>: /W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Wall -Wextra -Wpedantic -Werror>
 )

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.onip)
endif()

set_target_properties(onip PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS onip
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(onip)
endif()
