import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: displayPanel
    function pretty(str) {
        function isOp(ch) {
            return "+×÷*/^%".indexOf(ch) !== -1;
        }
        let out = "", prev = "";
        for (let i = 0; i < str.length; ++i) {
            const ch = str[i];
            const unaryMinus = (ch === '-' && (prev === "" || prev === '(' || isOp(prev) || prev === '-'));
            const afterExpSign = (ch === '+' || ch === '-') && (prev === 'e') && (i >= 2 && str[i-2] >= '0' && str[i-2] <= '9');
            const isCaret = (ch === '^');
            if (isOp(ch) || ch === '-') {
                if (afterExpSign || unaryMinus || isCaret) {
                    out += ch;
                } else {
                    out += ' ' + ch + ' ';
                }
            } else {
                out += ch;
            }
            if (ch !== ' ') prev = ch;
        }
        return out.replace(/\s+/g, ' ').trim();
    }

    ColumnLayout {
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter

        Text {
            id: expressionText
            text: qsTr(pretty(calculator.expression))
            font.pixelSize: 66; font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideLeft
            width: parent.width
            Layout.preferredWidth: displayPanel.width
            Layout.alignment: Qt.AlignRight
        }

        Text {
            id: previewText
            text: {
                if (calculator.hasError) return qsTr(calculator.errorMessage)
                if (calculator.hasVariable) return qsTr("Функция")
                return qsTr(calculator.previewResult)
            }
            font.pixelSize: 44
            color: calculator.hasError ? "#d8a2a2" : "#bbbbbb"
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideLeft
            Layout.preferredWidth: displayPanel.width
            Layout.alignment: Qt.AlignRight
            Layout.topMargin: 10
        }
    }
}
