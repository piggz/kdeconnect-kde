project(kdeconnect)

set(KDECONNECT_VERSION_MAJOR 1)
set(KDECONNECT_VERSION_MINOR 2)
set(KDECONNECT_VERSION_PATCH 1)
set(KDECONNECT_VERSION "${KDECONNECT_VERSION_MAJOR}.${KDECONNECT_VERSION_MINOR}.${KDECONNECT_VERSION_PATCH}")

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

find_package(PkgConfig)
find_package(Qt5 5.2 REQUIRED COMPONENTS Quick)

if (SAILFISHOS)
    set(KF5_MIN_VERSION 5.31.0)
    set(KF5_REQUIRED_COMPONENTS I18n DBusAddons CoreAddons)
    set(QCA_MIN_VERSION 2.0.0)
    pkg_search_module(SFOS REQUIRED sailfishapp)
    pkg_check_modules(QCA2 qca2-qt5>=${QCA_MIN_VERSION} REQUIRED)
    add_definitions(-DSAILFISHOS)
    include_directories(${CMAKE_SOURCE_DIR} ${QCA2_INCLUDEDIR})
else()
    set(KF%_MIN_VERSION 5.42.0)
    set(KF5_REQUIRED_COMPONENTS I18n ConfigWidgets DBusAddons)
    set(QCA_MIN_VERSION 2.1.0)
    find_package(Qca-qt5 ${QCA_MIN_VERSION} REQUIRED)
endif()

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS ${KF5_REQUIRED_COMPONENTS})

if (NOT SAILFISHOS)
    find_package(KF5DocTools)
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

add_definitions(-DQT_NO_URL_CAST_FROM_STRING -DQT_NO_KEYWORDS)

include(GenerateExportHeader)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)
if(NOT SAILFISHOS)
    add_subdirectory(kcm)
    add_subdirectory(kcmplugin)
endif()
if(NOT WIN32 AND NOT SAILFISHOS)
	add_subdirectory(kio)
	add_subdirectory(plasmoid)
endif()
add_subdirectory(icon)
add_subdirectory(interfaces)

option(EXPERIMENTALAPP_ENABLED OFF)
if(EXPERIMENTALAPP_ENABLED)
    add_subdirectory(app)
endif()
add_subdirectory(daemon)
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
