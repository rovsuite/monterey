import QtQuick 2.0

Rectangle {
    id: depthBar
    width: 72
    height: 2
    color: colorToDisplay

    property string textToDisplay: "0";
    property int paintedWidth: depthText.paintedWidth;
    property int paintedHeight: depthText.paintedHeight;
    property int textX: depthText.x;
    property color colorToDisplay: "#2A82DA"

    Text {
        id: depthText
        color: colorToDisplay
        text: textToDisplay
        font.pixelSize: 12
        y: -1 * depthText.paintedHeight/2;
        x: -1 * depthText.paintedWidth - 4;
    }
}
