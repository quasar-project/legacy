cmake_minimum_required (VERSION 3.15)

project(libquasarsdk LANGUAGES CXX)

option(REGISTER_QML_TYPES
  "Link against Qt5::Quick and let library register QML types automatically"
  ON
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
if(REGISTER_QML_TYPES)
  find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Network
    Quick
    Positioning
  )
else()
  find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Network
    Gui
    Positioning
  )
endif()

include_directories(src include)

file(GLOB_RECURSE SDK_CORE src/* )
file(GLOB_RECURSE SDK_CORE_INCLUDE include/* )
set(SRC ${SDK_CORE} ${SDK_CORE_INCLUDE} quasarsdk.qrc )

add_library(${PROJECT_NAME} ${SRC})

if(REGISTER_QML_TYPES)
  target_link_libraries(${PROJECT_NAME} PRIVATE
     Qt${QT_VERSION_MAJOR}::Core
     Qt${QT_VERSION_MAJOR}::Network
     Qt${QT_VERSION_MAJOR}::Quick
     Qt${QT_VERSION_MAJOR}::Positioning
  )
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Positioning
  )
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
  -DM_PI=3.14159265358979323846
)

target_include_directories(${PROJECT_NAME} PUBLIC include src)
