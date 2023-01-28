import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtLocation 5.12
import QtPositioning 5.12
import QtGraphicalEffects 1.15
import "qrc:/qml/ui" as UI
import UX 1.0

MapQuickItem {
    function remove()
    {
        var deleteCheck = imageRemove(index);
        if(deleteCheck)
        {
            imageModel.remove(index);
            imageUIModel.remove(index);
        }
    }

    id: sarUI;
    property bool anim: m_visible;
    
    z: 2;
    zoomLevel: 0;
    anchorPoint.x: 18;
    anchorPoint.y: 18;
    property real m_opacity: 0.9;
    opacity: m_opacity;
    property real m_scale: 1;
    scale: m_scale;
    coordinate: QtPositioning.coordinate(m_lat, m_lon);
    sourceItem: Item {
        Image {
            id: radarImage;
            width: 36;
            height: 36;
            layer.enabled: true;
            transformOrigin: Item.Center;
            smooth: true;
            source: "qrc:/map/markers/image.png";
            visible: true;
        }

        MouseArea {
            id: radarImageMouseArea;
            propagateComposedEvents: true;
            cursorShape: Qt.WhatsThisCursor;
            anchors.left: radarImage.left;
            anchors.leftMargin: -5;
            anchors.right: radarImage.right;
            anchors.rightMargin: -5;
            anchors.bottom: radarImage.bottom;
            anchors.bottomMargin: -5;
            anchors.top: radarImage.top;
            anchors.topMargin: -5;
            onClicked: { anim = !anim; }
        }
        ColorOverlay {
            id: radarImageOverlay;
            anchors.fill: radarImage;
            source: radarImage;
            opacity: 0.75;
            color: UX.textWhite;
        }
        DropShadow {
            anchors.fill: radarImageOverlay;
            horizontalOffset: 5;
            verticalOffset: 5;
            radius: 8.0;
            samples: 17;
            color: "#000000";
            source: radarImageOverlay;
        }
        Rectangle {
            id: textOverlay;
            color: UX.textWhite;
            width: (radarImageText.paintedWidth);
            height: (radarImageText.paintedHeight + 3);
            anchors.top: radarImage.bottom;
            anchors.topMargin: 5;
            anchors.horizontalCenter: radarImage.horizontalCenter;
            radius: 10
            Text {
                id: radarImageText;
                color: "#121212"; //radarImageOverlay.color;
                enabled: true;
                anchors.fill: parent;
                font.pointSize: 8;
                font.family: "Arial";
                font.weight: Font.Bold;
                textFormat: Text.RichText;
                text: "\u00A0Изображение №" + Number(index + 1) + "\u00A0"; //пробелы здесь не просто так =) U+00A0 nbsp
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;
            }
        }
        UI.RoundPane
        {
            id: sarDialogBase;
            anchors.left: textOverlay.right;
            anchors.leftMargin: 10;
            opacity: 0;
            radius: 10;
            z: 100;
            Material.elevation: 10;
            Component.onCompleted: { visible = false; }
            NumberAnimation on opacity {
                id: sardialogFadeIn;
                from: 0;
                to: 0.9;
                duration: 300;
                easing.type: Easing.Linear;
            }
            NumberAnimation on opacity {
                id: sardialogFadeOut;
                from: 0.9;
                to: 0;
                duration: 300;
                easing.type: Easing.Linear;
            }
            ColumnLayout
            {
                //columns: 2;
                spacing: 1;
                ButtonTopGroup { id: buttonColumn; }
                MetadataGroup { id: metadataColumn; }
            }
        }
    }
    onAnimChanged: {
        if(anim === true) { sardialogFadeIn.start(); sarDialogBase.enabled = true; sarDialogBase.visible = true;}
        else { sardialogFadeOut.start(); sarDialogBase.enabled = false; }
    }
}
