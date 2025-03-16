cmake_minimum_required(VERSION 3.31)

project(KubeConfManager VERSION 0.1 LANGUAGES CXX)

# Opcional: define variables individuales para cada componente de la versión
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

# Opcional: configura un archivo de cabecera con la versión para usarlo en el código
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/src/version.h
)

# Asegúrate de incluir el directorio del archivo generado
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Core LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Core LinguistTools REQUIRED)

add_subdirectory(lib/yaml-cpp)

# add_subdirectory(lib/kubernetes/kubernetes)
set(TS_FILES i18n/KubeConfManager_es_ES.ts)

set(PROJECT_SOURCES
    src/main.cpp
    src/KubeConfManager.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/settingsform.cpp
    src/settingsform.h
    src/settingsform.ui
    src/switchcontextform.cpp
    src/switchcontextform.h
    src/switchcontextform.ui
    src/clustereditor.cpp
    src/clustereditor.h
    src/clustereditor.ui
    src/kubeparser.cpp
    src/kubeparser.h
    src/kubeconfigutils.cpp
    src/kubeconfigutils.h
    src/contextswitcher.h
    src/contextswitcher.cpp
    src/contextswitcher.ui
    src/contexteditor.h
    src/contexteditor.cpp
    src/contexteditor.ui
    src/kubeconfigmerger.h
    src/kubeconfigmerger.cpp
    src/kubeconfigmerger.ui
    src/createnewkubeconfigdialog.h
    src/createnewkubeconfigdialog.cpp
    src/createnewkubeconfigdialog.ui
    src/tint.h
    src/kubeconfig.h
    src/kubeconfig.cpp
    assets.qrc
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(KubeConfManager
        ${PROJECT_SOURCES}
    )

    # Define target properties for Android with Qt 6 as:
    # set_property(TARGET KubeConfManager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    # ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(KubeConfManager SHARED
            ${PROJECT_SOURCES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(KubeConfManager
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(KubeConfManager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core yaml-cpp)

#set_target_properties(KubeConfManager PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#)

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_finalize_executable(KubeConfManager)
#endif()
