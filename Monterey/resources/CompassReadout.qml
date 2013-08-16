import QtQuick 2.0

Rectangle {
    id: currentHeadingRectangle
    z:1
    color: "#000000"
    radius: 3
    border.width: 2
    border.color: "#2A82DA"

    property string currentHeading: "0"

    function setNightMode() {
        currentHeadingRectangle.border.color = "#ff0000";
        currentHeadingReadout.color = "#ff0000";
    }

    function setDayMode() {
        currentHeadingRectangle.border.color = "#2A82DA";
        currentHeadingReadout.color = "#ffffff";
    }

    Text {
        id: currentHeadingReadout
        color: "#ffffff"
        z:1
        text: currentHeadingRectangle.currentHeading
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
    }
}
