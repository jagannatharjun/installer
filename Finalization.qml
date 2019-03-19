import QtQuick 2.12

Item {
    property int pageId: 6
    Image {
        source: "image://resources/images/info.png"
        x: 17
        y: 103

        Text {
            anchors.top: parent.top
            anchors.left: parent.right
            anchors.bottom: parent.bottom
            leftPadding: 4
            verticalAlignment: Text.AlignVCenter
            text: "INFO"
            font.family: defaultFont.name
            font.pixelSize: 18
            color: "white"
        }
    }

    Image {
        source: "image://resources/images/textBG.png"
        x: 0
        y: 128
        width: 266
        height: 50

        Text {
            anchors.fill: parent
            leftPadding: 17
            verticalAlignment: Text.AlignVCenter
            text: "If you have any issues with this repack, click the 'VISIT THREAD' button below and post your issue on Repack Topic"
            wrapMode: Text.WordWrap
            font.family: defaultFont.name
            font.pixelSize: 12
            font.weight: Font.Light
            color: "white"
        }
    }

    Text {
        x: 277
        y: 131
        width: 470
        height: 85
        wrapMode: Text.WordWrap
        font.family: defaultFont.name
        font.pixelSize: 13
        font.weight: Font.Light
        color: "white"

        text : "Repack has been installed successfully!\n\nYOU CAN LAUNCH THE GAME USING DESKTOP SHORTCUT, STARTMENU SHORTCUT OR EXECUTABLE FILE IN GAME DIRECTORY\n\nTotal Time Taken: " + installer_info.timeToStr(installer_info.totalTime())
    }

    Image {
        x: 278
        y: 97
        source: "image://resources/images/syscheck.png"

        Text {
            anchors.top: parent.top
            anchors.left: parent.right
            anchors.bottom: parent.bottom
            leftPadding: 4
            verticalAlignment: Text.AlignVCenter
            text: "INSTALLATION FINISHED"
            font.family: defaultFont.name
            font.pixelSize: 18
            color: "white"
        }
    }

    Rectangle {
        width: 171
        height: 59
        x: 599
        y: 359
        color: marea.containsMouse ? installer_info.themeColor : "#18191d"


        MouseArea{
            anchors.fill: parent
            id: marea
            onClicked: nextButtonClicked(pageId)
            hoverEnabled: true
        }
    }

    Image {
        source: "image://resources/images/arrow.png"
        x: 726
        y: 368
    }

    Text {
        width: 171
        height: 59
        x: 599
        y: 359
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: defaultFont.name
        font.pixelSize: 21
        font.weight: Font.ExtraLight
        text: 'FINISH'
        color: 'white'
    }
}
