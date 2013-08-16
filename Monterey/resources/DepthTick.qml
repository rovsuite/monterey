import QtQuick 2.0

Rectangle {
    id: depthBar
    width: 72
    height: 2
    color: "#2A82DA"

    function setNightMode() {
        depthBar.color = "#ff0000";
        depthText.color = "#ff0000";
    }

    function setDayMode() {
        depthBar.color = "#2A82DA";
        depthText.color = "#2A82DA"
    }

    property string textToDisplay: "0";
    property int paintedWidth: depthText.paintedWidth;
    property int paintedHeight: depthText.paintedHeight;
    property int textX: depthText.x;

    Text {
        id: depthText
        color: "#2A82DA"
        text: textToDisplay
        font.pixelSize: 12
        y: -1 * depthText.paintedHeight/2;
        x: -1 * depthText.paintedWidth - 4;
    }
}
