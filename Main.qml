// "StaticDesign.qml"
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import Ascent.info 1.0

Window {
    id: mainwindow
    objectName: "mainwindow"
    width: 800
    height: 480
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint

    MouseArea {
            anchors.fill: parent
            property point lastMousePos: Qt.point(0, 0)
            onPressed: { lastMousePos = Qt.point(mouseX, mouseY); }
            onMouseXChanged: mainwindow.x += (mouseX - lastMousePos.x)
            onMouseYChanged: mainwindow.y += (mouseY - lastMousePos.y)
        }

    // used for back button at about page
    property int lastPage: 0

    signal websiteButtonClicked
    signal facebookButtonClicked
    signal visitThreadButtonClicked
    signal minimizeButtonClicked
    signal closeButtonClicked
    signal musicButtonClicked
    signal backButtonClicked
    signal aboutButtonClicked

    signal nextButtonClicked(int source_page)
    signal currentPageChanged(int source_page)

    onAboutButtonClicked: {
        lastPage = staticDesign.pageNumber
        staticDesign.pageNumber = 0
    }

    onCloseButtonClicked: {
        installer_info.terminateInstallation(true)
        close()
    }

    onMinimizeButtonClicked: {
        lower()
    }

    Installerinfo {
        id: installer_info
        onInstallationFailed: {
            console.log(msg)
            mainwindow.close()
        }
        onInstallationCompleted: {
            staticDesign.pageNumber++
        }
    }


    /*
    property var pageSources: [
        "file:///E:/Cpp/Projects/Gui/installer/Overview.qml",
        'file:///E:/Cpp/Projects/Gui/installer/Directory.qml',
        'file:///E:/Cpp/Projects/Gui/installer/Components.qml',
        'file:///E:/Cpp/Projects/Gui/installer/Requirements.qml',
        'file:///E:/Cpp/Projects/Gui/installer/Installation.qml',
        'file:///E:/Cpp/Projects/Gui/installer/Finalization.qml'
    ]
    */
    property var pageSources: ["qrc:/About.qml", "qrc:/Overview.qml", 'qrc:/Directory.qml', 'qrc:/Components.qml', 'qrc:/Requirements.qml', 'qrc:/Installation.qml', 'qrc:/Finalization.qml']

    function readableSize(s, t) {
        var m = ['MB', 'GB', 'TB']
        t = typeof t !== 'undefined' ? t : 2
        for (var i = 0; i < m.length; i++)
            if (s < 1024)
                return s.toFixed(t) + m[i]
            else
                s /= 1024
        return s.toFixed(t) + m[m.length - 1]
    }

    function findPageNum(qmlFile) {
        for (var i = 0; i < pageSources.length; i++)
            if (pageSources[i] === qmlFile.toString())
                return i
        return -1
    }

    FontLoader {
        id: defaultFont
        //name: "Segoe Ui"
        //name: 'ITC Avant Garde Gothic Pro'

        source: 'file:///' + installer_info.expandConstant("{tmp}//font.ttf")
    }

    StaticDesign {
        id: staticDesign
        onPageNumberChanged: {
            page_loader.source = pageSources[pageNumber]
        }
    }

    Loader {
        id: page_loader
        onLoaded: {
            if (staticDesign.pageNumber == 5)
                installer_info.startInstallation()
        }
    }

    Component.onCompleted: {
        staticDesign.pageNumber = 1
    }

    onNextButtonClicked: {
        console.log('next button clicked')
        console.log('source_page = ' + source_page)
        if (source_page === 6)
            mainwindow.close()
        staticDesign.pageNumber = source_page + 1
    }

    onBackButtonClicked: {
        if (staticDesign.pageNumber == 0) {
            staticDesign.pageNumber = lastPage
        } else if (staticDesign.pageNumber != 1 && staticDesign.pageNumber != 5)
            staticDesign.pageNumber = staticDesign.pageNumber - 1
    }
}
