import QtQuick 2.0

Rectangle {
    id: currentHeadingRectangle
    z:1
    color: backgroundColor
    radius: 3
    border.width: 2
    border.color: borderColor

    property string currentHeading: "0"
    property color backgroundColor: "#000000"
    property color textColor: "#ffffff"
    property color borderColor: "#2A82DA"

    Text {
        id: currentHeadingReadout
        color: textColor
        z:1
        text: currentHeadingRectangle.currentHeading
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
    }
}
