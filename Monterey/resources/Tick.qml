import QtQuick 2.0

Item {
    id: tick
    function setText(newText) {
        depthText.text = newText;
    }

    function setNightMode() {
        depthText.color = "#ff0000";
    }

    function setDayMode() {
        depthText.color = "#0275ac";
    }

    property string text;
    property int paintedWidth: depthText.contentWidth;

    Text {
        id: depthText
        color: "#0275ac"
        text: tick.text;
        //font.family: "Alfphabet"
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
}
