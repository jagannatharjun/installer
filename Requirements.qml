import QtQuick 2.12

Item {
    property int pageId: 4

    Image {
        x: 17
        y: 103
        source: "image://resources/images/info.png"
        id: minimumTextBG

        Text {
            id: sysRqText
            text: qsTr("SYSTEM REQUIREMENTS")
            anchors.top: parent.top
            anchors.left: parent.right
            anchors.bottom: parent.bottom
            leftPadding: 4
            color: "white"
            font.family: defaultFont.name
            font.pixelSize: 18
            verticalAlignment: Text.AlignVCenter
        }
    }

    Image {
        source: "image://resources/images/textBG.png"
        x: 0
        y: 128
        width: 266
        height: 154
    }

    Text {
        x: 15
        y: 133
        id: textMinimum
        font.family: defaultFont.name
        text: "MINIMUM"
        color: installer_info.themeColor
        font.pixelSize: 14
    }

    Text {
        id: textOsMinimum
        anchors.top: textMinimum.bottom
        anchors.left: textMinimum.left
        width: 240
        topPadding: 10
        font.family: defaultFont.name
        font.weight: Font.Light
        font.pixelSize: 12
        color: installer_info.themeColor
        text: "OS: <font color='white'>" + installer_info.requirement(
                  "OS") + "</font>"
        wrapMode: Text.Wrap
    }

    Text {
        id: textProcessMinimum
        width: textOsMinimum.width
        anchors.top: textOsMinimum.bottom
        anchors.left: textOsMinimum.left
        font: textOsMinimum.font
        color: textOsMinimum.color
        text: "Processor: <font color='white'>" + installer_info.requirement(
                  "Processor") + "</font>"
        wrapMode: Text.Wrap
    }

    Text {
        id: textMemoryMinimum
        anchors.top: textProcessMinimum.bottom
        anchors.left: textProcessMinimum.left
        width: textOsMinimum.width
        font: textOsMinimum.font
        color: textOsMinimum.color
        text: "Memory: <font color='white'>" + installer_info.requirement(
                  "Memory") + "</font>"
        wrapMode: Text.Wrap
    }

    Text {
        id: textGraphicsMinimum
        anchors.top: textMemoryMinimum.bottom
        anchors.left: textMemoryMinimum.left
        width: textOsMinimum.width
        font: textOsMinimum.font
        color: textOsMinimum.color
        text: "Graphics: <font color='white'>" + installer_info.requirement(
                  "Graphics") + "</font>"
        wrapMode: Text.Wrap
    }

    Text {
        id: textStorageMinimum
        anchors.top: textGraphicsMinimum.bottom
        anchors.left: textGraphicsMinimum.left
        width: textOsMinimum.width
        font: textOsMinimum.font
        color: textOsMinimum.color
        text: "Storage: <font color='white'>" + installer_info.requirement(
                  "Storage") + "</font>"
        wrapMode: Text.Wrap
    }

    Image {
        x: 278
        y: 97
        source: "image://resources/images/syscheck.png"

        Text {
            text: "YOUR SYSTEM"
            color: "white"
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.right
            leftPadding: 10
            font.family: defaultFont.name
            font.pixelSize: 20
            verticalAlignment: Text.AlignVCenter
        }
    }

    Text {
        id: ostext
        text: "Operating System: " + installer_info.osText()
        x: 277
        y: 136
        width: 369
        color: "white"
        font.family: defaultFont.name
        font.pixelSize: 14
        font.weight: Font.Light
        wrapMode: Text.WordWrap
    }

    Text {
        id: buildNumberText
        anchors.top: ostext.bottom
        anchors.left: ostext.left
        anchors.right: ostext.right
        wrapMode: Text.WordWrap
        topPadding: 4
        text: "Build Number: " + installer_info.buildNumberText()
        font: ostext.font
        color: ostext.color
    }

    Text {
        id: cpuText
        anchors.top: buildNumberText.bottom
        anchors.left: buildNumberText.left
        anchors.right: ostext.right
        wrapMode: Text.WordWrap
        topPadding: 4
        text: "CPU: " + installer_info.cpuText()
        font: ostext.font
        color: ostext.color
    }

    Text {
        id: cpuFrequencyText
        anchors.top: cpuText.bottom
        anchors.left: cpuText.left
        anchors.right: ostext.right
        wrapMode: Text.WordWrap
        topPadding: 4
        text: "CPU Frequency: " + (installer_info.cpuFrequencyText(
                                       ) * 1e-9).toFixed(2) + "GHz"
        font: ostext.font
        color: ostext.color
    }

    Text {
        id: ramText
        wrapMode: Text.WordWrap
        anchors.top: cpuFrequencyText.bottom
        anchors.left: cpuFrequencyText.left
        anchors.right: ostext.right
        topPadding: 4
        text: "RAM: " + readableSize(installer_info.ramSize())
        font: ostext.font
        color: ostext.color
    }

    Text {
        id: gpuText
        anchors.top: ramText.bottom
        anchors.left: ramText.left
        anchors.right: ostext.right
        //anchors.right: ramText.right
        topPadding: 4
        text: "GPU: " + installer_info.gpuText()
        font: ostext.font
        color: ostext.color
        wrapMode: Text.WordWrap
    }

    Text {
        id: storageText
        anchors.top: gpuText.bottom
        anchors.left: gpuText.left
        anchors.right: ostext.right
        topPadding: 4
        text: "Storage: " + readableSize(
                  installer_info.diskFreeSpace) + " Available from " + readableSize(
                  installer_info.diskTotalSpace)
        font: ostext.font
        color: ostext.color
        wrapMode: Text.WordWrap
    }

    //    Text {
    //        x: 277
    //        y: 258
    //        id: cpuUsageText
    //        text: 'CPU: '
    //        font.family: defaultFont.name
    //        font.pixelSize: 14
    //        color: 'white'
    //    }

    //    Text {
    //        x: 277
    //        y: 312
    //        id: ramUsageText
    //        text: 'RAM: '
    //        font.family: defaultFont.name
    //        font.pixelSize: 14
    //        color: 'white'
    //    }

    //    Rectangle {
    //        x : 278
    //        y: 279
    //        width: 489
    //        height: 3
    //        color: 'white'

    //        Rectangle {
    //            anchors.left: parent.left
    //            anchors.top: parent.top
    //            anchors.bottom: parent.bottom
    //            color: installer_info.themeColor
    //            id: cpuUsageBar
    //        }
    //    }

    //    Rectangle {
    //        x : 278
    //        y: 333
    //        width: 489
    //        height: 3
    //        color: 'white'

    //        Rectangle {
    //            anchors.left: parent.left
    //            anchors.top: parent.top
    //            anchors.bottom: parent.bottom
    //            color: installer_info.themeColor
    //            id: ramUsageBar
    //        }
    //    }

    //    Timer {
    //        interval: 1000
    //        running: true
    //        repeat: true
    //        onTriggered: {
    //            cpuUsageText.text = 'CPU: ' + installer_info.cpuUsage() + '%'
    //            ramUsageText.text = 'RAM: ' + installer_info.ramUsage() + '%'
    //            cpuUsageBar.width = installer_info.cpuUsage() * 161 / 100
    //            ramUsageBar.width = installer_info.ramUsage() * 161 / 100
    //        }
    //    }
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
        x: 622
        y: 368
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
