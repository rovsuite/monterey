import QtQuick 2.0

Rectangle {
    id: page
    width: 180
    height: 580
    color: "#000000"
    radius: 5
    border.width: 2
    border.color: "#2A82DA"
    clip: true

    function setNightMode() {
        verticalBar.color = "#2A82DA";
        page.border.color = "#2A82DA"
    }

    function setDayMode() {
        verticalBar.color = "#2A82DA";
        page.border.color = "#2A82DA"
    }

    Rectangle {
        id: verticalBar
        objectName: "verticalBar"
        x: 50
        y: 0
        width: 3
        height: page.height
        color: "#2A82DA"
        opacity: 0
    }
}
