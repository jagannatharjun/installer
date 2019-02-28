import QtQuick 2.0

Component {
    id: contactDelegate
    Image {
        width: 266
        height: 35
        visible: caption != ''
        onVisibleChanged: {
            if (!visible) {
                height = -4
            }
        }
        source: "image://resources/images/textBg.png"

        Column {
            spacing: 4

            Text {
                x: 15
                text: title
                id: title_text
                color: "#4e4e4f"

                font.family: defaultFont.name
                font.pixelSize: 12
            }
            Text {
                x: title_text.x
                y: title_text.y + title_text.height
                text: caption
                color: "white"

                font.family: defaultFont.name
                font.pixelSize: 13
            }
        }
    }
}
