if (SAILFISHOS)
    pkg_check_modules(NNQT5 REQUIRED nemonotifications-qt5)
    set(NOTIFICATION_LIB ${NNQT5_LIBRARIES})
    include_directories(${NNQT5_INCLUDE_DIRS})
else()
    find_package(KF5 REQUIRED COMPONENTS Notifications)
    set(NOTIFICATION_LIB ${NNQT5_LIBRARIES})
endif()

set(kdeconnect_battery_SRCS
    batteryplugin.cpp
    batterydbusinterface.cpp
)

kdeconnect_add_plugin(kdeconnect_battery JSON kdeconnect_battery.json SOURCES ${kdeconnect_battery_SRCS})

target_link_libraries(kdeconnect_battery
    kdeconnectcore
    Qt5::DBus
    KF5::I18n
    ${NOTIFICATION_LIB}
)
