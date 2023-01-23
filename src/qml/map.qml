import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import QtLocation 5.12
import QtPositioning 5.12
import QtGraphicalEffects 1.15
import "groups" as Groups
import "sar-image" as ImageSAR
import "ui" as UI

import SMath 1.0
import MouseKeyHandler 1.0
import RecallHandler 1.0
import MarkerManager 1.0
import ImageManager 1.0

import RuntimeData 1.0
import UX 1.0

import FlightPrediction 1.0



Rectangle {
    id: qqview

    //=======================================================================================================
    //======                        ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ, ТИПЫ И КОНСТАНТЫ                           =======
    //=======================================================================================================

    //types instantiation
    SMath { id: smath; }
    MouseKeyHandler { id: mouseKeyHandler; }
    RecallHandler { id: ioHandler; }
    Predict { id: predict; }
    

    Timer { interval: 16; running: true; repeat: true; onTriggered: { update(); } }
    property bool bottomPanning: false;
    property bool topPanning: false;
    property bool leftPanning: false;
    property bool rightPanning: false;

    //ux constants
    Material.theme: Material.Dark;
    Material.accent: UX.primaryLight;
    Material.primary: UX.warningDark;
    Material.background: UX.primaryDarker;

    //ux settings
    layer.enabled: true;
    layer.samples: 4;

    //defaults constants
    property real defaultLatitude: 60.034;
    property real defaultLongitude: 30.28136;
    property real defaultZoom: 15.0;
    property int  defaultMapModeOnTestMode: 0;

    //constants related to mapItems
    property real movingThreshold: 5; //движение иконок начнется только если борт переместился более чем на 5 метров за итерацию

    property var currentQtCoordinates: QtPositioning.coordinate(defaultLatitude, defaultLongitude);

    //ruler
    property int r_currentstate: 0;
    property var r_firstpoint: QtPositioning.coordinate(0.0, 0.0);
    property var r_secondpoint: QtPositioning.coordinate(0.0, 0.0);

    //=======================================================================================================
    //======                        ОБЩИЕ ФУНКЦИИ ДЛЯ ВСЕГО QQW И ФРОНТЭНДА                           =======
    //=======================================================================================================

    //called on map component initialized
    function awake()
    {
        mapView.addMapItem(planeMapItem);
    }

    //called right after awake
    function start()
    {
        topBar.z_level = mapView.zoomLevel;
    }

    function qmlBackendStart()
    {
        if(RuntimeData.global_useOSMMaps) { defaultMapModeOnTestMode = 1; } else { defaultMapModeOnTestMode = 0; }
        //      5 = schema      4 = hybrid      1 = satellite
    }

    //called every fixed time (0.5 s default)           
    //call time equals to C_UPDATETIME in sconfig
    function fixedUpdate()
    {
        //сначала обновляем углы, потом изменяем переменную currentCoordinates. не иначе.
        updateAngles();
        currentQtCoordinates = QtPositioning.coordinate(RuntimeData.latitude, RuntimeData.longitude);

        drawRoute();
        if(RuntimeData.followPlane) panGPS(); 
    }

    //called 60 times per second (enable Timer ^^^^)
    function update()
    {
        if(bottomPanning) { mapView.pan(0, 2); }
        if(topPanning) { mapView.pan(0, -2); }
        if(leftPanning) { mapView.pan(-2, 0); }
        if(rightPanning) { mapView.pan(2, 0); }
    }

    function updateAngles()
    {
        if(Math.abs(currentQtCoordinates.distanceTo(QtPositioning.coordinate(RuntimeData.latitude, RuntimeData.longitude))) > movingThreshold)
        {
            predict.mercatorAngle = currentQtCoordinates.azimuthTo(QtPositioning.coordinate(RuntimeData.latitude, RuntimeData.longitude));
            RuntimeData.azimuthalDirection = predict.mercatorAngle;
            predict.geometricalAngle = (Math.atan2(RuntimeData.longitude - currentQtCoordinates.longitude,
                                                   RuntimeData.latitude - currentQtCoordinates.latitude) * 180)  /  Math.PI;
            RuntimeData.flatDirection = predict.geometricalAngle;
        }
        if(RuntimeData.drawPredict || RuntimeData.drawDiagram) 
        {
            predict.x0 = RuntimeData.longitude;
            predict.y0 = RuntimeData.latitude;
        }
    }

    function startEmulator()
    {
        console.log("[QML] Emulator started!");
    }

    //=======================================================================================================
    //======                        ФУНКЦИИ КОРНЕВОГО КЛАССА КАРТЫ В QML                              =======
    //=======================================================================================================

    function panImage()
    {
        console.log("[QML] Map centered on image");
        var coords = QtPositioning.coordinate(imageModel.get(imageModel.count - 1).m_lat, imageModel.get(imageModel.count - 1).m_lon);
        mapView.center = coords;
    }
    function panGPS()           { mapView.center = currentQtCoordinates; }
    function drawRoute()        { mapPolyline.addCoordinate(QtPositioning.coordinate(RuntimeData.latitude, RuntimeData.longitude)); }
    function clearRoute()       { mapPolyline.path = []; }

    //rework using mouse handler class(!)
    function changeTooltipPosition()
    {
        if(RuntimeData.drawTooltip)
        {
            if(mapMouseArea.pressed)
            {
                mapHoverCoordinatesTooltip.visible = false;
                mapHoverCoordinatesTooltip.visible = false;
            }
            else {
                mapHoverCoordinatesTooltip.visible = true;
                mapHoverCoordinatesTooltip.visible = true;
                mapHoverCoordinatesTooltip.x = mapMouseArea.mouseX;
                mapHoverCoordinatesTooltip.y = mapMouseArea.mouseY;
                var coordToStr = mapView.toCoordinate(Qt.point(mapMouseArea.mouseX,mapMouseArea.mouseY));
                mapHoverCoordinatesTooltip.tooltipText = "➤ Ш: "+coordToStr.latitude.toFixed(5)+",  Д: "+coordToStr.longitude.toFixed(5);
            }
        }
        else {
            mapHoverCoordinatesTooltip.visible = false;
            mapHoverCoordinatesTooltip.visible = false;
        }
    }

    //deprecated(!)
    function drawRuler()
    {
        rulerLine.path = [];
        rulerLine.addCoordinate(r_firstpoint);
        rulerLine.addCoordinate(r_secondpoint);
        rulerTextMapItem.visible = true;

        var cc = QtPositioning.coordinate((r_firstpoint.latitude + r_secondpoint.latitude) / 2,
                                          (r_firstpoint.longitude+r_secondpoint.longitude) / 2);
        var geoAngle = r_firstpoint.azimuthTo(r_secondpoint); //между N и выбранным вектором
        var textAngle = geoAngle + 0;
        if(textAngle > 90 & textAngle < 270) { textAngle += 180 }
        if(geoAngle > 90 & geoAngle < 270) { geoAngle += 180 }

        rulerTextMapItem.coordinate = cc;
        rulerTextMapItem.rulerRotationAngle = textAngle;
        r1MapItem.r1RotationAngle = geoAngle;
        r2MapItem.r2RotationAngle = geoAngle;
        r2MapItem.visible = true;
        r2MapItem.coordinate = r_secondpoint;
    }
    function clearRuler()
    {
        rulerLine.path = [];
        rulerTextMapItem.visible = false;
        r1MapItem.visible = false;
        r2MapItem.visible = false;
    }

    function addImage(meta, gui, base64: string, checksumSuccess: bool)
    {
        imageUIModel.append({   "m_lat": meta[0],
                                "m_lon": meta[1],
                                "m_width": meta[9],
                                "m_height": meta[10],
                                "m_latitude": gui[0],
                                "m_longitude": gui[1],
                                "m_dx": gui[2],
                                "m_dy": gui[3],
                                "m_x0": gui[4],
                                "m_y0": gui[5],
                                "m_angle": gui[6],
                                "m_driftAngle": gui[7],
                                "m_lx": gui[8],
                                "m_ly": gui[9],
                                "m_azimuth": gui[10],
                                "m_hash": gui[11],
                                "m_filename": gui[12],
                                "m_datetime": gui[13],
                                "m_checksumMatch": checksumSuccess ? "<font color=\"#73c924\">да</font>" : "<font color=\"#b16573\">нет</font>",
                                "m_visible": false
                            });
        imageModel.append({     "m_lat": meta[0],
                                "m_lon": meta[1],
                                "m_x0": meta[4],
                                "m_y0": meta[5],
                                "m_width": meta[9],
                                "m_height": meta[10],
                                "m_angle": meta[6],
                                "m_driftAngle": meta[7],
                                "m_base64": base64, //base64 строка
                                "m_zoom": smath.mercatorZoomLevel(1, meta[0]),
                                "m_i_visible": true
                            });
    }

    function addMarker(name: string, col: color, icon: string, latitude: real, longitude: real, anchorX: real, anchorY: real, zoomLevel: real){
        markerModel.append({    "m_name": name, 
                                "m_color": String(col), 
                                "m_qrc": icon, 
                                "lat": latitude, 
                                "lon": longitude, 
                                "anchorX": anchorX,
                                "anchorY": anchorY,
                                "m_zoom": zoomLevel,
                                "removeTrigger": false
                            });
    }

    function markerRemove(i)               { MarkerManager.removeMarker(i); }
    function markerRemoveFromBackend(i)    { markerModel.setProperty(i, "removeTrigger", true)  }
    function imageRemove(i) {
        var b = ImageManager.removeImage(i);
        if(b) { return true; } 
        else { return false; }
    }

    ListModel { id: imageModel; }
    ListModel { id: imageUIModel; }
    ListModel { id: markerModel; }
    
    property bool markerPlacementCursorChange: false;

    Map {
        MouseArea {
            id: mapMouseArea;
            anchors.fill: parent;
            hoverEnabled: true;
            propagateComposedEvents: true;
            cursorShape: markerPlacementCursorChange ? Qt.PointingHandCursor : Qt.CrossCursor;
            //preventStealing: true; //никогда не включать это на основной карте
            acceptedButtons: Qt.LeftButton | Qt.RightButton;
            z: 0;
            onEntered: {
                mapHoverCoordinatesTooltip.visible = true;
            }
            onExited: {
                mapHoverCoordinatesTooltip.visible = false;
            }
            onPositionChanged: {
                changeTooltipPosition();
                if(r_currentstate === 2)
                {
                    drawRuler();
                    r_secondpoint = mapView.toCoordinate(Qt.point(mapMouseArea.mouseX,mapMouseArea.mouseY));
                    rulerText.text = r_firstpoint.distanceTo(r_secondpoint).toLocaleString(Qt.locale("ru_RU"), 'f', 0) + " м";
                }
                if(mouseKeyHandler.mouseState === 1)
                {
                    markerPlacementCursorChange = true;
                } else { markerPlacementCursorChange = false; }
            }
            onClicked:
            {
                forceActiveFocus();
                if(r_currentstate !== 0 & mouse.button === Qt.RightButton)
                {
                    r_currentstate = 0;
                    clearRuler();
                }
                else if(r_currentstate === 1 & mouse.button === Qt.LeftButton)
                {
                    r_firstpoint = mapView.toCoordinate(Qt.point(mapMouseArea.mouseX,mapMouseArea.mouseY));
                    r1MapItem.coordinate = r_firstpoint;
                    r1MapItem.visible = true;
                    r_currentstate = 2;
                    r_secondpoint = r_firstpoint;
                }
                else if(r_currentstate === 2 & mouse.button === Qt.LeftButton)
                {
                    r_secondpoint = mapView.toCoordinate(Qt.point(mapMouseArea.mouseX,mapMouseArea.mouseY));
                    r_currentstate = 3;

                    r2MapItem.visible = true;
                }
                else if(r_currentstate === 3)
                {
                    r_currentstate = 0;
                    clearRuler();
                }
                if(r_currentstate === 0 & mouse.button === Qt.RightButton)
                {
                    mouseKeyHandler.copyCoordinates(mapView.toCoordinate(Qt.point(mapMouseArea.mouseX,mapMouseArea.mouseY)).latitude, mapView.toCoordinate(Qt.point(mapMouseArea.mouseX,mapMouseArea.mouseY)).longitude);
                }
                if(mouseKeyHandler.mouseState === 1 & mouse.button === Qt.LeftButton)
                {
                    mouseKeyHandler.placeMarker(mapView.toCoordinate(Qt.point(mapMouseArea.mouseX,mapMouseArea.mouseY)).latitude, mapView.toCoordinate(Qt.point(mapMouseArea.mouseX,mapMouseArea.mouseY)).longitude);
                }
                mouse.accepted = false;
            }
        } 

        id: mapView
        anchors.fill: parent;
        layer.smooth: true;
        tilt: 15;
        gesture.acceptedGestures: MapGestureArea.PanGesture | MapGestureArea.PinchGesture;
        plugin: Plugin {
            id: mapPluginID;
            name: "osm";
            PluginParameter {
                id: parameterOSM;
                name: "osm.mapping.providersrepository.address";
                value: "file:///" + OsmConfigPath; //variable being set in CoreUI.cpp
            }
            Component.onCompleted: console.info("[MAP] using osmconfig: " + OsmConfigPath);
        }
        
        activeMapType: mapView.supportedMapTypes[defaultMapModeOnTestMode]
        center: QtPositioning.coordinate(defaultLatitude, defaultLongitude);
        zoomLevel: defaultZoom;
        copyrightsVisible: false;
        z: 0;

        Component.onCompleted: { awake(); start(); }

        Behavior on center { CoordinateAnimation { duration: 1000; easing.type: Easing.Linear } }
        Behavior on zoomLevel { NumberAnimation { duration: 100 } }
        onZoomLevelChanged: topBar.z_level = mapView.zoomLevel;

        MapItemView
        {
            model: imageModel;
            add: Transition {
                    NumberAnimation {
                        property: "m_opacity";
                        from: 0;
                        to: 1;
                        duration: 2000;
                        easing.type: Easing.OutCubic;
                    }
            }
            remove: Transition {
                        NumberAnimation {
                            property: "m_opacity";
                            from: 1;
                            to: 0;
                            duration: 2000;
                            easing.type: Easing.OutCubic;
                        }
            }
            delegate: ImageSAR.MapImage { }
        }

        MapPolyline { id: mapPolyline; line.width: 5; 
                    opacity: RuntimeData.drawRoute ? 0.75 : 0; line.color: UX.warningLighter; path: [ ];
                    Behavior on opacity { NumberAnimation { duration: 1000; } } 
        }
        MapPolyline { id: predictLine; line.width: 3; opacity: RuntimeData.drawPredict ? 0.4 : 0; line.color: UX.warningLight; z: 1;
                      path: [ { latitude: predict.y0, longitude: predict.x0 }, { latitude: predict.y10, longitude: predict.x10 } ]; 
                      Behavior on opacity { NumberAnimation { duration: 1000 } } }
        MapPolygon {  property var point0: QtPositioning.coordinate(predict.y0, predict.x0);
                      property var point1: QtPositioning.coordinate(predict.y1, predict.x1);
                      property var point2: QtPositioning.coordinate(predict.y2, predict.x2);
                      property var point3: QtPositioning.coordinate(predict.y3, predict.x3);
                      id: predictPoly; border.width: 3; opacity: RuntimeData.drawDiagram ? 0.4 : 0; border.color: UX.warningLight; color: UX.warningLighter; z: 1;
                      path: [ point0,
                              point2,
                              point1,
                              point3
                            ];

                      Behavior on opacity { NumberAnimation { duration: 1000 } }
        }
        MapItemView
        {
            model: imageUIModel;
            add: Transition {
                    NumberAnimation {
                        property: "m_scale";
                        from: 0;
                        to: 1;
                        duration: 500;
                        easing.type: Easing.OutCubic;
                    }
            }
            remove: Transition {
                        NumberAnimation {
                            property: "m_opacity";
                            from: 1;
                            to: 0;
                            duration: 2000;
                            easing.type: Easing.OutCubic;
                        }
            }
            delegate: ImageSAR.Dialog { }
        }
        MapItemView
        {
            model: markerModel;
            add: Transition {
                    NumberAnimation {
                        property: "m_opacity";
                        from: 0;
                        to: 1;
                        duration: 2000;
                        easing.type: Easing.OutCubic;
                    }
            }
            remove: Transition {
                        NumberAnimation {
                            property: "m_opacity";
                            from: 1;
                            to: 0;
                            duration: 2000;
                            easing.type: Easing.OutCubic;
                        }
            }
            delegate: Marker { }
        }
        MapQuickItem {
            property alias rulerRotationAngle: rulerRotation.angle
            id: rulerTextMapItem
            visible: false
            width: 2
            height: 2
            transform: Rotation {
                id: rulerRotation
                origin.x: rulerText.width / 2;
                origin.y: rulerText.height / 2;
                angle: 0
            }
            anchorPoint.x: rulerText.width / 2
            anchorPoint.y: rulerText.height / 2
            z:10
            sourceItem: Text {
                id: rulerText;
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                style: Text.Outline
                font.family: "Verdana"
                color: UX.textWhite;
                text: "";
            }
        }
        MapQuickItem {
            property alias r1RotationAngle: r1Rotation.angle
            id: r1MapItem
            visible: false
            width: 2
            height: 2
            transform: Rotation {
                id: r1Rotation
                origin.x: r1Source.width;
                origin.y: r1Source.height/2;
                angle: 0
            }
            anchorPoint.x: r1Source.width
            anchorPoint.y: r1Source.height/2
            z: 10;
            sourceItem: Image {
                id: r1Source;
                layer.enabled: true
                transformOrigin: Item.Right
                source: "qrc:/ui-resources/qml/temp.png"
            }
            ColorOverlay {
                id: r1Overlay;
                anchors.fill: r1MapItem;
                source: r1Source;
                opacity: 1;
                color: UX.textWhites;
            }
        }
        MapQuickItem {
            property alias r2RotationAngle: r2Rotation.angle;
            id: r2MapItem;
            visible: false;
            width: 2;
            height: 2;
            transform: Rotation {
                id: r2Rotation
                origin.x: r2Source.width;
                origin.y: r2Source.height / 2;
                angle: 0;
            }
            anchorPoint.x: r2Source.width;
            anchorPoint.y: r2Source.height / 2;
            z: 10;
            sourceItem: Image {
                id: r2Source;
                layer.enabled: true;
                transformOrigin: Item.Right;
                source: "qrc:/ui-resources/qml/temp.png";
            }
            ColorOverlay {
                id: r2Overlay;
                anchors.fill: r2MapItem;
                source: r2Source;
                opacity: 1;
                color: UX.textWhite;
            }
        }
        MapQuickItem {
            property alias rotationAngle: rotation.angle;
            id: planeMapItem;
            anchorPoint.x: 20;
            anchorPoint.y: 20;
            transform: Rotation {
                id: rotation;
                origin.x: 20;
                origin.y: 20;
                angle: predict.mercatorAngle;
            }
            coordinate: QtPositioning.coordinate(RuntimeData.latitude, RuntimeData.longitude);
            z: 18;
            sourceItem: Item {
                Image {
                    id: planeSource;
                    layer.enabled: true;
                    transformOrigin: Item.Center;
                    smooth: true;
                    source: "qrc:/ui-resources/qml/gpsarrow.png";
                    visible: false;
                }
                ColorOverlay {
                    id: overlayPlane;
                    anchors.fill: planeSource;
                    source: planeSource;
                    opacity: 0.75;
                    color: UX.warningLighter;
                }
                DropShadow {
                    anchors.fill: overlayPlane;
                    horizontalOffset: 5;
                    verticalOffset: 5;
                    radius: 8.0;
                    samples: 17;
                    color: "#000000";
                    source: overlayPlane;
                }
                Glow {
                    anchors.fill: overlayPlane;
                    radius: 5;
                    samples: 17;
                    color: UX.warningLight;
                    spread: 0.5;
                    transparentBorder: true;
                    source: overlayPlane;
                }
            }
            Behavior on coordinate {
                CoordinateAnimation {
                    duration: 250;
                    easing.type: Easing.Linear;
                }
            }
        }
        MapPolyline { id: rulerLine; line.width: 4; opacity: 0.8; line.color: UX.textWhite; z: 100; path: [ ]; } //192.168.18.143

        DropShadow { z: 99; anchors.fill: topBar; horizontalOffset: -12; verticalOffset: 9; radius: 16;
                     samples: 32; color: "#80000000"; source: topBar; cached: true; }
        Groups.TopBar
        {
            id: topBar;
            anchors.top: parent.top;
            anchors.left: parent.left; anchors.right: parent.right;
            z: 100;
        }

        DropShadow { z: 99; anchors.fill: bottomBar; horizontalOffset: -12; verticalOffset: -9; radius: 16;
                     samples: 32; color: "#80000000"; source: bottomBar; cached: true; }
        Groups.BottomBar
        {
            id: bottomBar;
            anchors.bottom: parent.bottom;
            anchors.left: parent.left; anchors.right: parent.right;
            z: 100;
        }

        Rectangle { id: terminalOutline; color: UX.primaryDark; width: 5; anchors.right: parent.right; anchors.top: topBar.bottom;
                    anchors.bottom: bottomBar.top; }

        Groups.EmulatorTextPanel
        {
            id: emulatorTextPanel;
            visible: RuntimeData.global_emulatorEnabled;
            anchors.top: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 3;
        }

        FloatingTooltip
        {
            id: mapHoverCoordinatesTooltip;
            visible: true;
            tooltipPadding: 10;
            tooltipText: "Ш: 50.0000000 Д: 30.0000000";
            transparency: 0.5;
        }
        Text {
            id: closestACDistanceIndicator;
            color: UX.textWhite;
            font.capitalization: Font.MixedCase;
            font.pixelSize: 12;
            textFormat: Text.RichText
            opacity: RuntimeData.autocaptureEnabled ? 1 : 0;
            text: "Расстояние до ближайшей точки автозахвата: <b><i>" + Number(RuntimeData.autocaptureDistance / 1000).toFixed(3) + "</i></b> км";
            horizontalAlignment: Text.AlignLeft;
            verticalAlignment: Text.AlignVCenter;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.bottom: parent.bottom;
            anchors.margins: 23;
            Behavior on opacity { NumberAnimation { duration: 1000; } }
        }
        UI.StatusText
        {
            id: flightStatus;
            visible: false;
            status: "@в разработке@";
            prefix: "Статус полёта: ";
            anchors.right: parent.horizontalCenter;
            anchors.top: parent.top;
            anchors.topMargin: 25+70;
            anchors.rightMargin: 10;
        }
        UI.StatusText
        {
            id: formStatus;
            status: RuntimeData.formStatus;
            prefix: "Статус формирования: ";
            //anchors.left: parent.horizontalCenter;
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top;
            anchors.topMargin: 25+70;
            //anchors.leftMargin: 10;
        }
    }
    Connections {

    }
}


