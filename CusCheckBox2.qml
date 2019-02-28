import QtQuick.Controls 2.5
import QtQuick 2.12

CheckBox {
    id: control
    font.family: defaultFont.name
    font.pixelSize: 13
    font.weight: Font.Light

    indicator: Rectangle {
        width: 16
        height: 16
        color: "transparent"
        border.color: control.checked ? installer_info.themeColor : "white"

        Rectangle {
            anchors.fill: parent
            color: control.checked ? installer_info.themeColor : "transparent"
            anchors.margins: 2
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        color: "white"
        anchors.left: control.indicator.right
        anchors.top: control.indicator.top
        anchors.bottom: control.indicator.bottom
        leftPadding: 4
        verticalAlignment: Text.AlignVCenter
    }
}
