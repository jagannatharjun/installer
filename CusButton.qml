import QtQuick.Controls 2.12
import QtQuick 2.12

Button {
    width: 94
    height: 31
    id: button

    contentItem: Text {
        text: button.text
        font.family: defaultFont.name
        font.pointSize: 9
        font.capitalization: Font.AllUppercase
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "white"
    }

    background: Rectangle {
        color: button.hovered ? installer_info.themeColor : "#18191d"
    }
}
