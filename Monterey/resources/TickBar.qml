import QtQuick 2.0

Rectangle {
    id: depthBar
    width: 72
    height: 2
    color: "#2A82DA"

    function setNightMode() {
        depthBar.color = "#ff0000";
    }

    function setDayMode() {
        depthBar.color = "#2A82DA";
    }
}
