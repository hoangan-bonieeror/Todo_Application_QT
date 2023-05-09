import QtQuick
import QtQuick.Window
import QtQuick.Controls.Material 2.0
import TodoModel 1.0
//import NetworkController 1.0
import QtQml 2.0

Window {
    id : window
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    TodoModel {
        id : todoModel
    }
    Component.onCompleted: () => {
        todoModel.getData()
    }
    Rectangle {
        id : header
        width: parent.width
        height : parent.height * 1/10
        anchors.top : parent.top
        color : "#5DADE2"
        Text {
            text : qsTr("Todo Application")
            font.pixelSize: 32
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            leftPadding: 10
        }
        Button {
            id : resetBtn
            width : 100
            height : parent.height
            y : 0
            x : parent.width - resetBtn.width - 10
            background: Rectangle {
                radius: 20
            }
            text : "Refresh"
            font.pixelSize: 12
            onClicked: () => { todoModel.getData() }
            onPressed: () => {
                resetBtn.opacity = 0.5
            }
            onReleased: () => {
                resetBtn.opacity = 1
            }
        }
    }

    Item {
        width: parent.width
        height: parent.height * 9/10
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        Rectangle {
            width: parent.width / 2
            height : parent.height
            anchors.left : parent.left
            color : "#F4F6F6"
            Column {
                id : container
                width : parent.width
                height : parent.height
                padding : 10
                ListView {
                    id : todoList
                    model: todoModel
                    width : parent.width
                    height: parent.height
                    spacing: 10
                    add: Transition {
                        NumberAnimation {
                            properties: "opacity";
                            from: 0;
                            to: 1;
                            duration: 250;
                            easing.type: Easing.InOutQuad
                        }
                    }

                    delegate: Row {
                        property int gap: 5
                        property int paddingDefault: 10
                        width : todoList.width - paddingDefault*2
                        spacing: gap
                        CheckBox {
                            width : parent.width * 1/10 - gap
                            checked: status
                            anchors.verticalCenter: parent.verticalCenter
                            icon.color : "black"
                        }
                        Rectangle {
                            width: parent.width * 6/10 - gap
                            height: parent.height
                            Text {
                                text : content
                                verticalAlignment: TextInput.AlignVCenter
                                anchors.fill : parent
                                leftPadding: 10
                            }
                            border {
                                color : "gray"
                                width : 1
                            }
                            radius : 5
                        }

                        Button {
                            width : parent.width * 3/10 - gap
                            text : "Remove"
                            height : parent.height
                            anchors.verticalCenter: parent.verticalCenter
                            onClicked : () => {
                                            todoModel.removeTodo(index)
                                        }
                        }
                    }
                }
            }
        }
        Rectangle {
            width: parent.width / 2
            height : parent.height
            anchors.right : parent.right
            Column {
                property int gap: 10
                anchors.horizontalCenter: parent.horizontalCenter
                width : parent.width
                height: parent.height
                padding : 10
                spacing: gap
                TextField {
                    id : textInput
                    verticalAlignment: TextInput.AlignVCenter
                    width : parent.width - parent.padding*2
                    placeholderText: "Enter Todo..."
                    Timer {
                        id : errorTime
                        interval: 3000
                        running: false
                        repeat: false
                        onTriggered: prompt.visible = false
                    }

                    Rectangle {
                        id : prompt
                        Text {
                            text: "Todo required not to be empty !"
                            font.pixelSize: 16
                        }
                        color: "black"
                        y : parent.y - 20
                        visible: false
                    }
                    onPressed: () => {
                                   if(prompt.visible) prompt.visible = false
                                   errorTime.stop()
                               }
                }
                Button {
                    id : myBtn
                    width: parent.width - parent.padding*2
                    text: "Append"
                    onClicked: () => {
                                   if(textInput.text === "") {
                                       prompt.visible = true
                                       errorTime.start()
                                   } else {
                                       todoModel.addTodo(textInput.text)
                                       textInput.clear()
                                   }
                               }
                }
            }
        }
    }
}
