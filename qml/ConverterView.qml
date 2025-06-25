import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CalculatorModule 1.0

Item {
    id: root

    signal backClicked()

    property string fromCurrency: "RUB"
    property string toCurrency: "USD"
    property real amount: 0

    function swapCurrencies() {
        var temp = fromCurrency
        fromCurrency = toCurrency
        fromSelector.selectedCurrency = toCurrency
        toSelector.selectedCurrency = temp
        toCurrency = temp
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: qsTr("←")
                background: Rectangle { color: "transparent" }
                contentItem: Text {
                    text: parent.text
                    color: "#ffffff"
                    font.pixelSize: 24
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: root.backClicked()
            }

            Item { Layout.fillWidth: true }

            Text {
                text: qsTr("Конвертер валют")
                color: "#ffffff"
                font.pixelSize: 24
                font.weight: Font.Medium
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Item { Layout.fillWidth: true }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Column {
                anchors.centerIn: parent
                width: parent.width
                spacing: 40

                Column {
                    width: parent.width
                    spacing: 15

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 20

                        TextInput {
                            id: fromInput
                            text: amount.toString()
                            color: "#ffffff"
                            font.pixelSize: 72
                            font.weight: Font.Light
                            horizontalAlignment: Text.AlignRight

                            validator: DoubleValidator {
                                bottom: 0
                                top: 9999999999
                                decimals: 2
                                notation: DoubleValidator.StandardNotation
                            }

                            onTextEdited: {
                                var value = parseFloat(text)
                                amount = isNaN(value) || text === "" ? 0 : value
                            }

                            onEditingFinished: {
                                if (text === "" || parseFloat(text) === 0) {
                                    text = "0"
                                }
                            }
                        }

                        MouseArea {
                            width: childrenRect.width
                            height: childrenRect.height
                            onClicked: fromSelector.visible = true

                            Row {
                                spacing: 5

                                Text {
                                    text: fromCurrency
                                    color: "#888888"
                                    font.pixelSize: 36
                                    font.weight: Font.Medium
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                Text {
                                    text: "▼"
                                    color: "#888888"
                                    font.pixelSize: 20
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: currencyManager.getCurrencyName(fromCurrency)
                        color: "#666666"
                        font.pixelSize: 14
                        elide: Text.ElideRight
                        width: parent.width * 0.8
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 60
                    height: 60

                    background: Rectangle {
                        color: parent.pressed ? Qt.darker("#8b6f47", 1.2) : "#8b6f47"
                        radius: 30
                    }

                    contentItem: Text {
                        text: "⇅"
                        color: "#ffffff"
                        font.pixelSize: 28
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: swapCurrencies()
                }

                Column {
                    width: parent.width
                    spacing: 15

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 20

                        Text {
                            id: resultText
                            text: {
                                const result = currencyManager.convert(fromCurrency, toCurrency, amount);
                                if (Math.abs(result - Math.round(result)) < 0.01) {
                                    return Math.round(result).toString()
                                }
                                return result.toFixed(2)
                            }
                            color: "#ffffff"
                            font.pixelSize: 72
                            font.weight: Font.Light
                        }

                        MouseArea {
                            width: childrenRect.width
                            height: childrenRect.height
                            onClicked: toSelector.visible = true

                            Row {
                                spacing: 5

                                Text {
                                    text: toCurrency
                                    color: "#888888"
                                    font.pixelSize: 36
                                    font.weight: Font.Medium
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                Text {
                                    text: "▼"
                                    color: "#888888"
                                    font.pixelSize: 20
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: currencyManager.getCurrencyName(toCurrency)
                        color: "#666666"
                        font.pixelSize: 14
                        elide: Text.ElideRight
                        width: parent.width * 0.8
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#55525d"
            radius: 25

            Text {
                anchors.centerIn: parent
                text: {
                    var rate = currencyManager.convert(fromCurrency, toCurrency, 1)
                    return "1 " + fromCurrency + " = " + rate.toFixed(4) + " " + toCurrency
                }
                color: "#ffffff"
                font.pixelSize: 16
            }
        }
    }

    CurrencySelector {
        id: fromSelector
        parent: root
        visible: false
        selectedCurrency: fromCurrency
        onCurrencySelected: function(code) {
            fromCurrency = code
        }
    }

    CurrencySelector {
        id: toSelector
        parent: root
        visible: false
        selectedCurrency: toCurrency
        onCurrencySelected: function(code) {
            toCurrency = code
        }
    }
}