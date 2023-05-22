import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import Theme 1.0
import Network 1.0
import Config 1.0

import "network" as NetworkTab

Pane {
    Material.background: Theme.color("dark0");

    Flickable {
        anchors.fill: parent;
        clip: true;
        interactive: true;
        flickableDirection: Flickable.HorizontalAndVerticalFlick;
        contentHeight: 1000;
        contentWidth: 1896;

        NetworkTab.SocketConsole { id: panel_TelemetryConsole;
            name: "СОКЕТ ТЕЛЕМЕТРИИ";
            anchors.left: parent.left;
            anchors.top: parent.top;
            anchors.margins: 5;
            width: 720;
            height: 450;
            color: Theme.color("color3");
        }

        NetworkTab.SocketConsole { id: panel_ExecdConsole;
            name: "СОКЕТ КОМАНД";
            anchors.left: panel_TelemetryConsole.right;
            anchors.top: parent.top;
            anchors.margins: 5;
            width: 345;
            height: 450;
            color: Theme.color("accent");
        }

        NetworkTab.SocketConsole { id: panel_FeedbackConsole;
            name: "СОКЕТ ОБРАТНОЙ СВЯЗИ";
            anchors.left: panel_ExecdConsole.right;
            anchors.top: parent.top;
            anchors.margins: 5;
            width: 425;
            height: 450;
            color: Theme.color("yellow");
        }

        NetworkTab.SocketConsole { id: panel_LFSConsole;
            name: "СОКЕТ TCP-IP";
            anchors.right: parent.right;
            anchors.top: parent.top;
            anchors.margins: 5;
            width: 380;
            height: 450;
            color: Theme.color("green");
        }

        NetworkTab.SocketConsole { id: panel_StripConsole;
            name: "СОКЕТ UDP LFS";
            anchors.left: parent.left;
            anchors.top: panel_TelemetryConsole.bottom;
            anchors.margins: 5;
            width: 380;
            height: 450;
            color: Theme.color("color0");
        }
    }

    Pane { id: panel_Network;
        Material.elevation: 30;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.margins: 5;
        opacity: 0.85;

        Column {
            RoundButton { id: button_Connect;
                font.family: root.mainfont;
                height: 40;
                radius: 4;
                icon.source: "qrc:/icons/toolbar/map/refresh.png";
                text: Network.connected ? "Отключиться от РЛС" : "Подключиться к РЛС";
                Material.elevation: 30;
                Material.background: Network.connected ? Theme.color("red") : Theme.color("dark2");
                onPressed: {
                    if(Network.connected)
                    {
                        Network.stopExecdSocket();
                        Network.stopTelemetrySocket();
                        Network.stopTCPSocket();
                        Network.networkDelay = 100;
                    }
                    else
                    {
                        Network.startExecdSocket(Config.remoteIP + ":" + Config.execdPort,
                                                 Config.localIP + ":" + Config.feedbackPort);
                        Network.startTelemetrySocket(Config.remoteIP + ":" + Config.telemetryPort,
                                                     Config.telemetryFrequency);
                        Network.startTCPSocket(Config.localIP + ":" + Config.tcpLFSPort);
                    }
                }
            }
        }
    }

    Connections {
        target: Network;
        function onTelemetrySocketMetrics(data, size_bytes, out) { panel_TelemetryConsole.logdata(data, size_bytes, out); }
        function onFeedbackSocketMetrics(data, size_bytes, out) { panel_FeedbackConsole.logdata(data, size_bytes, out); }
    }
}
