import QtQuick 2.15
import QtLocation 5.15
import QtPositioning 5.15
import Theme 1.0

MapItemView {
    id: orthodromSegments;
    z: 100;
    visible: true;
    model: c_RulerModel;
    delegate: MapItemGroup {
        id: orthodromSegmentsItems;

        MapPolyline {
            id: segmentItem;
            line.color: Theme.color("color3");
            line.width: 5;
            antialiasing: true;
            path: segment;
        }

        MapQuickItem {
            coordinate: segmentCenter;
            anchorPoint.x: segmentLegthItem.width / 2;
            anchorPoint.y: segmentLegthItem.height / 2;
            transform: Rotation {
                id: rotationSegment;
                origin.x: segmentLegthItem.width / 2;
                origin.y: segmentLegthItem.height / 2;
                angle: c_RulerModel ? c_RulerModel.calculateAngle(segmentItem.path[0], segmentItem.path[segmentItem.path.length - 1]) : 0;
            }

            sourceItem: Rectangle {
                id: segmentLegthItem;
                width: segmentLengthText.width * 1.1;
                height: segmentLengthText.height * 1.1;
                radius: 4;
                border.width: 1;
                border.color: Theme.color("color2");
                color: Theme.color("color2");
                visible: true;

                Text {
                    id: segmentLengthText;
                    anchors.centerIn: parent;
                    text: segmentLength < 1000 ? segmentLength + " м" : segmentLength / 1000 + " км"
                    font.family: root.s_FontBold;
                    font.bold: true;
                    color: Theme.color("dark0");
                }
            }
        }
    }
}
