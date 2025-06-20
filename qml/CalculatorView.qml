import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CalculatorModule 1.0

Item {
    id: root
    width: parent ? parent.width  : implicitWidth
    height: parent ? parent.height : implicitHeight

    property bool showFunctions: false
    signal menuClicked()

    Button {
        id: menuButton
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 15
        width: 40;  height: 40
        background: Rectangle { color: "transparent" }

        contentItem: Text {
            anchors.fill: parent
            text: "⋮"
            color: "#888888"
            font.pixelSize: 24
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: root.menuClicked()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        DisplayPanel {
            Layout.fillWidth:  true
            Layout.fillHeight: true
            Layout.minimumHeight: 200
        }

        GridLayout {
            id: functionsGrid
            columns: 5
            rowSpacing: 10
            columnSpacing: Math.max(8, (root.width - 5 * buttonSize) / 5)

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            property int buttonSize: Math.min((root.width - 3 * 15) / 5, 80)

            property bool showSecondRow: false

            CalcButton {
                text: "√"
                fontSize: 20
                onClicked: calculator.sqrt()
                Layout.preferredWidth: functionsGrid.buttonSize
                Layout.preferredHeight: functionsGrid.buttonSize
            }

            CalcButton {
                text: "π"
                fontSize: 20
                onClicked: calculator.appendDigit("3.14159265359")
                Layout.preferredWidth: functionsGrid.buttonSize
                Layout.preferredHeight: functionsGrid.buttonSize
            }

            CalcButton {
                text: "^"
                fontSize: 20
                onClicked: calculator.power()
                Layout.preferredWidth: functionsGrid.buttonSize
                Layout.preferredHeight: functionsGrid.buttonSize
            }

            CalcButton {
                text: "!"
                fontSize: 20
                onClicked: calculator.factorial()
                Layout.preferredWidth: functionsGrid.buttonSize
                Layout.preferredHeight: functionsGrid.buttonSize
            }

            CalcButton {
                text: functionsGrid.showSecondRow ? "▲" : "▼"
                fontSize: 16
                onClicked: functionsGrid.showSecondRow = !functionsGrid.showSecondRow
                Layout.preferredWidth: functionsGrid.buttonSize
                Layout.preferredHeight: functionsGrid.buttonSize
            }

            Repeater {
                model: ["sin", "cos", "tan", "ln", "log"]

                delegate: CalcButton {
                    text: modelData
                    fontSize: 16
                    opacity: functionsGrid.showSecondRow ? 1 : 0

                    Layout.preferredWidth: functionsGrid.buttonSize
                    Layout.preferredHeight: functionsGrid.showSecondRow ? functionsGrid.buttonSize : 0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 250
                        }
                    }
                    Behavior on Layout.preferredHeight {
                        NumberAnimation {
                            duration: 250
                        }
                    }

                    onClicked: calculator.appendFunction(modelData)
                }
            }
        }

        GridLayout {
            id: keypad
            columns: 4
            rowSpacing: 15
            columnSpacing: Math.max(8, (root.width - 4 * buttonSize) / 3 - 10)

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            property real buttonSize: Math.min((root.width - 3 * 15) / 4, 80)

            CalcButton {
                text: "AC"; color: "#8b6f47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.clear()
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "( )"; color: "#55525d"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.addParenthesis()
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "%"; color: "#55525d"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.percentage()
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "÷"; color: "#55525d"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendOperator("/")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "7"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("7")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "8"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("8")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "9"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("9")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "×"; color: "#55525d"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendOperator("*")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "4"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("4")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "5"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("5")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "6"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("6")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "−"; color: "#55525d"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendOperator("-")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "1"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("1")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "2"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("2")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "3"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("3")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "+"; color: "#55525d"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendOperator("+")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "0"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.appendDigit("0")
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: ","; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.addDecimalPoint()
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "⌫"; color: "#3d3b47"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.backspace()
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
            CalcButton {
                text: "="; color: "#8b5a8c"; textColor: "#ffffff"; isCircle: true
                onClicked: calculator.calculate()
                Layout.preferredWidth: keypad.buttonSize
                Layout.preferredHeight: keypad.buttonSize
            }
        }
    }
}
