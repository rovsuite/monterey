import QtQuick 2.0

Rectangle {
    id: headingBar
    width: 2
    height: 12
    color: colorToDisplay

    property string textToDisplay;
    property int paintedWidth: headingText.contentWidth;
    property color colorToDisplay: "#2A82DA";

    Text {
        id: headingText
        color: colorToDisplay
        text: textToDisplay
        font.pixelSize: 12
        y: -1 * headingText.paintedHeight - 1
        x: -1 * headingText.paintedWidth/2

    }
}
