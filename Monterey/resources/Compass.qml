import QtQuick 2.0

Rectangle {
    id: page
    width: 580
    height: 35
    color: "#000000"
    radius: 5
    border.width: 2
    border.color: borderColor
    clip: true

    property color borderColor: "#2A82DA"
}
