import QtQuick 2.0

Rectangle {
    id: depthBar
    width: 72
    height: 2
    color: "#0275ac"

    function setNightMode() {
        depthBar.color = "#ff0000";
    }

    function setDayMode() {
        depthBar.color = "#0275ac";
    }
}
