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
#include <KLocalizedString>

#ifndef SAILFISHOS
#include <KIO/AccessManager>
#endif

#include "core/daemon.h"
#include "core/device.h"
#include "core/backends/pairinghandler.h"
#include "kdeconnect-version.h"

#ifdef SAILFISHOS
#include <notification.h>

class SailfishDaemon : public Daemon
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kdeconnect.daemon")
public:
    SailfishDaemon(QObject* parent = Q_NULLPTR)
        : Daemon(parent)
        , m_nam(Q_NULLPTR)
    {}

    void askPairingConfirmation(Device* device) override
    {
        qDebug() << "Pairing request from " << device->name().toHtmlEscaped();

        Notification *notification = new Notification(this);

        notification->setAppName(QCoreApplication::applicationName());
        notification->setPreviewSummary(i18n("Pairing request from %1", device->name().toHtmlEscaped()));
        notification->setPreviewBody(i18n("Click here to pair"));
        notification->setIcon("icon-s-sync");
        notification->setExpireTimeout(10000);

        connect(notification, &Notification::closed,
                [=]( uint reason ) {
                    qDebug() << "Notification closed" << reason;
                    if (reason == 2) { //clicked
                        device->acceptPairing();
                    } else {
                        device->rejectPairing();
                    }
                });

        notification->publish();
    }

    void reportError(const QString & title, const QString & description) override
    {
        qDebug() << "Error:  " << title << ":" << description;
    }

    QNetworkAccessManager* networkAccessManager() override
    {
        if (!m_nam) {
            m_nam = new QNetworkAccessManager(this);
        }
        return m_nam;
    }

private:
    QNetworkAccessManager* m_nam;
};

#else
#include <KNotification>
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
        KNotification* notification = new KNotification(QStringLiteral("pairingRequest"));
        notification->setIconName(QStringLiteral("dialog-information"));
        notification->setComponentName(QStringLiteral("kdeconnect"));
        notification->setText(i18n("Pairing request from %1", device->name().toHtmlEscaped()));
        notification->setActions(QStringList() << i18n("Accept") << i18n("Reject"));
//         notification->setTimeout(PairingHandler::pairingTimeoutMsec());
        connect(notification, &KNotification::action1Activated, device, &Device::acceptPairing);
        connect(notification, &KNotification::action2Activated, device, &Device::rejectPairing);
        notification->sendEvent();
    }

    void reportError(const QString & title, const QString & description) override
    {
        KNotification::event(KNotification::Error, title, description);
    }

    QNetworkAccessManager* networkAccessManager() override
    {
        if (!m_nam) {
            m_nam = new KIO::AccessManager(this);
        }
        return m_nam;
    }

private:
    QNetworkAccessManager* m_nam;
};

#endif

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

#ifdef SAILFISHOS
    Daemon* daemon = new SailfishDaemon;
#else
    Daemon* daemon = new DesktopDaemon;
#endif

    QObject::connect(daemon, SIGNAL(destroyed(QObject*)), &app, SLOT(quit()));

    return app.exec();
}

#include "kdeconnectd.moc"
