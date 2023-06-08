import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtLocation 5.15
import QtPositioning 5.15
import QtQuick.Layouts 1.15

import Theme 1.0
import Ruler 1.0
import ClickHandler 1.0
import Filesystem 1.0
import Config 1.0
import Images 1.0
import Network 1.0
import Markers 1.0
import RadarDiagram 1.0

import "map" as MapTab;
import "map/ui" as MapTabUI;
import "../widgets" as Widgets;

Map { id: maptab_root;
    property int i_MapMode: 1; // { 0 - offline, 5 - schema, 4 - hybrid, 1 - sattelite }
    property alias routeType: c_Route.type;

    Component.onDestruction: {
        Config.storedLatitude = center.latitude;
        Config.storedLongitude = center.longitude;
        Config.storedZoomLevel = zoomLevel;
    }

    tilt: 15;
    gesture.acceptedGestures: MapGestureArea.PanGesture | MapGestureArea.PinchGesture;
    plugin: Plugin {
        name: "osm";

        PluginParameter {
            name: "osm.mapping.providersrepository.address";
            value: "file:///" + Paths.mapConfig();
        }

        Component.onCompleted: console.info("[MAP] Using osmconfig: " + Paths.mapConfig());
    }

    activeMapType: maptab_root.supportedMapTypes[i_MapMode];
    center: QtPositioning.coordinate(Config.storedLatitude, Config.storedLongitude);
    zoomLevel: Config.storedZoomLevel;
    copyrightsVisible: false;
    z: 0;
    Behavior on center { CoordinateAnimation { duration: 250; easing.type: Easing.InOutQuad; } }

    MouseArea { id: c_MapMouseArea;
        hoverEnabled: true;
        anchors.fill: parent;
        acceptedButtons: Qt.LeftButton | Qt.RightButton;

        onClicked: {
            if(ClickHandler.state === ClickHandler.RulerActive)
            {
                if(mouse.button === Qt.LeftButton)
                    c_RulerModel.insertPoint(maptab_root.toCoordinate(Qt.point(mouseX, mouseY)), 0);
                if(mouse.button === Qt.RightButton)
                    c_RulerModel.resetRoute();
            }
            else if(ClickHandler.state === ClickHandler.MarkerActive)
            {
                if(mouse.button === Qt.LeftButton)
                {
                    let coord = maptab_root.toCoordinate(Qt.point(mouseX, mouseY));
                    markerwindow.open(coord.latitude, coord.longitude);
                }
                ClickHandler.state = ClickHandler.Idle;
            }

            else
            {
                if(mouse.button === Qt.RightButton) {
                    let coord = maptab_root.toCoordinate(Qt.point(mouseX, mouseY));
                    ClickHandler.copyCoordinatesToClipboard(coord.latitude, coord.longitude);
                }
            }
        }

        onPositionChanged: {
            let coord = toCoordinate(Qt.point(mouseX, mouseY))
            coord_tooltip.latitude = coord.latitude;
            coord_tooltip.longitude = coord.longitude;
        }
    }

    MapTab.UAV { id: c_UAV; }
    MapTab.UAVRoute { id: c_Route; visible: opacity > 0; Behavior on opacity { NumberAnimation { duration: 300; } } }
    RadarDiagram {  id: c_RadarDiagram;
        angle: 30 - Config.thetaAzimuthCorrection; // @FIXME
        uavPosition: QtPositioning.coordinate(Network.telemetry.latitude, Network.telemetry.longitude);
        azimuth: Network.telemetry.yaw;
        range: 3500; // @FIXME
        direction: Config.antennaAlignment ? 1 : 0;
        type: RadarDiagram.Telescopic;
    }

    MapPolygon { id: c_RadarDiagramView;
        property bool shown: true;
        path: c_RadarDiagram.polygon;
        border.width: 3;
        border.color: Theme.color("yellow");
        color: Qt.lighter(Theme.color("yellow"), 1.2);
        opacity: shown ? 0.2 : 0;
        Behavior on opacity { NumberAnimation { duration: 200; } }
    }

    RulerModel { id: c_RulerModel; }
    MapTab.RulerItem { id: c_Ruler; fl_LastLatitude: c_RulerModel.lastLatitude; fl_LastLongitude: c_RulerModel.lastLongitude; }

    // main image view
    MapItemView {
        model: ImagesModel;
        add: Transition { NumberAnimation { property: "m_opacity"; from: 0; to: 1; duration: 500; easing.type: Easing.OutCubic; } }
        remove: Transition { NumberAnimation { property: "m_opacity"; from: 1; to: 0; duration: 500; easing.type: Easing.OutCubic; } }
        delegate: MapTab.MapImage { }

        Connections { target: ImagesModel; function onAdded() { panel_Tools.panImage(); } }
    }

    function deleteImage(index) { ImagesModel.remove(index); }
    MapItemView {
        model: ImagesModel;
        add: Transition { NumberAnimation { property: "m_opacity"; from: 0; to: 1; duration: 500; easing.type: Easing.OutCubic; } }
        remove: Transition { NumberAnimation { property: "m_opacity"; from: 1; to: 0; duration: 500; easing.type: Easing.OutCubic; } }
        delegate: MapTab.MapImageUI { }
    }

    MapItemView {
        model: MarkersModel;
        add: Transition { NumberAnimation { property: "m_opacity"; from: 0; to: 1; duration: 500; easing.type: Easing.OutCubic; } }
        remove: Transition { NumberAnimation { property: "m_opacity"; from: 1; to: 0; duration: 500; easing.type: Easing.OutCubic; } }
        delegate: MapTab.MapMarker { }
    }

    // ui

    Widgets.CoordinateTooltip { id: coord_tooltip;
        anchors {
            top: parent.top;
            right: panel_Tools.left;
            margins: 5
            rightMargin: 15
        }
        z: 60;
        opacity: 0.85;
    }

    MapTab.AttitudeIndicator { id: attitude;
        anchors {
            bottom: parent.bottom;
            horizontalCenter: parent.horizontalCenter;
        }
        width: 250;
        implicitHeight: 150;
        pitch: Network.telemetry.pitch;
        roll: Network.telemetry.roll;
        yaw: Network.telemetry.yaw;
    }

    RoundButton { id: button_HideIndicator;
        anchors {
            bottom: attitude.top;
            bottomMargin: -7;
            horizontalCenter: attitude.horizontalCenter;
        }
        height: 40;
        radius: 4;
        icon.source: attitude.shown ? "qrc:/icons/google-material/expand-more.png"
                                    : "qrc:/icons/google-material/expand-less.png";
        icon.color: Theme.color("light0");
        font.family: root.mainfont;
        text: attitude.shown ? "" : "Авиагоризонт";
        Material.background: Material.background;
        Material.primary: Material.primary;
        Material.accent: Material.accent;
        checkable: true;
        checked: false;
        onCheckedChanged: attitude.shown = checked;
    }

    MapTabUI.PanelMainToolbar { id: panel_MainToolbar;
        anchors {
            top: parent.top
            left: parent.left
            margins: 5
        }

        opacity: 0.85
    }

    MapTabUI.PanelTools { id: panel_Tools;
        anchors {
            top: parent.top;
            right: parent.right;
            margins: 5;
        }
        opacity: 0.85;
    }

    MapTabUI.PanelEntities { id: panel_Entities;
        anchors {
            top: panel_Tools.bottom;
            topMargin: 30;
            right: parent.right;
            margins: 5;
        }
        opacity: 0.85;
    }

    MapTabUI.PanelParameters { id: panel_Parameters;
        anchors {
            bottom: parent.bottom;
            right: parent.right;
        }
        opacity: 0.85;
    }

    RoundButton { id: button_ExpandParameters;
        anchors {
            bottom: panel_Parameters.top;
            bottomMargin: -7;
            right: panel_Parameters.right;
            rightMargin: -7;
        }
        checkable: true;
        height: 40;
        radius: 4;
        icon.source: panel_Parameters.shown ? "qrc:/icons/google-material/collapse.png"
                                                 : "qrc:/icons/google-material/expand.png";
        icon.color: Theme.color("light0");
        font.family: root.mainfont;
        text: panel_Parameters.shown ? "" : "Параметры карты";
        Material.elevation: 30;
        Material.background: Material.background;
        Material.primary: Material.primary;
        Material.accent: Material.accent;
        onCheckedChanged: panel_Parameters.shown = checked;
    }

    MapTabUI.PanelImages { id: panel_ImageTools;
        anchors {
            bottom: parent.bottom;
            left: parent.left;
        }
        opacity: 0.85;
    }

    RoundButton { id: button_ToggleImageTools;
        anchors {
            bottom: panel_ImageTools.top;
            bottomMargin: -7;
            left: panel_ImageTools.left;
            leftMargin: -7;
        }
        font.family: root.mainfont;
        height: 40;
        radius: 4;
        icon.source: panel_ImageTools.shown ? "qrc:/icons/google-material/expand-more.png"
                                            : "qrc:/icons/google-material/expand-less.png";
        icon.color: Theme.color("light0");
        text: panel_ImageTools.shown ? "" : "Работа с изображениями";
        Material.elevation: 30;
        Material.background: Material.background;
        checkable: true;
        onCheckedChanged: panel_ImageTools.shown = checked;
    }
}
