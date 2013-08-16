import QtQuick 2.0

Rectangle {
    id: page
    width: 580
    height: 35
    color: "#000000"
    radius: 5
    border.width: 2
    border.color: "#2A82DA"
    clip: true

    function setNightMode() {
        page.border.color = "#ff0000"
    }

    function setDayMode() {
        page.border.color = "#2A82DA"
    }
}

