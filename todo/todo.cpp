
#include "todo.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "networkcontroller.h"

// Begin TodoItem Class
TodoItem::TodoItem(QString const &name, bool const &isDone) {
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
        default:
            return QVariant();
    }
}
void TodoModel::addTodo(QString const &value) {
    beginInsertRows(QModelIndex(), rowCount() , rowCount());
    TodoItem item(value, false);
    controller->POST("http://localhost:3000", item.content(), item.status());
    listTodo.append(item);
    endInsertRows();
}

void TodoModel::removeTodo(int index){
    beginRemoveRows(QModelIndex(), index, index);
    listTodo.removeAt(index);
    endRemoveRows();
}

QHash<int, QByteArray> TodoModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ContentRoles] = "content";
    roles[IsDoneRole] = "status";
    return roles;
}

void TodoModel::getData() {
    QByteArray data = controller->GET("http://localhost:3000");
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject rootObj = document.object();
    QJsonValue values = rootObj.value("data");
    if(values.isArray()) {
            if(rowCount() > 0) {
                beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
                listTodo.clear();
                endRemoveRows();
            }
            QJsonArray listData = values.toArray();
            qDebug() << listData.count();
            for (const auto &item : listData) {
                QJsonObject obj=item.toObject();
                beginInsertRows(QModelIndex(), rowCount() , rowCount());
                TodoItem todo(obj.value("content").toString(), obj.value("status").toBool());
                listTodo.append(todo);
                endInsertRows();
            }
    }
}
