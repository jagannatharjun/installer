import QtQuick.Controls 2.5
import QtQuick 2.12

CheckBox {

    id: control
    font.family: defaultFont.name
    font.pixelSize: 13
    font.weight: Font.Light
    font.capitalization: Font.AllUppercase

    indicator: Rectangle {
        width: 35
        height: 15
        color: "transparent"
        border.color: control.checked ? installer_info.themeColor : "gray"

        Rectangle {
            anchors.fill: parent
            color: control.checked ? installer_info.themeColor : "gray"
            anchors.margins: 2
        }

        Rectangle {
            x: control.checked ? 25 : 0
            width: 10
            height: parent.height
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        color: "white"
        anchors.fill: parent
        leftPadding: control.indicator.width + control.spacing
    }
}
