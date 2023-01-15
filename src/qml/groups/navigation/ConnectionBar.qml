import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import RuntimeData 1.0
import "qrc:/qml/ui" as UI

Item {
    id: base;
    width: childrenRect.width;
    height: childrenRect.height;
    Rectangle
    {
        id: bground;
        width: 230;
        height: 30;
        color: "#132623";
        border.color: "#204040";
        border.width: 2;
        radius: 14;
        opacity: 1;
        UI.ToolIcon {
            id: iconLinkStatus;
            tooltipenabled: true;
            tooltiptext: "Статус подключения к РЛС: " + (!RuntimeData.connected ? "<b><font color=\"#de3857\">нет соединения</font></b>"
                                                                                : "<b><font color=\"#73c924\">соединение установлено</font></b>");
            iconsource: "qrc:/ui-resources/white/antenna.png";
            anchors.right: parent.right;
            anchors.rightMargin: 7;
            anchors.verticalCenter: parent.verticalCenter;
            basewidth: 30;
            primarycolor: RuntimeData.connected ? "#73c924" : "#de3857";
            label: RuntimeData.connected ? "LNK" : "ERR";
        }
        UI.ToolIcon {
            id: iconLinkInfo;
            tooltipenabled: true;
            tooltiptext: "IP-адрес РЛС: <font color=\"#43a1ca\"><b>" + RuntimeData.sarIP + "</b></font><br>
                            IP-адрес АРМ: <font color=\"#43a1ca\"><b>" + RuntimeData.pcIP + "</b></font><br>
                            Порт телеметрии: <font color=\"#bc9117\"><b>" + RuntimeData.telemetryPort + "</b></font>,
                            порт загрузчика: <font color=\"#bc9117\"><b>" + RuntimeData.loaderPort + "</b></font>,<br>
                            порт интерфейса формирования: <font color=\"#c7a750\"><b>" + RuntimeData.commandPort + "</b></font>,
                            порт обратной связи <font color=\"#c7a750\"><b>" + RuntimeData.listenPort + "</b></font>";
            iconsource: "qrc:/ui-resources/white/connection-information.png";
            anchors.right: iconLinkStatus.left;
            anchors.rightMargin: 0;
            anchors.verticalCenter: parent.verticalCenter;
            basewidth: 30;
            primarycolor: "#dae1e5";
            label: "";
        }
        UI.ToolIcon {
            id: satellitesInfo;
            tooltipenabled: true;
            tooltiptext: "Количество спутников навигации";
            iconsource: "qrc:/ui-resources/white/satellite.png";
            label: "<font color=\"#c4bb4b\">" + Number(RuntimeData.satellites) + "</font>";
            anchors.right: iconLinkInfo.left;
            anchors.rightMargin: 0;
            anchors.verticalCenter: parent.verticalCenter;
            basewidth: 30;
            primarycolor: "#c4bb4b";
        }
        RoundButton
        {
            id: calibrateButton;
            Material.background: "#204040"
            height: 30;
            radius: 15;
            opacity: 1;
            text: "CAL";
            font.capitalization: Font.MixedCase;
            font.pixelSize: 11;
            font.bold: true;
            onClicked: RuntimeData.seaLevel = RuntimeData.elevation;
            z: 100;
        }
    }
}
