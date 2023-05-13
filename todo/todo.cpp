
#include "todo.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "networkcontroller.h"

// Begin TodoItem Class
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
// End TodoItem Class

// Begin TodoModel Class
//TodoModel::TodoModel(QObject *parent) : QAbstractListModel(parent){

//};
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
void TodoModel::addTodo(QString const &value) {
    beginInsertRows(QModelIndex(), rowCount() , rowCount());
    TodoItem lastTodo = listTodo.at(rowCount() - 1);
    int newID = lastTodo.id() + 1;
    TodoItem item(newID, value, false);
    controller->POST("http://localhst:3000", item.id(), item.content(), item.status());
    listTodo.append(item);
    listId.append(newID);
    endInsertRows();
}

void TodoModel::removeTodo(int index){
    TodoItem foundTodo = listTodo.at(index);
    controller->DELETE("http://localhost:3000", foundTodo.id());
    beginRemoveRows(QModelIndex(), index, index);
    listTodo.removeAt(index);
    endRemoveRows();
}

QHash<int, QByteArray> TodoModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ContentRoles] = "content";
    roles[IsDoneRole] = "status";
    roles[IdRole] = "id";
    return roles;
}

void TodoModel::getData() {
    QByteArray data = controller->GET("http://localhost:3000");
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject rootObj = document.object();
    QJsonValue values = rootObj.value("data");
    if(values.isArray()) {
            beginRemoveRows(QModelIndex(), 0, rowCount()-1);
            listTodo.clear();
            listId.clear();
            endRemoveRows();
            QJsonArray listData = values.toArray();
            for (const auto &item : listData) {
                QJsonObject obj=item.toObject();
                beginInsertRows(QModelIndex(), rowCount() , rowCount());
                TodoItem todo(obj.value("id").toInt(), obj.value("content").toString(), obj.value("status").toBool());
                listTodo.append(todo);
                listId.append(obj.value("id").toInt());
                endInsertRows();
            }
    }
}

void TodoModel::checkTodo(int index, QString content, bool status) {
    TodoItem foundTodo = listTodo.at(index);
    controller->PUT("http://localhost:3000", foundTodo.id(), content, status);
    foundTodo.setStatus(status);
    return;
}
