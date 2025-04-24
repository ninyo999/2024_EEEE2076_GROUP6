
cmake_minimum_required(VERSION 3.16)


project(GROUP6_GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL Core)
find_package( VTK REQUIRED)
find_package(VTK REQUIRED COMPONENTS
    RenderingCore
    RenderingOpenGL2
    IOImage
    IOGeometry
    InteractionStyle
    FiltersSources
    ViewsQt
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
	ModelPart.cpp
	ModelPart.h
	ModelPartList.cpp
	ModelPartList.h
	icons.qrc
	optiondialog.cpp
	optiondialog.h
	backgrounddialog.cpp
	backgrounddialog.h

	
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GROUP6_GUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        optiondialog.h optiondialog.cpp optiondialog.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GROUP6_GUI APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GROUP6_GUI SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GROUP6_GUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GROUP6_GUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGL ${VTK_LIBRARIES} )

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GROUP6_GUI)
endif()
set_target_properties(GROUP6_GUI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS GROUP6_GUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GROUP6_GUI)
endif()

# Copy across Open VR bindings that map controllers
# The program will expect to find these in the build dir when it runs
add_custom_target( VRBindings )
add_custom_command( TARGET VRBindings PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E
    	copy_directory ${CMAKE_SOURCE_DIR}/vrbindings ${CMAKE_BINARY_DIR}/ )