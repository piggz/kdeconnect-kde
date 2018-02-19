if (SAILFISHOS)
    pkg_check_modules(NNQT5 REQUIRED nemonotifications-qt5)
    set(NOTIFICATION_LIB ${NNQT5_LIBRARIES})
else()
    find_package(KF5 REQUIRED COMPONENTS Notifications)
    set(NOTIFICATION_LIB ${NNQT5_LIBRARIES})
endif()

set(kdeconnect_ping_SRCS
    pingplugin.cpp
)

kdeconnect_add_plugin(kdeconnect_ping JSON kdeconnect_ping.json SOURCES ${kdeconnect_ping_SRCS})

target_link_libraries(kdeconnect_ping
    kdeconnectcore
    Qt5::DBus
    KF5::I18n
    ${NOTIFICATION_LIB}
)
