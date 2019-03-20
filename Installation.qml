import QtQuick 2.12

Item {
    x: 0
    y: 0
    width: 800
    height: 480

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
            text: "INSTALLATION PROGRESS"
            font.family: defaultFont.name
            font.pixelSize: 18
            color: "white"

        }
    }

    Text {
        x: 277
        y: 131
        id: currentProgressText
        font.family: defaultFont.name
        font.pixelSize: 13
        color: "white"
        font.weight: Font.Light
        font.capitalization: Font.AllUppercase
    }

    Text {
        y: 180
        anchors.left: progressBar.left
        id: estimatedTimeText
        text: "Estimated Time: " 
        font.family: defaultFont.name
        font.pixelSize: 13
        color: "white"
        font.weight: Font.Light
    }

    Text {
        y: 180
        anchors.right: progressBar.right
        id: remainingTimeText
        font.family: defaultFont.name
        font.pixelSize: 13
        color: "white"
        font.weight: Font.Light
    }

    

    Image {
        source: "image://resources/images/loop.png"
        x: 278
        y: 210

        Text {
            anchors.top: parent.top
            anchors.left: parent.right
            anchors.bottom: parent.bottom
            width: progressBar.width - parent.width
            verticalAlignment: Text.AlignVCenter
            leftPadding: 6
            id: statusText
            font.family: defaultFont.name
            font.pixelSize: 14
            color: "white"
            font.weight: Font.Light
            TextMetrics {
                font: statusText.font
                text: installer_info.statusMessage
                elide: Text.ElideRight
                elideWidth: statusText.width
                id: elidedStatus
            }
            text:  elidedStatus.elidedText
        }
    }

    Image {
        x: 278
        y: 249
        source: "image://resources/images/doubleWheel.png"

        Text {
            anchors.top: parent.top
            anchors.left: parent.right
            anchors.bottom: parent.bottom
            verticalAlignment: Text.AlignVCenter
            leftPadding: 6
            text: "RESOURCE USAGE"
            font.family: defaultFont.name
            font.pixelSize: 18
            color: "white"
        }
    }

    CusArc {
        width: 101
        height: 101
        x: 283
        y: 298
        strokeWidth: 4
        strokeColor: installer_info.themeColor
        id: cpuUsageArc

        CusArc {
            anchors.fill: parent
            strokeColor: 'white'
            endAngle: cpuUsageArc.endAngle - 360
            counterclockwise: true
        }

        Text {
            anchors.centerIn: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            id: cpuUsageText
            property int usage: 0
            onUsageChanged: {
                text = "CPU\n" + usage + '%'
                cpuUsageArc.endAngle = 360 * usage / 100.0
            }
            text: "CPU\n10%"
            font.family: defaultFont.name
            font.pixelSize: 18
            color: "white"
            topPadding: 24
        }
    }
    Image {
        source: "image://resources/images/cpu.png"
        x: 324
        y: 317
    }

    CusArc {
        width: 101
        height: 101
        x: 444
        y: 298
        id: ramUsageArc
        strokeColor: installer_info.themeColor
        strokeWidth: 4

        CusArc {
            anchors.fill: parent
            strokeColor: 'white'
            endAngle: ramUsageArc.endAngle - 360
            counterclockwise: true
        }

        Text {
            anchors.centerIn: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            id: ramUsageText
            property int usage: 0
            onUsageChanged: {
                text = "RAM\n" + usage + '%'
                ramUsageArc.endAngle = 360 * usage / 100.0
            }
            text: "RAM\n10%"
            font.family: defaultFont.name
            font.pixelSize: 18
            color: "white"
            topPadding: 24
        }
    }

    Image {
        source: "image://resources/images/ram.png"
        x: 484
        y: 318
    }

    Timer {
        interval: 256
        repeat: true
        running: true
        onTriggered: {
            cpuUsageText.usage = installer_info.cpuUsage()
            ramUsageText.usage = installer_info.ramUsage()
            estimatedTimeText.text = "Estimated Time: " + installer_info.timeToStr(installer_info.totalTime())
            remainingTimeText.text = "Remaining Time: " + installer_info.timeToStr(installer_info.remainingTime())
            currentProgressText.text = "Current Progress: " + installer_info.getProgress().toFixed(2) + '% | ' + readableSize2(installer_info.bytesPerSec()) + '/sec'
            progressBar.progress = installer_info.getProgress()
            taskbarButton.progress.value = installer_info.getProgress()
        }
    }

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
            text: "You can speed up the installation by closing other CPU/HDD intensive tasks such as Bit-torrent client"
            wrapMode: Text.WordWrap
            font.family: defaultFont.name
            font.pixelSize: 12
            font.weight: Font.Light
            color: "white"
        }
    }

    Image {
        source: "image://resources/images/textBG.png"
        x: 0
        y: 186
        width: 266
        height: 80

        Text {
            anchors.fill: parent
            leftPadding: 17
            verticalAlignment: Text.AlignVCenter
            text: "If you receive any errors such as CRC error, click \"Verify File Integrity\" button and make sure your files are not corrupted. If files(s) are corrupted by any changce, rehash using torrent"
            wrapMode: Text.WordWrap
            font.family: defaultFont.name
            font.pixelSize: 12
            font.weight: Font.Light
            color: "white"
        }
    }

    CusCheckBox2 {
        x: 15
        y: 282
        text: 'HIBERNATE PC AFTER INSTALLATION FINISHED'
        font.pixelSize: 10
        Component.onCompleted: checked = installer_info.hibernatePCAfterInstallation
        onCheckedChanged: installer_info.hibernatePCAfterInstallation = checked
    }

    Rectangle {
        width: 498
        height: 25
        x: 278
        y: 149
        id: progressBar
        color: "transparent"
        property int progress: 0
        border.color: installer_info.themeColor

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: 497 * progressBar.progress / 100.0

            anchors.margins: parent.border.width + 1
            color: installer_info.themeColor
        }
    }

    Text {
        y: 380
        x: 17
        anchors.left: about_txt.left
        font.family: defaultFont.name
        font.pixelSize: 16
        font.weight: Font.ExtraLight
        font.capitalization: Font.AllUppercase
        color: "white"
        text: '\nInstaller By Gupta & -Raven-'
    }
}
