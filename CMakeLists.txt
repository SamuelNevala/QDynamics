cmake_minimum_required(VERSION 3.16)
project(QuickDynamics VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_library(QuickDynamics STATIC)

qt_add_qml_module(quickdynamicsplugin
    URI QuickDynamics
    VERSION 1.0
    QML_FILES DefaultAnimation.qml LoaderWindow.qml
    SOURCES dimensions.cpp dimensions.h
    PLUGIN_TARGET quickdynamicsplugin
    DEPENDENCIES QtQuick
)

# set_target_properties(quickdynamicsplugin PROPERTIES
#     # MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#     # MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     # MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     # MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )

target_compile_definitions(quickdynamicsplugin PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(quickdynamicsplugin PRIVATE Qt6::Core Qt6::Gui Qt6::Quick)
target_include_directories(quickdynamicsplugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# TODO figure out the installaiton later!
# # Set installation paths
# set(INSTALL_DIR "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_QML}/${PROJECT_NAME}")

# # Install the QML files and qmldir
# install(DIRECTORY ${QT_QML_OUTPUT_DIRECTORY}/${PROJECT_NAME}/
#         DESTINATION ${INSTALL_DIR}
#         FILES_MATCHING PATTERN "*.qml" PATTERN "qmldir" PATTERN "*.qmltypes" PATTERN "*.qrc" PATTERN "*.lib")

# install(TARGETS quickdynamicsplugin
#         ARCHIVE DESTINATION ${INSTALL_DIR}
#         LIBRARY DESTINATION ${INSTALL_DIR}
#         RUNTIME DESTINATION ${INSTALL_DIR})
# message(STATUS "CMAKE_CURRENT_BINARY_DIR:" ${CMAKE_BINARY_DIR})
# message(STATUS "CMAKE_BINARY_DIR:" ${CMAKE_BINARY_DIR})
# message(STATUS "QT6_INSTALL_PREFIX+QML:" ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_QML}/${PROJECT_NAME})
# message(STATUS "TARGET:" ${TARGET})
# message(STATUS "INSTALL_DIR:" ${INSTALL_DIR})
# message(STATUS "CMAKE_INSTALL_PREFIX:" ${CMAKE_INSTALL_PREFIX})

