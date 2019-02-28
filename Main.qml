// "StaticDesign.qml"
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

Item {
    width: 800
    height: 480
    visible: true

    signal websiteButtonClicked
    signal facebookButtonClicked
    signal visitThreadButtonClicked
    signal minimizeButtonClicked
    signal closeButtonClicked
    signal musicButtonClicked
    signal backButtonClicked

    signal nextButtonClicked(int source_page)
    signal currentPageChanged(int source_page)

    property var pageSources: ["file:///E:/Cpp/Projects/Gui/installer/Overview.qml", 'file:///E:/Cpp/Projects/Gui/installer/Directory.qml', 'file:///E:/Cpp/Projects/Gui/installer/Components.qml', 'file:///E:/Cpp/Projects/Gui/installer/Requirements.qml', 'file:///E:/Cpp/Projects/Gui/installer/Installation.qml', 'file:///E:/Cpp/Projects/Gui/installer/Finalization.qml']

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
        name: 'ITC Avant Garde Gothic Pro'
    }

    StaticDesign {
        id: staticDesign
        onPageNumberChanged: {
            page_loader.source = pageSources[pageNumber - 1]
        }
    }

    Loader {
        id: page_loader
        onSourceChanged: {
            console.log('loader source changed')
            console.log(source)
            console.log(findPageNum(source))
        }
    }
    Component.onCompleted: {
        staticDesign.pageNumber = 1
    }

    onNextButtonClicked: {
        console.log('next button clicked')
        console.log('source_page = ' + source_page)
        staticDesign.pageNumber = source_page + 1
    }

    onBackButtonClicked: {
        if(staticDesign.pageNumber != 1)
            staticDesign.pageNumber = staticDesign.pageNumber - 1

    }
}
