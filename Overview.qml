import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQml 2.12

Item {
    id: overview_page_comp
    x: 0
    y: 85
    width: 800
    height: 346


    FontLoader {
        id: defaultFont
        //name: "Segoe Ui"
        name: 'ITC Avant Garde Gothic Pro'
    }


    Image {
        width: 505
        height: 244
        x: 273
        y: 96 - parent.y
        source: "image://resources/images/textBG.png"

        ScrollView {
            anchors.fill: parent
            padding: 8

            TextArea {
                id: descriptionText
                text: installer_info.applicationDescription
                color: "white"
                font.family: defaultFont.name
                font.weight: Font.Medium
                wrapMode: Text.Wrap
                readOnly: true
            }
        }
    }

    Item {
        id: side_panel
        width: 267
        height: 346
       // source: "image://resources/images/overviewLeftPanel.png"

        Image {
            source: "image://resources/images/info.png"
            width: 13
            height: 13
            x: 17
            y: 98 - 85
        }

        Text {
            x: 34
            y: 10
            padding: 0
            color: "white"
            text: "REPACK INFO"
            font.family: defaultFont.name
            font.capitalization: Font.AllUppercase
            font.pointSize: 12
        }

        Item {
            x: 0
            y: 38
            width: 265
            height: 300

            ListView {
                property list<QtObject> paramModel: [
                    QtObject {
                        property string title: "Release Name"
                        property string caption: installer_info.applicationName
                    },
                    QtObject {
                        property string title: "Release Size"
                        property string caption: installer_info.info(
                                                     "ReleaseSize")
                    },
                    QtObject {
                        property string title: "Game Version"
                        property string caption: installer_info.info(
                                                     "GameVersion")
                    },
                    QtObject {
                        property string title: "Release Date"
                        property string caption: installer_info.info(
                                                     "ReleaseDate")
                    },
                    QtObject {
                        property string title: "Repack Type"
                        property string caption: installer_info.info(
                                                     "RepackType")
                    },
                    QtObject {
                        property string title: "Components"
                        property string caption: installer_info.info(
                                                     "Components")
                    },
                    QtObject {
                        property string title: "Based On"
                        property string caption: installer_info.info("BasedOn")
                    }
                ]
                anchors.fill: parent
                model: paramModel
                spacing: 4
                delegate: CusPropDelegate{}
            }
        }
    }

    Rectangle {
//        width: 171
//        height: 59
//        x: 607
//        y: 346 - parent.y

        width: 171
        height: 59
        x: 599
        y: 359 - parent.y
        color: marea.containsMouse ? installer_info.themeColor : "#18191d"

        MouseArea {
            id: marea
            anchors.fill: parent
            onClicked: {
                nextButtonClicked(1)
            }
            hoverEnabled: true
        }

        Image {
            source: "image://resources/images/next.png"
            x:20
            y:20

            Text {
                x: parent.width + 6
                y: -12
                text: "Proceed"
                font.family: defaultFont.name
                font.capitalization: Font.AllUppercase
                font.pixelSize: 22
                color: "white"

                Text {
                    y: parent.height - 4
                    text: "With Installation"
                    font.family: defaultFont.name
                    font.capitalization: Font.AllUppercase
                    font.pixelSize: 11
                    font.weight: Font.Light
                    color: "white"
                }
            }
        }
    }
}
