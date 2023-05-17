
#include "todo.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "networkcontroller.h"

// ###### Begin TodoItem Class ######
TodoItem::TodoItem(int const &id, QString const &name, bool const &isDone) {
    pId = id;
    pContent = name;
    pStatus = isDone;
}
QString TodoItem::content() const {
    return pContent;
}
void TodoItem::setContent(QString const &value) {
    if(pContent != value) {
        pContent = value;
    }
    return;
}
bool TodoItem::status() const{
    return pStatus;
}
void TodoItem::setStatus(bool const &value) {
    pStatus = value;
}
int TodoItem::id() const {
    return pId;
}
// ###### End TodoItem Class ######

// ###### Begin TodoModel Class ######
//TodoModel::TodoModel(QObject *parent) : QAbstractListModel(parent){};
// ## Override method
int TodoModel::rowCount(const QModelIndex &parent = QModelIndex()) const{
    return listTodo.count();
}
QVariant TodoModel::data(const QModelIndex &index, int role=Qt::DisplayRole) const {
    if(!index.isValid()) {
        return QVariant();
    }
    TodoItem const &item = listTodo[index.row()];
    switch (role) {
        case ContentRoles:
            return item.content();
            break;
        case IsDoneRole:
            return item.status();
        case IdRole:
            return item.id();
        default:
            return QVariant();
    }
}
QHash<int, QByteArray> TodoModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ContentRoles] = "content";
    roles[IsDoneRole] = "status";
    roles[IdRole] = "id";
    return roles;
}

// ## Implement parent class methods
bool TodoModel::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {
    TodoItem &item = listTodo[index.row()];
    switch (role) {
    case ContentRoles:
            item.setContent(value.toString());
            return true;
    case IsDoneRole:
            item.setStatus(value.toBool());
            return true;
    default:
            return true;
    }
}

// ## Invokable methods (via API)
// POST : add new todo
void TodoModel::addTodo(QString const &value) {
    // Insert data to the model
    beginInsertRows(QModelIndex(), rowCount() , rowCount());
    int newID;
    if(rowCount() == 0) {
        newID = 0;
    } else {
        TodoItem lastTodo = listTodo.at(rowCount() - 1);
        newID = lastTodo.id() + 1;
    }
    TodoItem item(newID, value, false);
    // Call API to add new record
    controller->POST("http://localhost:3000", item.id(), item.content(), item.status());
    // Append to todo list
    listTodo.append(item);
    listId.append(newID);
    endInsertRows();
}

// DELETE : delete todo
void TodoModel::removeTodo(int index){
    TodoItem foundTodo = listTodo.at(index);
    // Call API to remove record by id todo
    controller->DELETE("http://localhost:3000", foundTodo.id());
    // Remove data associated with specified index
    beginRemoveRows(QModelIndex(), index, index);
    // Remove todo by index from todo list
    listTodo.removeAt(index);
    endRemoveRows();
}

// GET : get all todos
void TodoModel::getData() {
    QByteArray data = controller->GET("http://localhost:3000");
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject rootObj = document.object();
    QJsonValue values = rootObj.value("data");
    if(values.isArray()) {
            // Clear all rows in case of refreshing data
            beginRemoveRows(QModelIndex(), 0, rowCount()-1);
            listTodo.clear();
            listId.clear();
            endRemoveRows();
            // Start insert every single data from API to the model
            QJsonArray listData = values.toArray();
            for (const auto &item : listData) {
                QJsonObject obj=item.toObject();
                beginInsertRows(QModelIndex(), rowCount() , rowCount());
                TodoItem todo(obj.value("id").toInt(), obj.value("content").toString(), obj.value("status").toBool());
                // Append data to listTodo
                listTodo.append(todo);
                listId.append(obj.value("id").toInt());
                endInsertRows();
            }
    }
}
 // PUT : toggle todo
void TodoModel::checkTodo(int index, QString content, bool status) {
    // Call API to modify data at server
    controller->PUT("http://localhost:3000", index, content, status);
    // Modify data in the model at a specified index in the list todo
    setData(createIndex(index, IsDoneRole), status, IsDoneRole);
    return;
}

// ###### End TodoModel class ######
