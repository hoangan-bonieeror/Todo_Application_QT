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
    title: qsTr("Todo App")

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
        Row {
            width : parent.width
            layoutDirection: Qt.RightToLeft
            anchors.verticalCenter: parent.verticalCenter
            rightPadding: 20
            Button {
                id : refreshBtn
                onClicked : () => {
                    todoModel.getData()
                }
                text : "Refresh"
                background: Rectangle {
                    radius: 20
                }
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
                        property int indexItem: index
                        width : todoList.width - paddingDefault*2
                        spacing: gap
                        CheckBox {
                            width : parent.width * 1/10 - gap
                            checked: status
                            anchors.verticalCenter: parent.verticalCenter
                            icon.color : "black"
                            onClicked : () => {
                                todoModel.checkTodo(parent.indexItem, content, !status)
                            }
                        }
                        Rectangle {
                            width: parent.width * 7/10 - gap
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
                            width : parent.width * 2/10 - gap
                            text : "\u03A7"
                            height : parent.height
                            anchors.verticalCenter: parent.verticalCenter
                            onClicked : () => {
                                        notification.visible = true
                                        notification.indexItem = parent.indexItem
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
                focus: true
                Keys.onPressed: (event) => {
                    if(event.key + 1 === Qt.Key_Enter) {
                        if(textInput.text === "") {
                            prompt.visible = true
                            errorTime.start()
                        } else {
                            todoModel.addTodo(textInput.text)
                            textInput.clear()
                        }
                    }
                }
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
                    onPressed: (event) => {
                                   if(prompt.visible) prompt.visible = false
                                   errorTime.stop()
                               }
                }
                Rectangle {
                    id : prompt
                    width : parent.width - parent.padding*2
                    height : 35
                    Text {
                        text: "Todo required not to be empty !"
                        font.pixelSize: 16
                        color : "red"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    color: "transparent"
                    visible: false
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
    Rectangle {
        id : notification
        width : 300
        height : 100
        anchors.centerIn: parent
        radius : 10
//        border.color: "#F5F5DC"
//        border.width: 1
        color: "#F5F5DC"
        property int indexItem: 0
        Column {
            width : parent.width
            height : parent.height
            Rectangle {
                id : notificationContainer
                width : parent.width
                height: parent.height/2
                color : "transparent"
                Text {
                    id : notificationText
                    color : "black"
                    text : "Are you sure to delete ?"
                    anchors.verticalCenter: parent.verticalCenter
                    x : 20
                }
            }
            Rectangle {
                id : notificationFooter
                height : parent.height/2
                width : parent.width
                color : "transparent"
                Row {
                    width : parent.width
                    height : parent.height
                    layoutDirection: Qt.RightToLeft
                    spacing: 10
                    rightPadding : 10
                    Button {
                        width : 70
                        height : 40
                        text : "No"
                        font.pixelSize: 14
                        onClicked : () => {
                            notification.visible = false
                        }
                    }
                    Button {
                        width : 70
                        height : 40
                        text : "Yes"
                        font.pixelSize: 14
                        onClicked : () => {
                            todoModel.removeTodo(notification.indexItem)
                            textInput.clear()
                            notification.visible = false
                        }
                    }
                }
            }
        }
        visible: false


    }

}
