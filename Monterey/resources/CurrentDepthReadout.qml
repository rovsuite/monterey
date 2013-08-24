import QtQuick 2.0

Rectangle {
    id: currentDepthRectangle
    x: 77
    y: 267
    z:1
    width: 60
    height: 45
    color: backgroundColor
    radius: 3
    border.width: 2
    border.color: borderColor

    property string currentDepth: "0"
    property color backgroundColor: "#000000"
    property color textColor: "#ffffff"
    property color borderColor: "#2A82DA"

    Text {
        id: currentDepthReadout
        color: textColor
        z:1
        text: currentDepthRectangle.currentDepth
        //font.family: "Alfphabet"
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
    }
}
