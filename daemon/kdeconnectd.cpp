/**
 * Copyright 2014 Yuri Samoilenko <kinnalru@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef SAILFISHOS
#include <QCoreApplication>
#else
#include <QApplication>
#endif

#include <QNetworkAccessManager>
#include <QTimer>

#include <KDBusService>
#ifdef SAILFISHOS
#include <notification.h>
#else
#include <KNotification>
#endif
#include <KLocalizedString>

#ifndef SAILFISHOS
#include <KIO/AccessManager>
#endif

#include "core/daemon.h"
#include "core/device.h"
#include "core/backends/pairinghandler.h"
#include "kdeconnect-version.h"

class DesktopDaemon : public Daemon
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kdeconnect.daemon")
public:
    DesktopDaemon(QObject* parent = Q_NULLPTR)
        : Daemon(parent)
        , m_nam(Q_NULLPTR)
    {}

    void askPairingConfirmation(Device* device) override
    {
#ifdef SAILFISHOS


#else
        KNotification* notification = new KNotification(QStringLiteral("pairingRequest"));
        notification->setIconName(QStringLiteral("dialog-information"));
        notification->setComponentName(QStringLiteral("kdeconnect"));
        notification->setText(i18n("Pairing request from %1", device->name().toHtmlEscaped()));
        notification->setActions(QStringList() << i18n("Accept") << i18n("Reject"));
//         notification->setTimeout(PairingHandler::pairingTimeoutMsec());
        connect(notification, &KNotification::action1Activated, device, &Device::acceptPairing);
        connect(notification, &KNotification::action2Activated, device, &Device::rejectPairing);
        notification->sendEvent();
#endif
    }

    void reportError(const QString & title, const QString & description) override
    {
#ifndef SAILFISHOS
        KNotification::event(KNotification::Error, title, description);
#endif
    }

    QNetworkAccessManager* networkAccessManager() override
    {
        if (!m_nam) {
#ifdef SAILFISHOS
            m_nam = new QNetworkAccessManager(this);
#else
            m_nam = new KIO::AccessManager(this);
#endif
        }
        return m_nam;
    }

private:
    QNetworkAccessManager* m_nam;
};

int main(int argc, char* argv[])
{
#ifdef SAILFISHOS
    QCoreApplication app(argc, argv);
#else
    QApplication app(argc, argv);
#endif

    app.setApplicationName(QStringLiteral("kdeconnectd"));
    app.setApplicationVersion(QStringLiteral(KDECONNECT_VERSION_STRING));
    app.setOrganizationDomain(QStringLiteral("kde.org"));

#ifndef SAILFISHOS
    app.setQuitOnLastWindowClosed(false);
#endif

    KDBusService dbusService(KDBusService::Unique);

    Daemon* daemon = new DesktopDaemon;
    QObject::connect(daemon, SIGNAL(destroyed(QObject*)), &app, SLOT(quit()));

    return app.exec();
}

#include "kdeconnectd.moc"
