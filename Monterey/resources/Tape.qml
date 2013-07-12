import QtQuick 2.0

Rectangle {
    id: page
    width: 180
    height: 580
    color: "#000000"
    radius: 5
    border.width: 2
    border.color: "#0275ac"
    clip: true

    function setNightMode() {
        verticalBar.color = "#ff0000";
        page.border.color = "#ff0000"
    }

    function setDayMode() {
        verticalBar.color = "#0275ac";
        page.border.color = "#0275ac"
    }

    Rectangle {
        id: verticalBar
        objectName: "verticalBar"
        x: 50
        y: 0
        width: 3
        height: page.height
        color: "#0275ac"
        opacity: 0
    }
}
