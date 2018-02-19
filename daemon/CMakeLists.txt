project(kdeconnectd)

if (SAILFISHOS)
    pkg_check_modules(NNQT5 REQUIRED nemonotifications-qt5)
    include_directories(${NNQT5_INCLUDE_DIRS})
else()
    find_package(KF5 REQUIRED COMPONENTS Notifications KIO)
endif()

add_definitions(-DTRANSLATION_DOMAIN="kdeconnect-kded")

add_executable(kdeconnectd kdeconnectd.cpp)

if (SAILFISHOS)
    target_link_libraries(kdeconnectd kdeconnectcore KF5::DBusAddons ${NNQT5_LIBRARIES} KF5::I18n)
else()
    target_link_libraries(kdeconnectd kdeconnectcore KF5::KIOWidgets KF5::DBusAddons KF5::Notifications KF5::I18n Qt5::Widgets)
endif()


ecm_mark_nongui_executable(kdeconnectd)

configure_file(kdeconnectd.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdeconnectd.desktop)
configure_file(org.kde.kdeconnect.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdeconnect.service)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kdeconnectd.desktop DESTINATION ${AUTOSTART_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdeconnect.service DESTINATION ${DBUS_SERVICES_INSTALL_DIR})
install(TARGETS kdeconnectd DESTINATION ${LIBEXEC_INSTALL_DIR})

