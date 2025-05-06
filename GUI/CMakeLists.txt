cmake_minimum_required(VERSION 3.16)

project(VRCADSTUDIO2025 VERSION 0.1 LANGUAGES CXX)

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
    CommonColor  
    FiltersCore
    FiltersGeneral
    FiltersGeometry  
    RenderingCore
    RenderingOpenGL2
    IOImage
    IOGeometry
    InteractionStyle
    FiltersSources
    ViewsQt
    RenderingOpenVR
)

message(STATUS "Using Qt version: ${Qt${QT_VERSION_MAJOR}_VERSION}")
message(STATUS "Using VTK version: ${VTK_VERSION}")

# Source files
set(PROJECT_SOURCES
  main.cpp
  mainwindow.cpp
  ModelPart.cpp
  ModelPartList.cpp
  optiondialog.cpp
  backgrounddialog.cpp
  vrrenderthread.cpp
  skyboxutils.cpp

  mainwindow.h
  ModelPart.h
  ModelPartList.h
  optiondialog.h
  backgrounddialog.h
  vrrenderthread.h
  skyboxutils.h

  mainwindow.ui
  optiondialog.ui
  backgrounddialog.ui

  icons.qrc
)

# Create executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VRCADSTUDIO2025
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(VRCADSTUDIO2025
        ${PROJECT_SOURCES}
    )
endif()

# Link libraries - MODERN TARGET-BASED APPROACH
target_link_libraries(VRCADSTUDIO2025 PRIVATE
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
    VTK::FiltersGeometry
    VTK::RenderingCore
    VTK::RenderingOpenGL2
    VTK::IOImage
    VTK::IOGeometry
    VTK::InteractionStyle
    VTK::FiltersSources
    VTK::ViewsQt
    VTK::RenderingOpenVR
    VTK::CommonColor
)

# Platform-specific settings
set_target_properties(VRCADSTUDIO2025 PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation

include(GNUInstallDirs)
include(InstallRequiredSystemLibraries)
install(TARGETS VRCADSTUDIO2025
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  BUNDLE DESTINATION .
)

# Qt6 finalization
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VRCADSTUDIO2025)
endif()

install(FILES
  "${OpenVR_INCLUDE_DIR}/../bin/win64/openvr_api.dll"
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT RuntimeLibraries
)
install(DIRECTORY ${CMAKE_BINARY_DIR}/vrbindings/
  DESTINATION ${CMAKE_INSTALL_BINDIR}/vrbindings
  COMPONENT headers
)

# VR Bindings
add_custom_target(VRBindings ALL)
add_custom_command(TARGET VRBindings PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/vrbindings
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/vrbindings
            ${CMAKE_BINARY_DIR}/vrbindings
)

add_dependencies(VRCADSTUDIO2025 VRBindings)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/STL_models"
  DESTINATION STL_models
  COMPONENT   Models
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/skyboxmap"
  DESTINATION Skyboxmap
  COMPONENT   Map
)

if (WIN32)
  set(CPACK_GENERATOR "NSIS")
else()
  set(CPACK_GENERATOR "ZIP")
endif()

set(CPACK_PACKAGE_NAME          "VRCAD Studio 2025")
set(CPACK_PACKAGE_VENDOR        "Ninyo999 and WaiiPati")
set(CPACK_PACKAGE_DESCRIPTION   "A Virtual Reality system for visualising complex engineering CAD models demo")
set(CPACK_NSIS_DISPLAY_NAME     "VRCADSTUDIO2025")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_EXECUTABLES
    "bin/VRCADSTUDIO2025.exe" "VRCADSTUDIO2025"
  )

# –– Installation defaults ––
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VRCADSTUDIO2025")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "VRCADSTUDIO2025")

set(CPACK_NSIS_DISPLAY_NAME "VRCADSTUDIO2025")
set(CPACK_NSIS_CREATE_UNINSTALLER "ON")
set(CPACK_NSIS_MENU_LINKS "${CPACK_NSIS_EXECUTABLES_DIRECTORY}/VRCADSTUDIO2025.exe" "VRCADSTUDIO2025" "Uninstall.exe" "Uninstall VRCADSTUDIO")
set(CPACK_NSIS_MODIFY_PATH ON)

if(WIN32)
    get_filename_component(QT_BIN_DIR "${QT_DIR}/bin" ABSOLUTE)
    set(DEPLOY_DIR "${CMAKE_BINARY_DIR}/deploy")
    file(MAKE_DIRECTORY ${DEPLOY_DIR})
    file(GLOB QT_DLLS "${QT_BIN_DIR}/*.dll")
    file(COPY ${QT_DLLS} DESTINATION ${DEPLOY_DIR})
    file(GLOB QT_PLUGINS "${QT_BIN_DIR}/plugins/platforms/*.dll")
    file(COPY ${QT_PLUGINS} DESTINATION ${DEPLOY_DIR}/platforms)
    set(ENV{QT_QPA_PLATFORM_PLUGIN_PATH} "${DEPLOY_DIR}/platforms")
endif()

include(CPack)
install(TARGETS VRCADSTUDIO2025 
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/vrbindings/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/vrbindings
    COMPONENT headers
)
if(WIN32)
  find_program(WINDEPLOYQT_EXECUTABLE windeployqt
    HINTS
      "${Qt6_DIR}/bin"
      "$ENV{Qt6_DIR}/bin"
      "C:/Qt/6.*/msvc*/bin"
  )
  if(NOT WINDEPLOYQT_EXECUTABLE)
    message(FATAL_ERROR "windeployqt not found! Cannot proceed with Qt DLL deployment.")
  endif()
endif()

add_custom_command(TARGET VRCADSTUDIO2025 POST_BUILD
    COMMAND "${WINDEPLOYQT_EXECUTABLE}"
    --verbose 0
    --no-compiler-runtime
    --no-opengl-sw
    --dir "${CMAKE_BINARY_DIR}/windeployqt"
    "$<TARGET_FILE:VRCADSTUDIO2025>"
)

install(DIRECTORY "${CMAKE_BINARY_DIR}/windeployqt/" DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install VTK DLLs
install(FILES $<TARGET_RUNTIME_DLLS:VRCADSTUDIO2025>
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RuntimeLibraries
)

# Install OpenVR DLLs
install(FILES "${OpenVR_INCLUDE_DIR}/../bin/win64/openvr_api.dll"
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RuntimeLibraries
)

cpack_add_component(libraries)
cpack_add_component(headers)
