import QtQuick 2.12
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.1
import QtQuick.Controls 2.12

Item {
    x: 0
    y: 0
    width: 800
    height: 480
    property int pageId: 2

    Image {
        x: 279
        y: 99
        source: "image://resources/images/folder.png"
    }

    Text {
        x: 306
        y: 99
        anchors.margins: 0
        text: 'Select Desitination Folder'
        color: "white"
        font.family: defaultFont.name
        font.pixelSize: 17
        font.weight: Font.ExtraLight
        font.capitalization: Font.AllUppercase
    }

    Text {
        x: 280
        y: 127
        width: 487.55
        height: 45.81
        wrapMode: Text.Wrap
        text: 'You can select a different directory by clicking browse button.\nMake sure you have enough space, otherwise the installaion will be failed'
        font.family: defaultFont.name
        font.pixelSize: 11
        font.weight: Font.ExtraLight
        color: "white"
    }

    Rectangle {
        width: 490
        height: 25
        x: 279
        y: 168
        color: "transparent"
        border.color: installer_info.themeColor

        Text {
            anchors.fill: parent
            anchors.margins: 4
            font.family: defaultFont.name
            font.pixelSize: 14
            font.weight: Font.Light
            color: "white"
            id: destFparent

            TextMetrics {
                font: destFparent.font
                text: installer_info.destinationFolder
                elide: Text.ElideMiddle
                elideWidth: 460
                id: destF
            }

            text: destF.elidedText
        }
    }

    FolderDialog {
        id: folderDialog
        onFolderChanged: {
            installer_info.destinationFolder = folder
        }
    }

    Image {
        id: search_icon
        x: 746
        y: 171
        source: "image://resources/images/search.png"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                folderDialog.open()
            }
        }
    }

    ColorOverlay {
        anchors.fill: search_icon
        source: search_icon
        color: installer_info.themeColor
    }

    CusCheckBox {
        x: 279
        y: 204
        text: "Create Desktop"
    }

    CusCheckBox {
        x: 279
        y: 229
        text: "Start Menu ShortCut"
    }

    Image {
        source: "image://resources/images/redistrib.png"
        x: 278
        y: 259

        Text {
            text: 'REDISTRIBUTABLES'
            anchors.fill: parent
            leftPadding: parent.width + 4
            font.family: defaultFont.name
            font.pixelSize: 17
            color: "white"
            font.weight: Font.ExtraLight
        }
    }

    Item {
        width: 320
        height: 126
        x: 279
        y: 294
        id: redistPackView

        GridView {
            anchors.fill: parent
            cellWidth: 164
            cellHeight: 30
            flow: GridView.FlowTopToBottom
            model: redistPackModel
            interactive: false
            delegate: CusCheckBox2 {
                text: "Install " + model.modelData.name
                checked: model.modelData.checked
                Component.onCompleted: checked = checked
                onCheckedChanged: model.modelData.checked = checked
            }
        }
    }

    /*
    CusCheckBox2 {
        x: 281
        y: 294
        text: 'Install Direct X'
        onCheckStateChanged: {
            console.log('state')
            sastaCircle.startAngle = 90
        }
    }
    
    CusCheckBox2 {
        x: 281
        y: 324
        text: 'Install Direct X'
    }

    CusCheckBox2 {
        x: 281
        y: 354
        text: 'Install Direct X'
    }

    CusCheckBox2 {
        x: 281
        y: 384
        text: 'Install Direct X'
    }
    */
    CusArc {
        id: availDiskSpaceArc
        width: 124
        height: 124
        x: 60
        y: 105
        startAngle: 0
        endAngle: 300
        strokeColor: installer_info.themeColor
        strokeWidth: 6
        CusArc {
            id: fillDiskSpaceArc
            anchors.fill: parent
            strokeColor: 'white'
            counterclockwise: true
            strokeWidth: availDiskSpaceArc.strokeWidth
            endAngle: availDiskSpaceArc.endAngle
        }

        Text {
            text: (installer_info.diskFreeSpace * 100 / installer_info.diskTotalSpace).toFixed(
                      2) + '%'
            color: 'white'
            anchors.centerIn: parent
            font.family: defaultFont.name
            font.pixelSize: 24

            Text {
                text: readableSize(installer_info.diskTotalSpace, 0)
                color: 'white'
                anchors.top: parent.top
                anchors.topMargin: -height
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: defaultFont.name
                font.pixelSize: 12

                Image {
                    anchors.right: parent.left
                    source: 'image://resources/images/storage.png'
                }
            }

            Text {
                text: 'Free'
                anchors.top: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: defaultFont.name
                color: 'gray'
            }
        }
    }

    Image {
        source: "image://resources/images/info.png"
        x: 17
        y: 248
    }

    Text {
        text: "DISK INFO"
        font.family: defaultFont.name
        font.pixelSize: 17
        color: "white"
        x: 34
        y: 243
    }

    Item {
        x: 0
        y: 274
        width: 257
        height: 116
        ListView {
            property list<QtObject> paramModel: [
                QtObject {
                    property string title: "Required Space"
                    property string caption: readableSize(
                                                 installer_info.requiredSize)
                },
                QtObject {
                    property string title: "Available Space"
                    property string caption: readableSize(
                                                 installer_info.diskFreeSpace)
                },
                QtObject {
                    property string title: "Free Space After Installation"
                    property string caption: readableSize(
                                                 installer_info.diskFreeSpace
                                                 - installer_info.requiredSize)
                }
            ]
            anchors.fill: parent
            model: paramModel
            spacing: 8
            delegate: CusPropDelegate {
            }
        }
    }

    Rectangle {
        width: 171
        height: 59
        x: 599
        y: 359
        color: installer_info.themeColor

        MouseArea{
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
