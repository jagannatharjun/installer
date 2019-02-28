import QtQuick 2.12

Item {
    property int pageId: 3
    Image {
        source: "image://resources/images/info.png"
        x: 17
        y: 103

        Text {
            anchors.left: parent.right
            anchors.top: parent.top
            leftPadding: 4
            topPadding: -4
            anchors.topMargin: 0
            text: "INFO"
            color: "white"
            font.family: defaultFont.name
            font.pointSize: 13
        }
    }

    Image {
        source: "image://resources/images/textBG.png"
        width: 266
        height: 40
        x: 0
        y: 128

        Text {
            anchors.fill: parent
            text: "From this page, you can choose the Components you wants to install"
            anchors.margins: 4
            leftPadding: 12
            font.pointSize: 8
            font.family: defaultFont.name
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            color: "white"
        }
    }

    Image {
        source: "image://resources/images/textBG.png"
        width: 266
        height: 80
        x: 0
        y: 176

        Text {
            anchors.fill: parent
            text: "If you already have the Components downloaded and placed with the Setup.exe, you don't need to do anything. Setup will automatically select thouse Components and install them for you."
            anchors.margins: 4
            leftPadding: 12
            id: cmpHelpTxt
            font.pointSize: 8
            font.family: defaultFont.name
            wrapMode: Text.WordWrap
            color: "white"
        }
    }

    Image {
        source: "image://resources/images/textBG.png"
        width: 266
        height: 51
        x: 0
        y: 262

        Text {
            //anchors.top: parent.top
            text: "Language Pack"
            x: 15
            y: 2
            id: lgpText
            font.family: defaultFont.name
            font.pointSize: 10
            wrapMode: Text.WordWrap
            color: "gray"

            Text {
                anchors.top: parent.bottom
                topPadding: 2
                width: 260
                text: "If this Repack has any Selective Language Packs, you must install at-least one."
                wrapMode: Text.WordWrap
                color: "white"
                font.family: defaultFont.name
                font.pointSize: 8
            }
        }
    }

    Image {
        source: "image://resources/images/textBG.png"
        width: 266
        height: 51
        x: 0
        y: 318

        Text {
            x: lgpText.x
            y: lgpText.y
            text: "Optional Components"
            font.pointSize: 10
            font.family: defaultFont.name
            wrapMode: Text.WordWrap
            color: "gray"

            Text {
                anchors.top: parent.bottom
                anchors.left: parent.left
                topPadding: 2
                width: 260
                text: "These Components are optional and you can skip installing them"
                wrapMode: Text.WordWrap
                color: "white"
                font.family: defaultFont.name
                font.pointSize: 8
            }
        }
    }

    Image {
        source: "image://resources/images/folder.png"
        x: 279
        y: 99

        Text {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.right
            leftPadding: 6
            text: "LANGUAGE PACK"
            font.family: defaultFont.name
            color: "white"
            font.pixelSize: 18
            verticalAlignment: Text.AlignVCenter
        }
    }

    Item {
        width: 320
        height: 126
        x: 279
        y: 136
        id: languagePackView

        GridView {
            anchors.fill: parent
            cellWidth: 164
            cellHeight: 30
            flow: GridView.FlowTopToBottom
            model: languagePackModel
            interactive: false
            delegate: CusCheckBox2 {
                text: "Install " + model.modelData.name
                checked: model.modelData.checked
                Component.onCompleted: checked = checked
                onCheckedChanged: model.modelData.checked = checked
            }
        }
    }

    Image {
        anchors.top: languagePackView.bottom
        anchors.left: languagePackView.left
        source: "image://resources/images/folder.png"
        id: cmpPackIcon
        Text {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.right
            leftPadding: 6
            text: "OPTIONAL COMPONENTS"
            font.family: defaultFont.name
            color: "white"
            font.pixelSize: 18
            verticalAlignment: Text.AlignVCenter
        }
    }

    Item {
        width: 320
        height: 136

        anchors.top: cmpPackIcon.bottom
        anchors.left: cmpPackIcon.left
        anchors.topMargin: 16
        GridView {
            anchors.fill: parent
            cellWidth: 164
            cellHeight: 30
            flow: GridView.FlowTopToBottom
            model: componentPackModel
            interactive: false
            delegate: CusCheckBox2 {
                text: "Install " + model.modelData.name
                checked: model.modelData.checked
                Component.onCompleted: checked = checked
                onCheckedChanged: model.modelData.checked = checked
            }
        }
    }

    Rectangle {
        width: 171
        height: 59
        x: 599
        y: 359
        color: installer_info.themeColor

        MouseArea {
            anchors.fill: parent
            onClicked: nextButtonClicked(pageId)
        }
    }

    Image {
        source: "image://resources/images/arrow.png"
        x: 726
        y: 368
    }

    Text {
        x: 622
        y: 372
        font.family: defaultFont.name
        font.pixelSize: 21
        font.weight: Font.ExtraLight
        text: 'PROCEED'
        color: 'white'

        Text {
            anchors.top: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            text: 'TO THE NEXT STEP'
            color: 'white'
            font.family: defaultFont.name
            font.pixelSize: 12
            font.weight: Font.Light
        }
    }
}
