import QtQuick 2.12

Item {
    property int pageId: 0

    Text {
        id: about_txt
        x: 15
        y: 100
        width: 700
        text: installer_info.aboutTxt()
        color: "white"
        font.family: defaultFont.name
        wrapMode: Text.WordWrap
        textFormat: Text.RichText
    }
    Text {
        y: 380
        anchors.left: about_txt.left
        font.family: defaultFont.name
        font.pixelSize: 16
        font.weight: Font.ExtraLight
        font.capitalization: Font.AllUppercase
        color: installer_info.themeColor
        text: '\nInstaller By Gupta & -Raven-'
    }
}
