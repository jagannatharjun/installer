import QtQuick 2.0

//The starting angle, in degrees (0 is at the 12 o'clock position of the arc's circle)
Item {
    property int startAngle: 0
    property int endAngle: 180
    property color strokeColor: 'white'
    property color color: strokeColor
    property int strokeWidth: 4
    property bool counterclockwise: false
    id: sastaCircle
    onStartAngleChanged: canvasArc.requestPaint()
    onEndAngleChanged: canvasArc.requestPaint()
    onStrokeColorChanged: canvasArc.requestPaint()
    onCounterclockwiseChanged: canvasArc.requestPaint()

    Canvas {
        anchors.fill: parent
        id: canvasArc

        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            var centerX = width / 2
            var centerY = height / 2
            var sa = (sastaCircle.startAngle - 90) / 180 * Math.PI
            var se = (sastaCircle.endAngle - 90) / 180 * Math.PI
            ctx.beginPath();
            ctx.strokeStyle = sastaCircle.strokeColor
            ctx.lineWidth = sastaCircle.strokeWidth
            ctx.arc(centerX, centerY, Math.min(width,height) / 2 - strokeWidth, sa, se,sastaCircle.counterclockwise);
            ctx.stroke();
        }
    }
}
