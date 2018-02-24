/**
 * Copyright 2018 Adam Pigg <adam@piggz.co.uk>
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

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QTimer>

#include <KDBusService>
#include <KLocalizedString>

#include "core/daemon.h"
#include "core/device.h"
#include "core/backends/pairinghandler.h"
#include "kdeconnect-version.h"

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
