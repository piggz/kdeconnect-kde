cmake_minimum_required(VERSION 3.0)

project(kdeconnect)

set(KDECONNECT_VERSION_MAJOR 1)
set(KDECONNECT_VERSION_MINOR 2)
set(KDECONNECT_VERSION_PATCH 1)
set(KDECONNECT_VERSION "${KDECONNECT_VERSION_MAJOR}.${KDECONNECT_VERSION_MINOR}.${KDECONNECT_VERSION_PATCH}")

find_package(PkgConfig)

if (SAILFISHOS)
    set(KF5_MIN_VERSION "5.31.0")
    set(QT_MIN_VERSION "5.6.0")
    set(KF5_REQUIRED_COMPONENTS I18n DBusAddons CoreAddons IconThemes Config)
    set(KF5_OPTIONAL_COMPONENTS)
    set(QCA_MIN_VERSION 2.0.0)
    pkg_search_module(SFOS REQUIRED sailfishapp)
    pkg_check_modules(QCA2 qca2-qt5>=${QCA_MIN_VERSION} REQUIRED)
    add_definitions(-DSAILFISHOS)
    include_directories(${QCA2_INCLUDEDIR})
    add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
else()
    set(KF5_MIN_VERSION "5.42.0")
    set(QT_MIN_VERSION "5.7.0")
    set(KF5_REQUIRED_COMPONENTS I18n ConfigWidgets DBusAddons IconThemes Notifications KIO KCMUtils)
    set(KF5_OPTIONAL_COMPONENTS DocTools)
    set(QCA_MIN_VERSION "2.1.0")
    find_package(Qca-qt5 ${QCA_MIN_VERSION} REQUIRED)
    add_definitions(-DQT_NO_URL_CAST_FROM_STRING -DQT_NO_KEYWORDS)
endif()

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Quick)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS ${KF5_REQUIRED_COMPONENTS})
if (KF5_OPTIONAL_COMPONENTS)
find_package(KF5 ${KF5_MIN_VERSION} COMPONENTS ${KF5_OPTIONAL_COMPONENTS})
endif()

include_directories(${CMAKE_SOURCE_DIR})

configure_file(kdeconnect-version.h.in ${CMAKE_CURRENT_BINARY_DIR}/kdeconnect-version.h)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMAddTests)
include(ECMSetupVersion)
include(ECMInstallIcons)
include(FeatureSummary)

include(KDEConnectMacros.cmake)

include(GenerateExportHeader)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)
if(NOT SAILFISHOS)
    add_subdirectory(kcm)
    add_subdirectory(kcmplugin)
    add_subdirectory(daemon)
endif()

if(NOT WIN32)
    if (NOT SAILFISHOS)
        add_subdirectory(kio)
    endif()
	add_subdirectory(plasmoid)
endif()

add_subdirectory(icon)
add_subdirectory(interfaces)

option(EXPERIMENTALAPP_ENABLED OFF)
if(EXPERIMENTALAPP_ENABLED)
    add_subdirectory(app)
endif()
add_subdirectory(plugins)
add_subdirectory(cli)

if (NOT SAILFISHOS)
    add_subdirectory(indicator)
    add_subdirectory(fileitemactionplugin)
    add_subdirectory(urlhandler)
else()
    add_subdirectory(sfos)
endif()

if(KF5DocTools_FOUND)
    add_subdirectory(doc)
endif()

if(BUILD_TESTING AND NOT SAILFISHOS)
    add_subdirectory(tests)
endif()

if(NOT SAILFISHOS)
    install(FILES org.kde.kdeconnect.kcm.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
endif()
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
