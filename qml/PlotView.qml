import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts
import CalculatorModule 1.0

Item {
    id: root
    property string functionExpression: ""
    signal backClicked()

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
        anchors.fill: parent
        width: parent.width
        clip: true
        anchors.margins: 20
        spacing: 15

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: qsTr("←")
                background: Rectangle { color: "transparent" }
                contentItem: Text { text: parent.text; color: "#ffffff"; font.pixelSize: 16 }
                onClicked: root.backClicked()
            }

            Item { Layout.fillWidth: true }

            Text {
                text: qsTr("f(x) = " + pretty(root.functionExpression))
                color: "#ffffff"
                font.pixelSize: 20
                font.bold: true
            }

            Item { Layout.fillWidth: true }
        }

        ChartView {
            id: chartView
            Layout.fillWidth: true
            Layout.fillHeight: true

            theme: ChartView.ChartThemeDark
            legend.visible: false

            ValueAxis {
                id: xAxis
                color: "#cccccc"
                labelsColor: "#ffffff"
                gridLineColor: "#2a2a2a"
                minorGridLineColor: "#1a1a1a"
                min: -10
                max: 10

                onMinChanged: {
                    if (!isUpdatingFromTextField && !isInteracting) {
                        rebuildTimer.restart()
                    }
                }
                onMaxChanged: {
                    if (!isUpdatingFromTextField && !isInteracting) {
                        rebuildTimer.restart()
                    }
                }
            }

            ValueAxis {
                id: yAxis
                color: "#cccccc"
                labelsColor: "#ffffff"
                gridLineColor: "#2a2a2a"
                minorGridLineColor: "#1a1a1a"

                onMinChanged: {
                    if (!isUpdatingFromTextField && !isInteracting) {
                        rebuildTimer.restart()
                    }
                }
                onMaxChanged: {
                    if (!isUpdatingFromTextField && !isInteracting) {
                        rebuildTimer.restart()
                    }
                }
            }

            LineSeries {
                id: xAxisLine
                color: "#cccccc"
                width: 1
                axisX: xAxis
                axisY: yAxis
            }

            LineSeries {
                id: yAxisLine
                color: "#cccccc"
                width: 1
                axisX: xAxis
                axisY: yAxis
            }

            LineSeries {
                id: functionSeries
                color: "#8b5a8c"
                width: 5
                axisX: xAxis
                axisY: yAxis
            }

            PinchArea {
                anchors.fill: parent

                property real initialXRange
                property real initialYRange
                property real initialXCenter
                property real initialYCenter

                onPinchStarted: {
                    isInteracting = true
                    initialXRange = xAxis.max - xAxis.min
                    initialYRange = yAxis.max - yAxis.min
                    initialXCenter = (xAxis.min + xAxis.max) / 2
                    initialYCenter = (yAxis.min + yAxis.max) / 2
                }

                onPinchUpdated: (pinch) => {
                    const newXRange = initialXRange / pinch.scale
                    const newYRange = initialYRange / pinch.scale

                    xAxis.min = initialXCenter - newXRange / 2
                    xAxis.max = initialXCenter + newXRange / 2
                    yAxis.min = initialYCenter - newYRange / 2
                    yAxis.max = initialYCenter + newYRange / 2

                    xMinField.text = xAxis.min.toFixed(2)
                    xMaxField.text = xAxis.max.toFixed(2)
                    yMinField.text = yAxis.min.toFixed(2)
                    yMaxField.text = yAxis.max.toFixed(2)

                    updateAxisLines()
                }

                onPinchFinished: {
                    isInteracting = false
                    rebuildTimer.restart()
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton

                    property real lastX
                    property real lastY
                    property bool isDragging: false

                    onPressed: (mouse) => {
                        lastX = mouse.x
                        lastY = mouse.y
                        isDragging = true
                        isInteracting = true
                    }

                    onPositionChanged: (mouse) => {
                        if (isDragging && pressed) {
                            const deltaX = mouse.x - lastX
                            const deltaY = mouse.y - lastY

                            const xRange = xAxis.max - xAxis.min
                            const yRange = yAxis.max - yAxis.min
                            const xDelta = -(deltaX / chartView.width) * xRange
                            const yDelta = (deltaY / chartView.height) * yRange

                            xAxis.min += xDelta
                            xAxis.max += xDelta
                            yAxis.min += yDelta
                            yAxis.max += yDelta

                            xMinField.text = xAxis.min.toFixed(2)
                            xMaxField.text = xAxis.max.toFixed(2)
                            yMinField.text = yAxis.min.toFixed(2)
                            yMaxField.text = yAxis.max.toFixed(2)

                            updateAxisLines()

                            lastX = mouse.x
                            lastY = mouse.y
                        }
                    }

                    onReleased: {
                        isDragging = false
                        isInteracting = false
                        rebuildTimer.restart()
                    }

                    onWheel: (wheel) => {
                        isInteracting = true
                        const scrollFactor = 0.01;

                        if (Math.abs(wheel.angleDelta.x) > 0) {
                            const xRange = xAxis.max - xAxis.min;
                            const deltaX = xRange * scrollFactor;

                            if (wheel.angleDelta.x > 0) {
                                xAxis.min -= deltaX;
                                xAxis.max -= deltaX;
                            } else {
                                xAxis.min += deltaX;
                                xAxis.max += deltaX;
                            }

                            xMinField.text = xAxis.min.toFixed(2);
                            xMaxField.text = xAxis.max.toFixed(2);
                        }

                        if (Math.abs(wheel.angleDelta.y) > 0) {
                            if (wheel.modifiers & Qt.ShiftModifier) {
                                const xRange = xAxis.max - xAxis.min;
                                const deltaX = xRange * scrollFactor;

                                if (wheel.angleDelta.y > 0) {
                                    xAxis.min += deltaX;
                                    xAxis.max += deltaX;
                                } else {
                                    xAxis.min -= deltaX;
                                    xAxis.max -= deltaX;
                                }

                                xMinField.text = xAxis.min.toFixed(2);
                                xMaxField.text = xAxis.max.toFixed(2);
                            } else {
                                const yRange = yAxis.max - yAxis.min;
                                const deltaY = yRange * scrollFactor;

                                if (wheel.angleDelta.y > 0) {
                                    yAxis.min += deltaY;
                                    yAxis.max += deltaY;
                                } else {
                                    yAxis.min -= deltaY;
                                    yAxis.max -= deltaY;
                                }

                                yMinField.text = yAxis.min.toFixed(2);
                                yMaxField.text = yAxis.max.toFixed(2);
                            }
                        }

                        updateAxisLines();

                        isInteracting = false
                        rebuildTimer.restart()
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 25
            Layout.rightMargin: 25

            Row {
                Layout.alignment: Qt.AlignLeft
                spacing: 10

                Text {
                    text: qsTr("X:")
                    color: "#ffffff"
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }

                TextField {
                    id: xMinField
                    text: qsTr("-10")
                    horizontalAlignment: TextInput.AlignHCenter
                    font.pixelSize: 16
                    color: "#ffffff"

                    background: Rectangle {
                        color: "#55525d"
                        radius: 4
                    }

                    onTextChanged: {
                        if (!isUpdatingFromTextField) {
                            fieldUpdateTimer.restart()
                        }
                    }
                }

                Text {
                    text: qsTr("до")
                    color: "#ffffff"
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }

                TextField {
                    id: xMaxField
                    text: qsTr("10")
                    font.pixelSize: 16
                    color: "#ffffff"
                    horizontalAlignment: TextInput.AlignHCenter

                    background: Rectangle {
                        color: "#55525d"
                        radius: 4
                    }

                    onTextChanged: {
                        if (!isUpdatingFromTextField) {
                            fieldUpdateTimer.restart()
                        }
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Row {
                Layout.alignment: Qt.AlignRight
                spacing: 10

                Text {
                    text: qsTr("Y:")
                    color: "#ffffff"
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }

                TextField {
                    id: yMinField
                    text: qsTr("-10")
                    horizontalAlignment: TextInput.AlignHCenter
                    font.pixelSize: 16
                    color: "#ffffff"

                    background: Rectangle {
                        color: "#55525d"
                        radius: 4
                    }

                    onTextChanged: {
                        if (!isUpdatingFromTextField) {
                            fieldUpdateTimer.restart()
                        }
                    }
                }

                Text {
                    text: qsTr("до")
                    color: "#ffffff"
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }

                TextField {
                    id: yMaxField
                    text: qsTr("10")
                    font.pixelSize: 16
                    color: "#ffffff"
                    horizontalAlignment: TextInput.AlignHCenter

                    background: Rectangle {
                        color: "#55525d"
                        radius: 4
                    }

                    onTextChanged: {
                        if (!isUpdatingFromTextField) {
                            fieldUpdateTimer.restart()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (root.functionExpression) {
            yAxis.min = parseFloat(yMinField.text);
            yAxis.max = parseFloat(yMaxField.text);
            updatePlot();
        }
    }

    function updateAxisLines() {
        xAxisLine.removePoints(0, xAxisLine.count);
        yAxisLine.removePoints(0, yAxisLine.count);

        const xMin = xAxis.min;
        const xMax = xAxis.max;

        if (yAxis.min <= 0 && yAxis.max >= 0) {
            xAxisLine.append(xMin, 0);
            xAxisLine.append(xMax, 0);
        }

        if (xMin <= 0 && xMax >= 0) {
            yAxisLine.append(0, yAxis.min);
            yAxisLine.append(0, yAxis.max);
        }
    }

    property bool isUpdatingFromTextField: false
    property bool isInteracting: false

    Timer {
        id: rebuildTimer
        interval: 250
        onTriggered: {
            if (!isInteracting) {
                rebuildFunction()
            }
        }
    }

    function updatePlot() {
        const xMin = parseFloat(xMinField.text) || -10;
        const xMax = parseFloat(xMaxField.text) || 10;
        const yMin = parseFloat(yMinField.text) || -10;
        const yMax = parseFloat(yMaxField.text) || 10;

        if (xMin >= xMax || yMin >= yMax) return;

        isUpdatingFromTextField = true;
        xAxis.min = xMin;
        xAxis.max = xMax;
        yAxis.min = yMin;
        yAxis.max = yMax;
        isUpdatingFromTextField = false;

        rebuildFunction();
    }

    Timer {
        id: fieldUpdateTimer
        interval: 300
        onTriggered: updatePlot()
    }

    function rebuildFunction() {
        if (!root.functionExpression) return;

        functionSeries.removePoints(0, functionSeries.count);

        const xMin = xAxis.min;
        const xMax = xAxis.max;

        const range = xMax - xMin;
        const margin = range * 0.5;
        const expandedXMin = xMin - margin;
        const expandedXMax = xMax + margin;

        const points = plotModel.generatePoints(root.functionExpression, expandedXMin, expandedXMax, 500);

        let yMin = Infinity;
        let yMax = -Infinity;
        let validPoints = 0;

        for (let i = 0; i < points.length; i++) {
            const point = points[i];
            if (point.x >= xMin && point.x <= xMax && !isNaN(point.y) && isFinite(point.y)) {
                yMin = Math.min(yMin, point.y);
                yMax = Math.max(yMax, point.y);
                validPoints++;
            }
        }

        for (let i = 0; i < points.length; i++) {
            const point = points[i];
            functionSeries.append(point.x, point.y);
        }

        updateAxisLines();
    }
}