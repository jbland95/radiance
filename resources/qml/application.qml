import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import radiance 1.0
import "."

ApplicationWindow {
    id: window;
    visible: true
    color: "#333"
    width: 1200
    height: 800
    title: "Radiance"
    property bool hasMidi: false

    PreviewAdapter {
        id: previewAdapter;
        model: model;
        previewWindow: window;
    }

    Context {
        id: context;
    }

    Registry {
        id: registry;
    }

    Model {
        id: model;
        onGraphChanged: {
            var changeset = "+" + verticesAdded.length + " -" + verticesRemoved.length + " vertices, ";
            changeset += "+" + edgesAdded.length + " -" + edgesRemoved.length + " edges";
            console.log("Graph changed!", changeset);
        }

        onMessage: {
            messages.text += "<font color=\"green\"><pre>" + str + "</pre></font>";
        }
        onWarning: {
            messages.text += "<font color=\"gold\"><pre>" + str + "</pre></font>";
        }
        onFatal: {
            messages.text += "<font color=\"red\"><pre>" + str + "</pre></font>";
        }
    }

    Action {
        id: saveAction
        shortcut: "Ctrl+S"
        onTriggered: {
            if (model.vertices.length >= 0) {
                model.saveFile("radiance_state.json");
            }
        }
    }

    Action {
        id: loadAction
        shortcut: "Ctrl+R"
        onTriggered: {
            console.log("Loading state from file...");
            model.loadFile(context, registry, "radiance_state.json");
            model.flush();
        }
    }

    Timer {
        repeat: true
        running: true
        interval: 10 * 1000
        onTriggered: saveAction.trigger()
    }

    onClosing: {
        saveAction.trigger();
    }

    /*
    // Make some nodes here to show it can be done; alternatively call model.createVideoNode(...)
    EffectNode {
        id: cross
        name: "crossfader"
        inputCount: 2
    }
    */

    Component.onCompleted: {
        Globals.previewAdapter = previewAdapter;

        loadAction.trigger();
        if (model.vertices.length == 0) {
            // If the state was empty, then open up a few nodes as a demo
            var n1 = registry.deserialize(context, "{\"type\": \"ImageNode\", \"imagePath\": \"nyancat.gif\"}");
            model.addVideoNode(n1);
            var n2 = registry.deserialize(context, "{\"type\": \"EffectNode\", \"name\": \"test\"}");
            model.addVideoNode(n2);
            var n3 = registry.deserialize(context, "{\"type\": \"EffectNode\", \"name\": \"interstellar\"}");
            model.addVideoNode(n3);
            var cross = registry.deserialize(context, "{\"type\": \"EffectNode\", \"name\": \"crossfader\"}");
            model.addVideoNode(cross);
            model.addEdge(n1, n2, 0);
            model.addEdge(n2, n3, 0);
            model.addEdge(n3, cross, 0);
            model.flush();
        }
    }

    ColumnLayout {
        anchors.fill: parent;

        RowLayout {
            Layout.fillWidth: true;

            Loader {
                source: window.hasMidi ? "MidiMappingSelector.qml" : ""
                onLoaded: {
                    item.target = graph.view;
                }
            }

            Button {
                text: "Save"
                action: saveAction
            }
            Button {
                text: "Load"
                action: loadAction
            }
            Button {
                text: "Clear"
                onClicked: {
                    model.clear();
                    model.flush();
                }
            }
        }

        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;

            Graph {
                id: graph
                model: model
                anchors.fill: parent
            }

            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    BeatIndicator {
                        width: 25
                        opacity: .9
                        context: context
                    }
                    Waveform {
                        width: 500
                        opacity: .9
                        context: context
                    }
                    Spectrum {
                        width: 500
                        opacity: .9
                        context: context
                    }
                }
                RowLayout {
                    LibraryWidget {
                        id: libraryWidget
                        registry: registry
                        model: model
                        width: 500
                        Layout.fillHeight: true
                    }
                }
            }

            Label {
                id: messages
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10
                color: "white"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        messages.text = "";
                    }
                }
            }
        }
    }

    Action {
        id: quitAction
        text: "&Quit"
        shortcut: "Ctrl+Q"
        onTriggered: {
            saveAction.trigger()
            Qt.quit()
        }
    }

    Action {
        id: newNodeAction
        text: "&New Node"
        shortcut: ":"
        onTriggered: nodeSelector.forceActiveFocus()
    }
}
