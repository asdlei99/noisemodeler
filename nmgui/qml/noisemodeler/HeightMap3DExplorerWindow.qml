import QtQuick 2.2
import NoiseModeler 1.0
import QtQuick.Layouts 1.1

SubWindow {
    property bool maximized: false
    windowTitle: "heightmap 3D preview"
    contents.width: maximized ? mainArea.width: 350
    contents.height: maximized ? mainArea.height - topBar.height - titleHeight: 350 //TODO fix this ugly dependency
    contents.children: [
        HeightMap3DExplorer {
            id: renderer
            heightMapFunction: HeightMapFunction {
                inputLink: document.typeManager.userTypes[0].graph.findModule("inputs").inputs[0];
                outputLink: document.typeManager.userTypes[0].graph.findModule("outputs").outputs[0];
            }
            anchors.fill: parent
            Keys.forwardTo: keyMap
        },
        Item {
            id: cameraKeyboardControls
            anchors.fill: renderer
            KeyMap {
                id: keyMap
            }
            Timer {
                running: true; repeat: true
                interval: 10
                onTriggered: {
                    //yawing/turning
                    var yawDir = -keyMap.getAxis(Qt.Key_Right, Qt.Key_Left);
                    var yawSpeed = 80;
                    if(yawDir!==0)renderer.yawCamera(yawDir * yawSpeed * interval/1000);

                    //pitching/tilting
                    var pitchDir = keyMap.getAxis(Qt.Key_Up, Qt.Key_Down);
                    var pitchSpeed = 80;
                    if(pitchDir!==0)renderer.pitchCamera(-pitchDir * pitchSpeed * interval/1000);

                    //forwards/backwards
                    var forwardDir = keyMap.getAxis(Qt.Key_W, Qt.Key_S);
                    var forwardSpeed = 64;
                    if(forwardDir!==0)renderer.moveCameraForward(forwardDir * forwardSpeed * interval/1000);

                    var strafeDir = keyMap.getAxis(Qt.Key_D, Qt.Key_A);
                    var strafeSpeed = 64;
                    if(strafeDir!==0)renderer.moveCameraRight(strafeDir * strafeSpeed * interval/1000);
                }
            }
        },
//        MapLikeDomainController {
//            size: renderer.size
//            center: renderer.center
//            anchors.fill: renderer
//            onPressedChanged: renderer.forceActiveFocus(); //renderer.focus = true;
//            onDoubleClicked: maximized = !maximized
//        },
        MouseArea {
            anchors.fill: renderer
            onDoubleClicked: maximized = !maximized
            onPressedChanged: renderer.forceActiveFocus(); //renderer.focus = true;
        },
        GridLayout {
            anchors.top: renderer.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            columns: 4
            Text {
                text: "x:"
            }
            LineInput {
                id: xPosInput
                text:"0"
                width: 40
                validator: DoubleValidator{}
                onNewAcceptableValue: {
                    renderer.center.x = parseFloat(text);
                }
                Connections{
                    target: renderer
                    onCenterChanged: if(!xPosInput.textInput.focus)xPosInput.text = renderer.center.x.toFixed(3);
                }
            }
            Text {
                text: "width scale:"
            }
            LineInput {
                id: widthInput
                text:"1"
                width: 40
                validator: DoubleValidator{}
                onNewAcceptableValue: {
                    renderer.widthScale = parseFloat(text);
                }
                Connections{
                    target: renderer
                    onWidthScaleChanged: if(!widthInput.textInput.focus)widthInput.text = renderer.widthScale.toFixed(3);
                }
            }
            Text {
                text: "y:"
            }
            LineInput {
                id: yPosInput
                text:"0"
                width: 40
                validator: DoubleValidator{}
                onNewAcceptableValue: {
                    renderer.center.y = parseFloat(text);
                }
                Connections{
                    target: renderer
                    onCenterChanged: if(!yPosInput.textInput.focus)yPosInput.text = renderer.center.y.toFixed(3);
                }
           }
            Text {
                text: "height scale:"
            }
            LineInput {
                id: heightInput
                text:"1"
                width: 40
                validator: DoubleValidator{}
                onNewAcceptableValue: {
                    renderer.heightScale = parseFloat(text);
                }
                Connections{
                    target: renderer
                    onHeightScaleChanged: if(!heightInput.textInput.focus)heightInput.text = renderer.heightScale.toFixed(3);
                }
            }

        }
    ]
}
