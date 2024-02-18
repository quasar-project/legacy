cmake_minimum_required(VERSION 3.25)

project(QuaSAR
  DESCRIPTION "SAR control software"
  HOMEPAGE_URL "github.com/whs31/quasar-software"
  LANGUAGES C CXX
)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake)
include(qtprojectbuild)
include(cmake/all.cmake)

project_version_patch_clean()

set(COMPANY "Radar-MMS")
set(COPYRIGHT "Copyright (c) 2023 ${COMPANY}. All rights reserved.")
set(IDENTIFIER "${PROJECT_NAME}")

add_project_meta(META_FILES)

set_language_standard(23)
check_language_features()

# 3rd party packages locating
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt5 REQUIRED COMPONENTS
  Core
  Quick
  Widgets
  Network
  Location
  Positioning
  QuickControls2
  Concurrent
)

include_directories(src/c++ libs)

file(GLOB_RECURSE CPP_SOURCES
  src/c++/application/*
  src/c++/gui/*
  src/c++/filesystem/*
  src/c++/config/*
  src/c++/map/*
  src/c++/scenegraph/*
  src/c++/network/*
  src/c++/processing/*
)

set(PROJECT_SOURCES
  src/c++/main.c++
  src/c++/quasar.h src/c++/quasar.c++

  src/qml/qml.qrc
  src/js/js.qrc
  resources/resources.qrc
)

add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCES}
  ${CPP_SOURCES}
  ${META_FILES}
)

# CMake options for children libraries
set(ARGPARSE_SHARED OFF CACHE BOOL "Build shared library")
set(ARGPARSE_STATIC ON CACHE BOOL "Build static library")

# Libraries and subprojects
add_subdirectory(libs/Argparse)
add_subdirectory(src/c++/sdk) # deprecated
add_subdirectory(libs/LPVL)
add_subdirectory(libs/CCL)
add_subdirectory(libs/libquasarsdk)
add_subdirectory(services/UpdateService)

# Linking
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::Widgets
    Qt5::Network
    Qt5::Location
    Qt5::Positioning
    Qt5::QuickControls2
    Qt5::Concurrent
    #Corrosion
    argparse
    SDK
    LPVL
    CCL
    libquasarsdk
)

target_add_metadata_definitions(${PROJECT_NAME})
enable_static_crt()

# C++ definitions
add_definitions(
  -DPROJECT_NAME="${PROJECT_NAME}"
  -DPROJECT_VERSION="${GIT_VERSION}"
  -DPROJECT_COMPANY="${COMPANY}"
  -DM_PI=3.14159265358979323846
)

message("${PROJECT_NAME} version ${PROJECT_VERSION} compiled successfully")

