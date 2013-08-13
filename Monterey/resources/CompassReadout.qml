import QtQuick 2.0

Rectangle {
    id: currentHeadingRectangle
    z:1
    color: "#000000"
    radius: 3
    border.width: 2
    border.color: "#0275ac"

    property string currentHeading: "0"

    function setNightMode() {
        currentHeadingRectangle.border.color = "#ff0000";
        currentHeadingReadout.color = "#ff0000";
    }

    function setDayMode() {
        currentHeadingRectangle.border.color = "#0275ac";
        currentHeadingReadout.color = "#bcd5fe";
    }

    Text {
        id: currentHeadingReadout
        color: "#bcd5fe"
        z:1
        text: currentHeadingRectangle.currentHeading
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
    }
}
