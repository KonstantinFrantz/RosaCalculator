import QtQuick
import QtQuick.Controls

Item {
    id: displayPanel
    width: parent.width
    height: expressionText.paintedHeight + previewText.paintedHeight + 24

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

    Column {
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        spacing: 6

        Text {
            id: expressionText
            text: pretty(calculator.expression)
            font.pixelSize: 66; font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideLeft
            width: displayPanel.width
        }

        Text {
            id: previewText
            text: calculator.hasError ? calculator.errorMessage : calculator.previewResult
            font.pixelSize: 44
            color: calculator.hasError ? "#d8a2a2" : "#bbbbbb"
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideLeft
            width: displayPanel.width
        }
    }
}
