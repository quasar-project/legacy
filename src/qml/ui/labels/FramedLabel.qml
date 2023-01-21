import QtQuick 2.15

Rectangle {
    property int fixed_width: 100;
    property int fixed_height: 25;

    property string label_text: "Sample text";
    property color label_color: "#de3857";
    property int label_text_size: 12;
    property string label_text_family: "Helvetica [Cronyx]";
    property bool label_text_bold: false;
    property int frame_radius: 8;
    property int frame_width: 1;
    property int label_textAlignment: Text.AlignHCenter;

    id: frame;
    color: "transparent";
    border.width: frame_width;
    border.color: label_color;
    radius: frame_radius;
    width: fixed_width;
    height: fixed_height;
    Text {
        text: label_text;
        font.capitalization: Font.MixedCase;
        font.pixelSize: label_text_size;
        font.family: label_text_family;
        font.bold: label_text_bold;
        color: label_color;
        opacity: enabled ? 1.0 : 0.3;
        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: label_textAlignment;
        lineHeight: 0.75;
        anchors.fill: parent;
    }
}