import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0

Dialog {
    id: dialog
    closePolicy: Popup.CloseOnEscape
    property string boxTitle: staticDesign.pageNumber === 5 ? 'Warning' : "Exit"
    property string boxCaption1: staticDesign.pageNumber !== 5 ? 'Exit Setup Wizard' : "Abort Installation"
    property string boxCaption2: staticDesign.pageNumber !== 5 ? "are you sure you want to exit the setup wizard?" : "This will close the setup wizard and installation will be aborted\n\nAre you Sure?"
    width: 400
    height: 230
    modal:true
    dim:true

    onVisibleChanged:  {
        if (visible) {
            x = (parent.width - width) / 2
            y = (parent.height - height) / 2
        }
    }

    background: Image {
        source: "image://resources/images/1.png"
        Rectangle {
            width: parent.width
            height: 34
            color: "black"
            opacity: .7
        }
        MouseArea {
            anchors.fill: parent
            property point lastMousePos: Qt.point(0, 0)
            onPressed: {
                lastMousePos = Qt.point(mouseX, mouseY)
            }
            onMouseXChanged: dialog.x += (mouseX - lastMousePos.x)
            onMouseYChanged: dialog.y += (mouseY - lastMousePos.y)
        }

        Rectangle {
            x: 0
            y: 0
            width: 120
            height: 34
            color: "black"

            Text {
                id: dialog_core_logo
                x: 9
                y: 7
                text: "Core"
                color: "white"
                font.family: defaultFont_light.name
                font.weight: Font.ExtraLight
                font.pixelSize: 19
                font.capitalization: Font.AllUppercase

                Text {
                    anchors.top: dialog_core_logo.top
                    anchors.left: dialog_core_logo.right
                    anchors.leftMargin: 1
                    text: "pack"
                    color: installer_info.themeColor
                    font: dialog_core_logo.font
                }
            }



            Image {
                source: "image://resources/images/warningSmall.png"
                x: 129
                y: 7
                width: 19
                height: 19
                ColorOverlay {
                    source: parent
                    anchors.fill: parent
                    color: installer_info.themeColor
                    smooth: true
                }
                Text {
                    id: dialogTitle
                    anchors.left: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 6
                    verticalAlignment: Text.AlignVCenter
                    text: boxTitle
                    font.family: defaultFont.name
                    font.pixelSize: 14
                    font.capitalization: Font.AllUppercase
                    color: 'white'
                }

            }

        }

        Rectangle {
            width: 400
            height: 193
            x: 0
            y : 37
            color: "black"
            opacity: .8
        }

        Rectangle {
            width: 399
            height: 38
            x: 0
            y: 57
            color: installer_info.themeColor

            Text {
                font: dialog_core_logo.font
                anchors.fill: parent
                anchors.leftMargin: 9
                verticalAlignment: Text.AlignVCenter
                text: boxCaption1
                color: "white"

            }
        }

        Text {
            x : 9
            y: 112
            font: dialog_core_logo.font
            Component.onCompleted: font.pixelSize = 12
            width: 364
            wrapMode: Text.WordWrap
            text: boxCaption2
            color: "white"
        }

        CusButton {
            text: "YES"
            x: 9
            y: 186
            onClicked: dialog.accept()
        }

        CusButton {
            text: "No"
            x: 297
            y: 186
            onClicked: dialog.reject()
        }
    }
}
