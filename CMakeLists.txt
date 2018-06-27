cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)

cmake_policy(SET CMP0020 NEW)


## Specify Qt build directory
set(QT_ROOT_DIR "/usr/local/opt/qt")

set(CMAKE_PREFIX_PATH "${QT_ROOT_DIR}/lib/cmake/")

project(LLabVolumeUpBacker)

set(PROJECT_LIB ${PROJECT_NAME}_lib)

# Define all source and header files in src/
file(GLOB LIB_SOURCES lib/*.cpp)
file(GLOB LIB_HEADERS lib/*.h)


######################################################################
## Macos native frameworks
######################################################################
if(APPLE)
    include_directories(/Developer/Headers/FlatCarbon)
    find_library(CARBON_LIBRARY Carbon)
    find_library(FOUNDATION_LIBRAY Foundation)
    find_library(APP_SERVICES_LIBRARY ApplicationServices )
    if(NOT CARBON_LIBRARY)
        message(FATAL_ERROR "Carbon not found")
    endif()
    if(NOT FOUNDATION_LIBRAY)
        message(FATAL_ERROR "Foundation not found")
    endif()
    if(NOT APP_SERVICES_LIBRARY)
        message(FATAL_ERROR "ApplicationServices not found")
    endif()
    mark_as_advanced(CARBON_LIBRARY FOUNDATION_LIBRAY APP_SERVICES_LIBRARY)
    SET(MACOS_FRAMEWORKS ${CARBON_LIBRARY} ${FOUNDATION_LIBRAY} ${APP_SERVICES_LIBRARY})
endif(APPLE)

######################################################################
## endof Macos native frameworks
######################################################################

######################################################################
## GUI setup
######################################################################
if(WIN32)
else()
    add_definitions(-fPIC)
endif()

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt5Core and Qt5Widgets
find_package(Qt5Core REQUIRED CONFIG)
find_package(Qt5Widgets REQUIRED CONFIG)

# For finding Qt includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR} LIB_SOURCES LIB_HEADERS)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})

file(GLOB GUI_SRC gui/*.cpp)
file(GLOB GUI_H gui/*.h)

# set Qt specific files
file ( GLOB UIS gui/*.ui)
file ( GLOB RESOURCES resources/*.qrc)

add_library(${PROJECT_LIB} ${LIB_SOURCES} ${LIB_HEADERS} ${GUI_SRC} ${GUI_H} ${UIS} ${RESOURCES})
target_link_libraries(${PROJECT_LIB} ${MACOS_FRAMEWORKS})
target_link_libraries(${PROJECT_LIB} Qt5::Core Qt5::Widgets)

######################################################################
## endof GUI setup
######################################################################




######################################################################
## Catch2 setup - TEST PROJECT
######################################################################
set(TEST_EXECUTABLE "Test_build")

# Prepare catch2 library
set(CATCH_INCLUDE_DIR tst_catch/catch.hpp)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Create executable for catch2 test project
file(GLOB CATCH_TEST_SOURCES tst/*.cpp)
add_executable(${TEST_EXECUTABLE} ${CATCH_TEST_SOURCES})
# Link catch
target_link_libraries(${TEST_EXECUTABLE} Catch)
# Link OpenCV and Cellsorter lib
target_link_libraries(${TEST_EXECUTABLE} ${PROJECT_LIB})

######################################################################
## endof Catch2 setup
######################################################################



######################################################################
## macOS deployment
######################################################################
if(APPLE)
    ## Project specific
    set(BUNDLE_TARGET_NAME ${PROJECT_NAME})
    set(BUNDLE_ICON_PATH LLabVolumeBackuper.icns)

    ## Generic macOs deployment script

    include(BundleUtilities)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
    # set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_ICON_FILE ${BUNDLE_ICON_PATH})
    set(BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/${BUNDLE_ICON_PATH})
    set_source_files_properties(${BUNDLE_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    set(PRINCIPAL_CLASS "NSApplication")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.llab.volumebackuptool")
    set(MACOSX_BUNDLE_INFO_STRING "Created with cmake and qt")
    set(MACOSX_BUNDLE_COPYRIGHT "MIT license")


    file(GLOB QTROOTS ${QT_ROOT_DIR}/clang_64/bin/)
    find_program(QT_QMAKE_EXECUTABLE NAMES qmake PATHS ${QTROOTS})


    # Create executables for RTOC
    add_executable(${BUNDLE_TARGET_NAME} MACOSX_BUNDLE ${BUNDLE_ICON} main.cpp)
    set_target_properties(${BUNDLE_TARGET_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in)

    # Link OpenCV libraries
    target_link_libraries(${BUNDLE_TARGET_NAME} ${PROJECT_LIB})

    # Clean deployment destination
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_TARGET_NAME}.app/)
        message("${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_TARGET_NAME}.app/")
        add_custom_command (
                TARGET ${BUNDLE_TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E time remove ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_TARGET_NAME}.app/Contents/Frameworks
                COMMENT "Clean macos deploy"
                VERBATIM
        )
    endif()

    # Deploy with macdeployqt
    add_custom_command(
                TARGET ${BUNDLE_TARGET_NAME} POST_BUILD
                COMMAND ${QT_ROOT_DIR}/bin/macdeployqt ${BUNDLE_TARGET_NAME}.app
                WORKING_DIRECTORY
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                COMMENT "Running macOs deployment lezgooo"
                VERBATIM
    )
endif()
######################################################################
## endof macOS deployment
######################################################################
