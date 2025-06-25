import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CalculatorModule 1.0

Item {
    id: root

    property bool showFunctions: false
    signal historyClicked()
    signal converterClicked()

    Button {
        id: converterButton
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 15
        width: 40;  height: 40
        background: Rectangle {
            color: "transparent"
            radius: 20
        }

        contentItem: Text {
            anchors.fill: parent
            text: qsTr("₽")
            color: "#888888"
            font.pixelSize: 24
            font.weight: Font.Medium
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: root.converterClicked()
    }

    Button {
        id: historyButton
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 15
        width: 40;  height: 40
        background: Rectangle {
            color: "transparent"
            radius: 20
        }

        contentItem: Text {
            anchors.fill: parent
            text: qsTr("⟳")
            color: "#888888"
            font.pixelSize: 24
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: root.historyClicked()
    }

    ColumnLayout {
        anchors.top: historyButton.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        spacing: 15

        DisplayPanel {
            Layout.fillWidth:  true
            Layout.fillHeight: true
            Layout.minimumHeight: 125
        }

        GridLayout {
            id: functionsGrid
            columns: 5
            uniformCellWidths: true

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter

            property bool showSecondRow: false
            property bool inverseMode: false

            CalcButton {
                text: qsTr("√")
                onClicked: calculator.sqrt()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            CalcButton {
                text: qsTr("π")
                onClicked: calculator.appendConstant('π')
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            CalcButton {
                text: qsTr("^")
                onClicked: calculator.power()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            CalcButton {
                text: qsTr("!")
                onClicked: calculator.factorial()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            CalcButton {
                text: functionsGrid.showSecondRow ? qsTr("▲") : qsTr("▼")
                onClicked: functionsGrid.showSecondRow = !functionsGrid.showSecondRow
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            CalcButton {
                text: calculator.angleDegrees ? qsTr("DEG") : qsTr("RAD")
                onClicked: calculator.toggleAngleMode()
                visible: functionsGrid.showSecondRow
                Layout.fillWidth: true
                Layout.fillHeight: functionsGrid.showSecondRow
            }

            Repeater {
                model: ["sin", "cos", "tan"]

                delegate: CalcButton {
                    readonly property string baseName: modelData

                    text: {
                        if (baseName === "sin") return functionsGrid.inverseMode ? qsTr("arcsin") : qsTr("sin")
                        if (baseName === "cos") return functionsGrid.inverseMode ? qsTr("arccos") : qsTr("cos")
                        if (baseName === "tan") return functionsGrid.inverseMode ? qsTr("arctan") : qsTr("tan")
                        return baseName
                    }
                    visible: functionsGrid.showSecondRow
                    Layout.fillWidth: true
                    Layout.fillHeight: functionsGrid.showSecondRow
                    onClicked: calculator.appendFunction(text)
                }
            }

            CalcButton {
                text: qsTr("INV")
                color: functionsGrid.inverseMode ? "#55525d" : "#3d3b47"
                onClicked: functionsGrid.inverseMode = !functionsGrid.inverseMode
                visible: functionsGrid.showSecondRow
                Layout.fillWidth: true
                Layout.fillHeight: functionsGrid.showSecondRow
            }

            CalcButton {
                text: qsTr("x")
                onClicked: calculator.appendVariable()
                visible: functionsGrid.showSecondRow
                Layout.fillWidth: true
                Layout.fillHeight: functionsGrid.showSecondRow
            }

            CalcButton {
                text: qsTr("e")
                onClicked: calculator.appendConstant("e")
                visible: functionsGrid.showSecondRow
                Layout.fillWidth: true
                Layout.fillHeight: functionsGrid.showSecondRow
            }

            Repeater {
                model: ["ln", "log", "abs"]

                delegate: CalcButton {
                    text: qsTr(modelData)
                    visible: functionsGrid.showSecondRow
                    Layout.fillWidth: true
                    Layout.fillHeight: functionsGrid.showSecondRow
                    onClicked: calculator.appendFunction(text)
                }
            }
        }

        GridLayout {
            id: keypad
            columns: 4
            columnSpacing: 20
            uniformCellWidths : true

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter

            CalcButton {
                text: qsTr("AC")
                color: "#8b6f47"
                isCircle: true
                onClicked: calculator.clear()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("( )");
                color: "#55525d";
                isCircle: true
                onClicked: calculator.addParenthesis()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("%")
                color: "#55525d"
                isCircle: true
                onClicked: calculator.percentage()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("/")
                color: "#55525d"
                isCircle: true
                onClicked: calculator.appendOperator("/")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("7")
                isCircle: true
                onClicked: calculator.appendDigit("7")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("8")
                isCircle: true
                onClicked: calculator.appendDigit("8")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("9")
                isCircle: true
                onClicked: calculator.appendDigit("9")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("×")
                color: "#55525d"
                isCircle: true
                onClicked: calculator.appendOperator("*")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("4")
                isCircle: true
                onClicked: calculator.appendDigit("4")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("5")
                isCircle: true
                onClicked: calculator.appendDigit("5")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("6")
                isCircle: true
                onClicked: calculator.appendDigit("6")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("−")
                color: "#55525d"
                isCircle: true
                onClicked: calculator.appendOperator("-")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("1")
                isCircle: true
                onClicked: calculator.appendDigit("1")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("2")
                isCircle: true
                onClicked: calculator.appendDigit("2")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("3")
                isCircle: true
                onClicked: calculator.appendDigit("3")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("+")
                color: "#55525d"
                isCircle: true
                onClicked: calculator.appendOperator("+")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("0")
                isCircle: true
                onClicked: calculator.appendDigit("0")
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr(",")
                isCircle: true
                onClicked: calculator.addDecimalPoint()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: qsTr("⌫")
                isCircle: true
                onClicked: calculator.backspace()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            CalcButton {
                text: calculator.hasVariable && !calculator.hasError ? qsTr("📈") : qsTr("=")
                color: "#8b5a8c"
                isCircle: true
                onClicked: calculator.hasVariable ? calculator.requestPlot() : calculator.calculate()
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
