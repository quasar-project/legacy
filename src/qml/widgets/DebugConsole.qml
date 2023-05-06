import QtQuick 2.15
import QtQuick.Controls 2.15
import ConsoleWidget 1.0

Rectangle { id: control;
    width: 600;
    height: 400;
    x: 150;
    y: 300; // относительно верхнего левого
    color: "#232323";
    border.width: 0.5;
    border.color: "#343434";
    z: 100;
    clip: true;

    FontLoader { id: monoFont; source: "qrc:/fonts/UbuntuMono.ttf"; }
    Connections
    {
        target: Impl;
        function onAppendSignal(text)
        {
            textArea.append(text);
        }
    }
    Rectangle { id: header;
        height: 20;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        color: "#343434";

        Image { id: title;
            source: "qrc:/icons/console/header.png";
            anchors.left: parent.left;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;
        }

        MouseArea { // move window mouse area
            anchors.fill: parent;
            anchors.rightMargin: 20;
            property point offset: Qt.point(0, 0);
            onPressed: {
                parent.color = "#444444";
                offset = Qt.point(mouseX, mouseY);
            }
            onReleased: {
                parent.color = "#343434";
            }
            onPositionChanged: {
                if(pressed) {
                    let global_pos = mapToItem(root, mouseX, mouseY);
                    control.x = global_pos.x - offset.x;
                    control.y = global_pos.y - offset.y;
                }
            }
        }

        Rectangle { id: closeButton;
            anchors.right: parent.right;
            anchors.top: parent.top;
            width: 20;
            height: 20;
            color: "#343434";

            MouseArea { // close window mouse area
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered: parent.color = "#563A3D";
                onExited: parent.color = "#343434";
                onClicked: {
                    root.b_ConsoleShown = false;
                }
            }

            Image {
                source: "qrc:/icons/console/cross.png";
                anchors.centerIn: parent;
            }
        }
    }

    ScrollView { id: scrollView;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: header.bottom;
        anchors.bottom: inputArea.top;
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn;

        TextArea { id: textArea;
            color: "#ECEFF4";
            background: Rectangle{
                color:"#232323";
            }
            text: "[GUI] Console initialized";
            selectByMouse: true;
            readOnly: true;
            selectedTextColor: "#2E3440";
            selectionColor: "#B48EAD";
            textFormat: Text.RichText;
            font.family: monoFont.name;
            font.pixelSize: 13;
            wrapMode: Text.NoWrap;
            function append(strAdd)
            {
                textArea.text = textArea.text + strAdd;
                textArea.cursorPosition = textArea.length - 1;
            }
        }
    }

    Rectangle { id: inputArea;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        height: 28;
        color: "#000000";
        border.width: 0.5;
        border.color: "#343434";

        TextInput {
            anchors.fill: parent;
            anchors.leftMargin: 3;
            color: "#ECEFF4";
            text: "";
            verticalAlignment: Text.AlignVCenter;
            selectByMouse: true;
            selectedTextColor: "#2E3440";
            selectionColor: "#B48EAD";
            font.family: monoFont.name;
            font.pixelSize: 13;
            onAccepted:
            {
                Impl.sendCommand(text);
                text = "";
            }
        }

        Rectangle { id: resizeButton;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;
            width: 16;
            height: 16;
            color: "#000000";

            MouseArea { // resize window mouse area
                property point offset: Qt.point(0, 0);
                anchors.fill: parent;
                hoverEnabled: true;
                onPressed: {
                    parent.color = "#343434";
                    offset = Qt.point(mouseX, mouseY);
                }
                onReleased: parent.color = "#000000";
                onPositionChanged: {
                    if(pressed) {
                        let global_pos = mapToItem(control, mouseX, mouseY);
                        control.width = global_pos.x - offset.x;
                        control.height = global_pos.y - offset.y;
                    }
                }
            }

            Image {
                source: "qrc:/icons/console/handle.png";
                anchors.centerIn: parent;
            }
        }
    }
}
