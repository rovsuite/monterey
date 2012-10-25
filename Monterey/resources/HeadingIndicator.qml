/*
    Copyright (C) 2012  Chris Konstad (chriskon149@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
