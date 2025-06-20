import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CalculatorModule 1.0

ApplicationWindow {
    id: window
    width: 400
    height: 700
    visible: true
    title: qsTr("Калькулятор")
    color: "#3d3b47"

    Component.onCompleted: {
        calculator.calculationPerformed.connect(historyManager.addCalculation)
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: calculatorPage
    }

    Component {
        id: calculatorPage
        CalculatorView {
            onMenuClicked: stack.push(historyPage)
        }
    }

    Component {
        id: historyPage
        HistoryView {
            onBackClicked: stack.pop()
        }
    }
}
