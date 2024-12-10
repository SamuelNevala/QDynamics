import QtQuick
import QtQuick.Window
import QDynamics

pragma ComponentBehavior: Bound

Window {
    id: window

    property alias asynchronous: loader.asynchronous
    property alias source: loader.source
    property alias type: dimensions.type

    color: "black"
    visible: false

    Dimensions {
        id: dimensions
        onScaleChanged: {
            loader.reload();
        }
    }

    Loader {
        id: loader

        function reload() {
            window.visible = false
            loader.asynchronous = false;
            const source = loader.source;
            loader.source = "";
            loader.source = source;
            loader.asynchronous = true;
        }

        anchors { fill: parent }
        asynchronous: true
        opacity: 0.2
        onLoaded: {
            loader.item.dimensions = dimensions
            window.visible = true
            opacity = 1.0
        }
        Behavior on opacity { DefaultAnimation { duration: 500 } }
    }
}
