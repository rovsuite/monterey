// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: baseItem
    width: 350
    height: 350
    color: "#00000000"

    function rotateBackground(heading) {compassBackground.rotation = -1 * heading }

        Image {
            id: compassBackground
            objectName: "compassBackground"
            x: 0
            y: 0
            width: baseItem.width
            height: baseItem.height
            fillMode: Image.PreserveAspectFit
            smooth: true
            source: "qrc:/pictures/resources/hsi_face.svg.png"
        }

        Image {
            id: compassROV
            x: compassRing.width/2 - compassROV.width/2
            y: compassRing.height/2 - compassROV.height/2
            width: 72 * compassBackground.width / 205
            height: compassROV.width
            z: 0
            smooth: true
            fillMode: Image.PreserveAspectFit
            source: "qrc:/pictures/resources/ROV.png"
        }

        Image {
            id: compassRing
            x: compassBackground.x - 3
            y: compassBackground.y - 3
            width: compassBackground.width + 6
            height: compassBackground.height + 6
            smooth: true
            z: 1
            fillMode: Image.PreserveAspectFit
            source: "qrc:/pictures/resources/any_casing.svg.png"
        }
}
