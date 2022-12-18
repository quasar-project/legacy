import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

Pane {
    id: control
    property int radius: 10
    background: Rectangle {
        color: control.Material.backgroundColor
        radius: control.radius;

        layer.enabled: control.enabled && control.Material.elevation > 0
        layer.effect: ElevationEffect {
            elevation: control.Material.elevation
        }
    }
}
