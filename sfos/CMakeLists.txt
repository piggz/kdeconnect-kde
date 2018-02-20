find_package(Qt5 5.2 REQUIRED COMPONENTS DBus)

set(kdeconnectsfos_SRCS
    kdeconnect-sfos.cpp
    ../plasmoid/declarativeplugin/kdeconnectdeclarativeplugin.cpp
    ../plasmoid/declarativeplugin/responsewaiter.cpp
)

#find_package(KF5 COMPONENTS Declarative)

#qt5_add_resources(kcapp_SRCS resources.qrc)

add_executable(kdeconnect-sfos ${kdeconnectsfos_SRCS})
target_link_libraries(kdeconnect-sfos Qt5::Quick KF5::CoreAddons KF5::I18n sailfishapp Qt5::DBus kdeconnectinterfaces)

install(TARGETS kdeconnect-sfos ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL( DIRECTORY qml DESTINATION ${SHARE_INSTALL_PREFIX}/kdeconnect-sfos/ )

