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

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.kdeconnect 1.0
import Nemo.Notifications 1.0

Page {
    allowedOrientations: Orientation.Portrait

    SilicaFlickable {
            anchors.fill: parent
            contentHeight: column.height

            // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
            PullDownMenu {
                MenuItem {
                    text: qsTr("Find Devices")
                    onClicked: pageStack.push(Qt.resolvedUrl("FindDevices.qml"))
                }
            }

            Column {
                id: column
                width: parent.width
                spacing: 20

                PageHeader { title: "KDE Connect" }

                Button {
                    DevicesModel {
                        id: model;
                    }

                    onClicked: {
                        Notify.message("Hello", "over there");
                    }

                //TODO What should go here
                }

                Button {
                    onClicked: {
                            notification.show("Hello");
                    }
                }

                     Notification {
         id: notification
         category: "x-nemo.example"
         appName: "Example App"
         appIcon: "/usr/share/example-app/icon-l-application"
         summary: "Notification summary"
         body: "Notification body"
         previewSummary: "Notification preview summary"
         previewBody: "Notification preview body"
         itemCount: 5
         timestamp: "2013-02-20 18:21:00"
         remoteActions: [ {
             "name": "default",
             "displayName": "Do something",
             "icon": "icon-s-setting",
             "service": "org.nemomobile.example",
             "path": "/example",
             "iface": "org.nemomobile.example",
             "method": "doSomething",
             "arguments": [ "argument", 1 ]
         },{
             "name": "ignore",
             "displayName": "Ignore the problem",
             "icon": "icon-s-time",
             "service": "org.nemomobile.example",
             "path": "/example",
             "iface": "org.nemomobile.example",
             "method": "ignore",
             "arguments": [ "argument", 1 ]
         } ]
         onClicked: console.log("Clicked")
         onClosed: console.log("Closed, reason: " + reason)

                     }
                Button {
                    onClicked: {
                            notification.publish();
                    }
                }

            }
    }
}

