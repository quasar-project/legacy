import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Terminals 1.0
import Theme 1.0

Pane {
    id: control

    width: 600
    height: 400
    x: root.width / 3
    y: root.height / 3
    opacity: root.consoleshown ? 1 : 0
    z: 100
    clip: true

    Material.background: Qt.darker(Theme.color("dark0"), 1.2)
    Material.elevation: 200
    Component.onCompleted: DebugConsole.execute("help")

    Pane {
        id: header

        height: 32
        Material.background: Theme.color("dark3")
        Material.elevation: 20

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: -12
        }

        Text {
            id: title

            text: "КОНСОЛЬ РАЗРАБОТЧИКА"
            color: Theme.color("light0")
            verticalAlignment: Text.AlignVCenter

            anchors {
                left: parent.left
                leftMargin: 5
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: 2
            }

            font {
                family: root.mainfont
                pixelSize: 14
                bold: true
            }

        }

        // move window mouse area
        MouseArea {
            property point offset: Qt.point(0, 0)

            onPressed: offset = Qt.point(mouseX, mouseY)
            onPositionChanged: {
                if (pressed) {
                    let global_pos = mapToItem(root, mouseX, mouseY);
                    control.x = global_pos.x - offset.x;
                    control.y = global_pos.y - offset.y;
                }
            }

            anchors {
                fill: parent
                margins: -12
                rightMargin: 8
            }

        }

        Row {
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                topMargin: -8
                bottomMargin: -3
            }

            RoundButton {
                width: 26
                height: 26
                Material.background: Theme.color("red")
                onPressed: root.consoleshown = false
            }

        }

    }

    ListView { id: scrollView;
        model: DebugConsole;
        interactive: true;
        clip: true;

        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
            bottom: inputArea.top
        }

        ScrollBar.vertical: ScrollBar {
            id: vbar

            active: true
            policy: ScrollBar.AlwaysOn

            contentItem: Rectangle {
                implicitWidth: 4
                implicitHeight: 100
                radius: width / 2
                color: vbar.pressed ? Theme.color("dark3") : Theme.color("dark2")
            }

        }

        delegate: Row {
            spacing: 5

            Text {
                property var colors: [Theme.color("light0"), Theme.color("color3"), Theme.color("yellow"),
                    Theme.color("red"), Theme.color("green"), Theme.color("accent"), Theme.color("light1")]

                width: scrollView.width;
                text: message;
                font.family: root.monofont;
                color: colors[type]
                font.bold: true
                font.pixelSize: 12
                textFormat: Text.RichText
            }
        }

        onCountChanged: scrollView.ScrollBar.vertical.position = 1.0;
    }

    Rectangle {
        id: inputArea

        color: Theme.color("dark1")
        border.width: 0.5
        border.color: Theme.color("dark2")
        height: 28

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: -12
        }

        TextInput {
            anchors.fill: parent;
            anchors.leftMargin: 3;
            color: Theme.color("light1");
            text: "";
            verticalAlignment: Text.AlignVCenter;
            selectByMouse: true;
            selectedTextColor: Theme.color("dark0");
            selectionColor: Theme.color("yellow");
            font.family: root.monofont;
            font.pixelSize: 13;
            onAccepted: {
                DebugConsole.execute(text);
                text = "";
            }
        }

        Rectangle { id: resizeButton;
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: 16;
            height: 16;
            color: Theme.color("dark1");

            // resize window mouse area
            MouseArea {
                property point offset: Qt.point(0, 0)

                anchors.fill: parent
                hoverEnabled: true
                onPressed: {
                    parent.color = Theme.color("dark2");
                    offset = Qt.point(mouseX, mouseY);
                }
                onReleased: parent.color = Theme.color("dark1")
                onPositionChanged: {
                    if (pressed) {
                        let global_pos = mapToItem(control, mouseX, mouseY);
                        control.width = global_pos.x - offset.x;
                        control.height = global_pos.y - offset.y;
                    }
                }
            }

            Image {
                source: "qrc:/icons/console/handle.png"
                anchors.centerIn: parent
            }

        }

    }

    Behavior on opacity { NumberAnimation { duration: 150; } }
}
