import QtQuick
import QtQuick.Controls

Button {
    id: root

    property string color: "#3d3b47"
    property string textColor: "#ffffff"
    property real fontSize: 24
    property bool isCircle: false

    font.pixelSize: fontSize
    font.weight: Font.Normal

    background: Rectangle {
        color: root.pressed ? Qt.darker(root.color, 1.3) :
               root.hovered ? Qt.lighter(root.color, 1.2) : root.color
        radius: isCircle ? Math.min(width, height) / 2 : 8

        Behavior on scale {
            NumberAnimation { duration: 100 }
        }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
