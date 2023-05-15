import QtQuick 2.15
import Theme 1.0

Rectangle {
    property real latitude: 0;
    property real longitude: 0;

    height: 20;
    width: txt.paintedWidth + 10;
    color: Theme.color("light0");
    radius: 10;

    Text { id: txt;
        property string s_LatSuffix: latitude > 0 ? "N" : "S";
        property string s_LonSuffix: longitude > 0 ? "E" : "W";

        anchors.centerIn: parent;
        font.family: root.mainfont;
        font.weight: Font.DemiBold;
        color: Theme.color("dark0");
        text: Number(Math.abs(latitude)).toFixed(7) + "°" + s_LatSuffix +
              ", " + Number(Math.abs(longitude)).toFixed(7) + "°" + s_LonSuffix;
    }
}