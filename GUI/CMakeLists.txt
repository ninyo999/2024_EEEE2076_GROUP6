cmake_minimum_required(VERSION 3.16)

project(GROUP6_GUI VERSION 0.1 LANGUAGES CXX)

# Set CMake policies
cmake_policy(SET CMP0072 NEW)

# Qt automatic processing
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages (compatible with both Qt5 and Qt6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS 
    Widgets 
    Core
    OpenGL
    OpenGLWidgets
)

# Find VTK 9.4 with ALL required components
find_package(VTK 9.4 REQUIRED COMPONENTS
    CommonCore
    CommonDataModel
    FiltersCore
    FiltersGeneral
    FiltersGeometry  # Required for vtkGeometryFilter
    RenderingCore
    RenderingOpenGL2
    IOImage
    IOGeometry
    InteractionStyle
    FiltersSources
    ViewsQt
)

message(STATUS "Using Qt version: ${Qt${QT_VERSION_MAJOR}_VERSION}")
message(STATUS "Using VTK version: ${VTK_VERSION}")

# Source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ModelPart.cpp
    ModelPart.h
    ModelPartList.cpp
    ModelPartList.h
    optiondialog.cpp
    optiondialog.h
    optiondialog.ui
    backgrounddialog.cpp
    backgrounddialog.h
    backgrounddialog.ui
    icons.qrc
)

# Create executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GROUP6_GUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(GROUP6_GUI
        ${PROJECT_SOURCES}
    )
endif()

# Link libraries - MODERN TARGET-BASED APPROACH
target_link_libraries(GROUP6_GUI PRIVATE
    # Qt libraries
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets

    # VTK libraries - must include FiltersGeometry
    VTK::CommonCore
    VTK::CommonDataModel
    VTK::FiltersCore
    VTK::FiltersGeneral
    VTK::FiltersGeometry  # This provides vtkGeometryFilter
    VTK::RenderingCore
    VTK::RenderingOpenGL2
    VTK::IOImage
    VTK::IOGeometry
    VTK::InteractionStyle
    VTK::FiltersSources
    VTK::ViewsQt
)

# Platform-specific settings
set_target_properties(GROUP6_GUI PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation
include(GNUInstallDirs)
install(TARGETS GROUP6_GUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt6 finalization
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GROUP6_GUI)
endif()

# VR Bindings
add_custom_target(VRBindings)
add_custom_command(TARGET VRBindings PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/vrbindings
    ${CMAKE_BINARY_DIR}/
)