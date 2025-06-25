import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CalculatorModule 1.0

Item {
    id: root
    signal backClicked()

    RowLayout {
        id: header
        anchors { top: parent.top; left: parent.left; right: parent.right; }
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 10

        Button {
            width: 40;  height: 40
            background: Rectangle {
                color: "transparent"
                radius: 20
            }

            contentItem: Text {
                anchors.fill: parent
                text: qsTr("←");
                color: "#888888"
                font.pixelSize: 24
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: root.backClicked()
        }
        Text {
            text: qsTr("История");
            color: "#ffffff"
            font.pixelSize: 24
            font.weight: Font.Medium
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
        }
        Button {
            width: 40;  height: 40
            background: Rectangle {
                color: "transparent"
                radius: 20
            }

            contentItem: Text {
                anchors.fill: parent
                text: qsTr("🗑");
                color: "#888888"
                font.pixelSize: 24
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: historyManager.clearHistory()
        }
    }

    ListView {
        id: listView
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom; topMargin: 8 }
        clip: true
        model: historyManager

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

        delegate: Rectangle {
            width: listView.width; height: 96
            color: index % 2 ? "#3b3944" : "#35333e"

            ColumnLayout {
                anchors {
                    left: parent.left; right: delBtn.left
                    leftMargin: 16; rightMargin: 8
                    verticalCenter: parent.verticalCenter
                }
                spacing: 4

                Label {
                    text: qsTr(listView.pretty(expression))
                    color: "#ffffff"; font.pixelSize: 24
                    elide: Label.ElideRight
                }
                Label {
                    text: qsTr(result)
                    color: "#cfcfcf"; font.pixelSize: 20
                    elide: Label.ElideRight
                }
            }

            Button {
                id: delBtn
                width: 40; height: parent.height
                anchors.right: parent.right
                anchors.rightMargin: 15
                background: Rectangle { color: "transparent" }
                contentItem: Text { text: "✕"; color: "#ff5c5c"; font.pixelSize: 20
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter }
                onClicked: historyManager.removeCalculation(index)
            }

            MouseArea {
                anchors { left: parent.left; right: delBtn.left; top: parent.top; bottom: parent.bottom }
                onClicked: {
                    calculator.loadExpression(expression)
                    root.backClicked()
                }
            }
        }

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
    }
}
