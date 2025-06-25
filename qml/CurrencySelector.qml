import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root

    property string selectedCurrency: "RUB"
    signal currencySelected(string code)

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width * 0.9, 350)
    height: Math.min(parent.height * 0.8, 500)

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        color: "#55525d"
        radius: 20
        border.color: "#3d3b47"
        border.width: 2
    }

    enter: Transition {
            NumberAnimation {
                property: "opacity"
                from: 0.0
                to: 1.0
                duration: 200
            }
            NumberAnimation {
                property: "scale"
                from: 0.9
                to: 1.0
                duration: 200
            }
    }

    exit: Transition {
            NumberAnimation {
                property: "opacity"
                from: 1.0
                to: 0.0
                duration: 200
            }
            NumberAnimation {
                property: "scale"
                from: 1.0
                to: 0.9
                duration: 200
            }
    }

    contentItem: Column {
        spacing: 0

        Rectangle {
            id: header
            width: parent.width
            height: 60
            color: "#3d3b47"
            radius: 20

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 20
                color: "#3d3b47"
            }

            Text {
                anchors.centerIn: parent
                text: qsTr("Выберите валюту")
                color: "#ffffff"
                font.pixelSize: 20
                font.weight: Font.Medium
            }
        }

        ListView {
            id: listView
            width: parent.width
            height: parent.height - 70
            clip: true

            model: currencyManager.currencies

            ScrollBar.vertical: ScrollBar {
                visible: listView.contentHeight > listView.height
                width: 8
                policy: ScrollBar.AsNeeded
            }

            delegate: ItemDelegate {
                width: listView.width
                height: 70

                background: Rectangle {
                    color: {
                        if (modelData === root.selectedCurrency) return "#8b5a8c"
                        if (hovered) return "#3d3b47"
                        return "transparent"
                    }

                    Behavior on color {
                        ColorAnimation { duration: 150 }
                    }
                }

                contentItem: Column {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    spacing: 10

                    Text {
                        text: modelData
                        color: "#ffffff"
                        font.pixelSize: 20
                        font.weight: Font.Medium
                    }

                    Text {
                        text: currencyManager.getCurrencyName(modelData)
                        color: modelData === root.selectedCurrency ? "#dddddd" : "#888888"
                        font.pixelSize: 14
                        elide: Text.ElideRight
                        width: listView.width - 40
                    }
                }

                onClicked: {
                    root.selectedCurrency = modelData
                    root.currencySelected(modelData)
                    root.close()
                }
            }
        }
    }
}