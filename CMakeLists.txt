cmake_minimum_required(VERSION 3.16)

project(DDLonline VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(ANDROID_APPLICATION_NAME "DDLkiller")

find_package(Qt6 REQUIRED COMPONENTS Quick Sql WebSockets)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appDDLonline
    main.cpp
)

qt_add_qml_module(appDDLonline
    URI DDLonline
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES Date.h
        SOURCES Date.cpp
        SOURCES DataSql.h
        SOURCES DataSql.cpp
        QML_FILES CalendarSelector.qml
        QML_FILES MaskCalender.qml
        SOURCES jsonGenerator.h
        SOURCES jsonGenerator.cpp
        SOURCES database.h
        SOURCES database.cpp
        SOURCES json.cpp
        SOURCES websocketclient.h
        SOURCES json.h
        SOURCES websocketclient.cpp
        RESOURCES android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml
        RESOURCES android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDDLonline PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDDLonline
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appDDLonline
    PRIVATE Qt6::Quick
    Qt6::Sql
    Qt6::Qml
    Qt6::WebSockets
)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android)
    set_target_properties(appDDLonline PROPERTIES
       QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif()

include(GNUInstallDirs)
install(TARGETS appDDLonline
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android)
    include("D:/SDK/Android/android_openssl/android_openssl.cmake")
    add_android_openssl_libraries(appDDLonline)
endif()
