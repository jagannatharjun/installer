import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

Image {
    width: 800
    height: 480
    visible: true
    id: installer
    source: "image://resources/images/1.png"
    property alias overviewRectColor: overview.color
    property alias directoryRectColor: directory.color
    property int pageNumber: 0

    focus: true
    Keys.onEnterPressed: if (pageNumber != 5)
                             nextButtonClicked(pageNumber)
    Keys.onReturnPressed: if (pageNumber != 5)
                              nextButtonClicked(pageNumber)

    onPageNumberChanged: {

        overview.color = pageNumber == 1 ? installer_info.themeColor : 'black'
        directory.color = pageNumber == 2 ? installer_info.themeColor : 'black'
        components.color = pageNumber == 3 ? installer_info.themeColor : 'black'
        requirements.color = pageNumber == 4 ? installer_info.themeColor : 'black'
        installation.color = pageNumber == 5 ? installer_info.themeColor : 'black'
        finalization.color = pageNumber == 6 ? installer_info.themeColor : 'black'

        overviewTickOverlay.visible = pageNumber > 1
        directoryTickOverlay.visible = pageNumber > 2
        componentsTickOverlay.visible = pageNumber > 3
        requirementsTickOverlay.visible = pageNumber > 4
        installationTickOverlay.visible = pageNumber > 5
        finalizationTickOverlay.visible = pageNumber > 6

        leftBG.visible = pageNumber != 0

        function tickImageFile(srcPage) {
            if (pageNumber > srcPage)
                return "image://resources/images/tickblack.png"
            if (pageNumber === srcPage)
                return "image://resources/images/tickWhite.png"
            return "image://resources/images/tick_dark.png"
        }

        overview_tick.source = tickImageFile(1)
        directory_tick.source = tickImageFile(2)
        components_tick.source = tickImageFile(3)
        requirements_tick.source = tickImageFile(4)
        installation_tick.source = tickImageFile(5)
        finalization_tick.source = tickImageFile(6)
    }



    Image {
        id: centerBG
        y: 85
        source: "image://resources/images/center.png"
    }

    Image {
        id: upperGradient
        y: 85
        source: "image://resources/images/UpperGradient.png"
    }

    Image {
        id: leftBG
        y: 85
        source: "image://resources/images/Left.png"
    }

    Rectangle {
        id: rectangle
        x: 0
        width: 800
        height: 34
        y: 0
        color: "black"
        opacity: .7

        Rectangle {
            id: logoback
            x: 0
            y: 0
            width: 124
            height: 34
            color: "black"

            Text {
                id: core_logo
                x: 9
                y: 9
                text: "Core"
                color: "white"
                font.family: defaultFont.name
                font.weight: Font.ExtraLight
                font.pixelSize: 16
                font.capitalization: Font.AllUppercase
            }

            Text {
                id: pack_logo
                x: core_logo.x + core_logo.width + 1
                y: core_logo.y - 1
                height: core_logo.height
                text: "pack"
                color: installer_info.themeColor
                font.family: defaultFont.name
                font.weight: Font.Bold
                font.pixelSize: 17
                font.capitalization: Font.AllUppercase
                fontSizeMode: Text.HorizontalFit
            }
            Text {
                id: appTitle
                x: 132
                y: 10
                //text: 'The Witcher 3: Wild Hunt Goty Edition'
                text: installer_info.applicationName
                font.family: defaultFont.name
                font.pixelSize: 14
                font.capitalization: Font.AllUppercase
                color: 'white'
                renderType: Text.NativeRendering
            }
        }
    }

    Rectangle {
        id: pageTitleContainer
        x: 0
        y: 35
        width: installer.width
        height: 45
        color: "black"
        opacity: .7

        Rectangle {
            id: overview
            x: 0
            y: 0
            width: 133
            height: 45
            color: "black"

            MouseArea {
                anchors.fill: parent
                onPressed:  {
                    nextButtonClicked(0)
                }
            }

            Image {
                id: overview_tick
                source: "image://resources/images/tick_dark.png"
                width: 13
                height: 13
                x: 25
                y: 15
            }

            ColorOverlay {
                id: overviewTickOverlay
                source: overview_tick
                color: installer_info.themeColor
                anchors.fill: overview_tick
            }

            Text {
                id: overview_text
                x: overview_tick.x + overview_tick.width + 4
                y: overview_tick.y
                text: "Overview"
                font.family: defaultFont.name
                font.pixelSize: 12
                color: "white"
            }
        }

        Rectangle {
            id: directory
            x: overview.x + overview.width + 1
            width: 133
            height: 45
            color: "black"

            MouseArea {
                anchors.fill: parent
                onPressed:  {
                    nextButtonClicked(1)
                }
            }

            Image {
                id: directory_tick
                source: "image://resources/images/tick_dark.png"
                width: 13
                height: 13
                x: 25
                y: 15
            }

            ColorOverlay {
                id: directoryTickOverlay
                source: directory_tick
                color: installer_info.themeColor
                anchors.fill: directory_tick
            }

            Text {
                id: directory_text
                x: directory_tick.x + directory_tick.width + 4
                y: directory_tick.y
                text: "Directory"
                font.family: defaultFont.name
                font.pixelSize: 12
                color: "white"
            }
        }

        Rectangle {
            id: components
            x: directory.x + directory.width + 1
            width: 133
            height: 45
            color: "black"

            MouseArea {
                anchors.fill: parent
                onPressed:  {
                    nextButtonClicked(2)
                }
            }

            Image {
                id: components_tick
                source: "image://resources/images/tick_dark.png"
                width: 13
                height: 13
                x: 25
                y: 15
            }

            ColorOverlay {
                id: componentsTickOverlay
                source: components_tick
                color: installer_info.themeColor
                anchors.fill: components_tick
            }

            Text {
                id: components_text
                x: components_tick.x + components_tick.width + 4
                y: components_tick.y
                text: "Components"
                font.family: defaultFont.name
                font.pixelSize: 12
                color: "white"
            }
        }

        Rectangle {
            id: requirements
            x: components.x + components.width + 1
            width: 133
            height: 45
            color: "black"

            MouseArea {
                anchors.fill: parent
                onPressed:  {
                    nextButtonClicked(3)
                }
            }

            Image {
                id: requirements_tick
                source: "image://resources/images/tick_dark.png"
                width: 13
                height: 13
                x: 25
                y: 15
            }

            ColorOverlay {
                id: requirementsTickOverlay
                source: requirements_tick
                color: installer_info.themeColor
                anchors.fill: requirements_tick
            }

            Text {
                id: requirements_text
                x: components_tick.x + components_tick.width + 4
                y: components_tick.y
                text: "Requirements"
                font.family: defaultFont.name
                font.pixelSize: 12
                color: "white"
            }
        }

        Rectangle {
            id: installation
            x: requirements.x + requirements.width + 1
            width: 133
            height: 45
            color: "black"

            Image {
                id: installation_tick
                source: "image://resources/images/tick_dark.png"
                width: 13
                height: 13
                x: 25
                y: 15
            }

            ColorOverlay {
                id: installationTickOverlay
                source: installation_tick
                color: installer_info.themeColor
                anchors.fill: installation_tick
            }

            Text {
                id: installation_text
                x: components_tick.x + components_tick.width + 4
                y: components_tick.y
                text: "Installation"
                font.family: defaultFont.name
                font.pixelSize: 12
                color: "white"
            }
        }

        Rectangle {
            id: finalization
            x: installation.x + installation.width + 1
            width: 133
            height: 45
            color: "black"

            Image {
                id: finalization_tick
                source: "image://resources/images/tick_dark.png"
                width: 13
                height: 13
                x: 25
                y: 15
            }

            ColorOverlay {
                id: finalizationTickOverlay
                source: finalization_tick
                color: installer_info.themeColor
                anchors.fill: finalization_tick
            }

            Text {
                id: finalization_text
                x: components_tick.x + components_tick.width + 4
                y: components_tick.y
                text: "Finalization"
                font.family: defaultFont.name
                font.pixelSize: 12
                color: "white"
            }
        }
    }

    CusButton {
        x: 11
        y: 441
        id: back_button
        text: 'Back'
        onClicked: backButtonClicked()
    }

    CusButton {
        x: 114
        y: 441
        id: bottom_close_btn
        text: 'Close'
        onClicked: closeButtonClicked()
    }

    CusButton {
        x: 214
        y: 441
        id: about_button

        onClicked: {
            aboutButtonClicked()
        }

        text: 'About'
    }

    CusButton {
        x: 314
        y: 441
        id: integrity_button
        enabled: pageNumber < 500

        onClicked: {
            verifyFilesButtonClicked()
        }

        text: 'Verify Files'
    }

    Button {
        width: 108
        height: 24
        x: 676
        y: 445
        id: visitThreadBtn

        onClicked: {
            Qt.openUrlExternally(installer_info.threadUrl())
        }

        contentItem: Text {
            text: 'Visit Thread'
            anchors.left: parent.left
            anchors.right: parent.right
            font.family: defaultFont.name
            font.pointSize: 9
            font.capitalization: Font.AllUppercase
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "white"
            leftPadding: 24
        }

        background: Rectangle {
            color: visitThreadBtn.hovered ? installer_info.themeColor : "#18191d"

            Image {
                source: "image://resources/images/thread_icon.png"

                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.margins: 6
            }
        }
    }

    Image {
        id: website_button
        source: "image://resources/images/website.png"
        x: 645
        y: 445
        width: 24
        height: 24

        MouseArea {
            anchors.fill: parent
            onClicked: {
                Qt.openUrlExternally(installer_info.websiteUrl())
            }
        }
    }

    Image {
        id: facebook_button
        source: "image://resources/images/facebook.png"
        x: 613
        y: 445
        width: 24
        height: 24

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.openUrlExternally(installer_info.facebookUrl())
        }
    }

    Image {
        id: close_button
        source: "image://resources/images/close.png"
        width: 12
        height: 12
        x: 778
        y: 10

        MouseArea {
            anchors.fill: parent
            onClicked: closeButtonClicked()
        }
    }

    Button {
        id: minimize_button
        x: 744
        y: 10
        width: 12
        height: 12

        background: Image {
            source: "image://resources/images/minimize.png"
            y: 10
            height: 2
        }
        onClicked: minimizeButtonClicked()
    }

    Image {
        id: music_button
        source: "image://resources/images/music.png"
        x: 744 - (778 - 744)
        y: 10

        MouseArea {
            anchors.fill: parent
            onClicked: {
                musicButtonClicked()
                console.log('music')
            }
        }
    }
}

/*##^## Designer {
    D{i:2;anchors_x:0;anchors_y:16}
}
 ##^##*/

