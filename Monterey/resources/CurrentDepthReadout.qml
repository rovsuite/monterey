import QtQuick 2.0

Rectangle {
    id: currentDepthRectangle
    x: 77
    y: 267
    z:1
    width: 60
    height: 45
    color: "#000000"
    radius: 3
    border.width: 2
    border.color: "#2A82DA"

    property string currentDepth: "0"

    function setNightMode() {
        currentDepthRectangle.border.color = "#ff0000";
        currentDepthReadout.color = "#ff0000";
    }

    function setDayMode() {
        currentDepthRectangle.border.color = "#2A82DA";
        currentDepthReadout.color = "#ffffff";
    }

    Text {
        id: currentDepthReadout
        color: "#ffffff"
        z:1
        text: currentDepthRectangle.currentDepth
        //font.family: "Alfphabet"
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
    }
}
