find_package(Qt5 5.2 REQUIRED COMPONENTS DBus)

    pkg_check_modules(NNQT5 REQUIRED nemonotifications-qt5)
    include_directories(${NNQT5_INCLUDE_DIRS})

set(kdeconnectsfos_SRCS
    kdeconnect-sfos.cpp
    ../plasmoid/declarativeplugin/kdeconnectdeclarativeplugin.cpp
    ../plasmoid/declarativeplugin/responsewaiter.cpp
)

#find_package(KF5 COMPONENTS Declarative)

#qt5_add_resources(kcapp_SRCS resources.qrc)
qt5_wrap_cpp(kdeconnectsfos_SRCS ../plasmoid/declarativeplugin/objectfactory.h)

add_executable(kdeconnect-sfos ${kdeconnectsfos_SRCS})
target_link_libraries(kdeconnect-sfos Qt5::Quick KF5::CoreAddons KF5::I18n sailfishapp Qt5::DBus kdeconnectinterfaces ${NNQT5_LIBRARIES})

install(TARGETS kdeconnect-sfos ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS kdeconnect-sfos.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
INSTALL( DIRECTORY qml DESTINATION ${SHARE_INSTALL_PREFIX}/kdeconnect-sfos/ )

