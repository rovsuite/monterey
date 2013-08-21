import QtQuick 2.0

Rectangle
{
    id: background

    property bool isStatusOn: true
    property string indicatorTitle
    property real backgroundColorOpacity: 0.35
    property int collapseHeight: 50

    color: Qt.rgba(0.14, 0.7, 0.95, backgroundColorOpacity)

    border.width: 2;
    border.color: "white"

    Text
    {
        id: title

        width: parent.width
        height: parent.height * 0.2

        anchors.top: parent.top
        anchors.topMargin: 5

        text: indicatorTitle
        font.family: "Arial"
        color: Qt.rgba(0.14, 0.7, 0.95, 0.8)
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 12
    }

    Text
    {
        id: statusText

        width: parent.width
        height: parent.height * 0.8

        anchors.top: title.bottom
        anchors.topMargin: background.height > collapseHeight ? parent.height * 0.15 : 10

        text: isStatusOn ? "On" : "Off"
        font.family: "Arial"
        color: "white"
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: background.height > collapseHeight ? 18 : 12
    }

    states: [
        State
        {
            name: "off"; when: isStatusOn == false

            PropertyChanges
            {
                target: background
                opacity: 0.4
            }
        }
    ]

    transitions: [
        Transition
        {
            from: "*"
            to: "off"

            NumberAnimation { target: background; property: "opacity"; duration: 400; easing.type: Easing.OutCubic }
        },

        Transition
        {
            from: "off"
            to: "*"

            NumberAnimation { target: background; property: "opacity"; duration: 200; easing.type: Easing.Linear }

            SequentialAnimation
            {
                NumberAnimation { target: background; property: "scale"; duration: 200; to: 1.1; easing.type: Easing.InOutQuad }
                NumberAnimation { target: background; property: "scale"; duration: 200; to: 1; easing.type: Easing.InOutQuad }
            }
        }
    ]
}
