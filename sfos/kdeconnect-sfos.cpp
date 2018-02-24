/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp/sailfishapp.h>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <notification.h>

#include <plasmoid/declarativeplugin/kdeconnectdeclarativeplugin.h>

class Notify : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void message(const QString &title, const QString &body)
    {
        Notification notification;
        QVariantList remoteactions;


        remoteactions << Notification::remoteAction("yes",
                                                    QString(),
                                                    "com.kimmoli.harbour.maira",
                                                    "/",
                                                    "com.kimmoli.harbour.maira",
                                                    "yes",
                                                    QVariantList());

        //notification.setAppName("kdeconnect");
        notification.setSummary(title);
        notification.setPreviewSummary(title);
        notification.setBody(body);
        notification.setPreviewBody(body);
        //notification.setCategory("x-nemo.call.missed");
        //notification.setItemCount(0);
        //notification.setReplacesId(0);
        notification.setIcon("icon-s-sync");
        notification.setExpireTimeout(10000);
        notification.setRemoteActions(remoteactions);

        notification.publish();
    };
};

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/kdeconnect-sfos.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //   - SailfishApp::pathToMainQml() to get a QUrl to the main QML file
    //
    // To display the view, call "show()" (will show fullscreen on device).

    //return SailfishApp::main(argc, argv);
    QGuiApplication *app = SailfishApp::application(argc, argv);
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    KdeConnectDeclarativePlugin plugin;
    plugin.registerTypes("ork.kde.kdeconnect");
    plugin.initializeEngine(view->engine(),"org.kde.kdeconnect");

    Notify notif;
    view->engine()->rootContext()->setContextProperty(QStringLiteral("Notify"), &notif);

    view->setSource(SailfishApp::pathTo("qml/kdeconnect-sfos.qml"));
    view->showFullScreen();

    return app->exec();
}

#include "kdeconnect-sfos.moc"
