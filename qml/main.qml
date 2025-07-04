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
        calculator.plotRequested.connect(function(expr) {
            stack.push(plotPage, { "functionExpression": expr })
        })
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: calculatorPage
    }

    Component {
        id: calculatorPage
        CalculatorView {
            onHistoryClicked: stack.push(historyPage)
            onConverterClicked: stack.push(converterPage)
        }
    }

    Component {
        id: historyPage
        HistoryView {
            onBackClicked: stack.pop()
        }
    }

    Component {
        id: plotPage
        PlotView {
            onBackClicked: stack.pop()
        }
    }

    Component {
        id: converterPage
        ConverterView {
            onBackClicked: stack.pop()
        }
    }
}
