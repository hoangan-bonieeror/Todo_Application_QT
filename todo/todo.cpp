
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
TodoModel::TodoModel(QObject *parent) : QAbstractListModel(parent){
    this->controller = new NetworkController();
    this->setProperty("isLoading", false);
};
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
    this->setProcessing(true);
    int newID;
    if(rowCount() == 0) {
            newID = 0;
    } else {
            TodoItem lastTodo = listTodo.at(rowCount() - 1);
            newID = lastTodo.id() + 1;
    }
    TodoItem item(newID, value, false);

    // Edit
    int numRetry = this->controller->getNumRetry();
    QString hostName = "http://localhost:3000";
    QNetworkReply * reply;
    reply = controller->POST(hostName, item.id(), item.content(), item.status());
    if(reply->error() != QNetworkReply::NoError) {
        int initRetryTime = 1;
        do {
            reply = controller->POST(hostName, item.id(), item.content(), item.status());
            if(reply->error() != QNetworkReply::NoError) {
                initRetryTime += 1;
                continue;
            } else {
                break;
            }
        } while(initRetryTime <= numRetry);
    }


    if(reply->error() == QNetworkReply::NoError) {
            beginInsertRows(QModelIndex(), rowCount() , rowCount());
            listTodo.append(item);
            listId.append(newID);
            endInsertRows();
    }
    this->setProcessing(false);
}

// DELETE : delete todo
void TodoModel::removeTodo(int index){
    this->setProcessing(true);
    int numRetry = this->controller->getNumRetry();
    TodoItem foundTodo = listTodo.at(index);
    const QString hostName = "http://localhost:3000";
    QNetworkReply *reply = controller->DELETE(hostName, foundTodo.id());
    if(reply->error() != QNetworkReply::NoError) {
        int initRetryTime = 1;
        do {
            reply = controller->DELETE(hostName, foundTodo.id());
            if(reply->error() != QNetworkReply::NoError) {
                initRetryTime += 1;
                continue;
            } else {
                break;
            }
        } while(initRetryTime <= numRetry);
    }

    if(reply->error() == QNetworkReply::NoError) {
            beginRemoveRows(QModelIndex(), index, index);
            listTodo.removeAt(index);
            endRemoveRows();
    }
    this->setProcessing(false);
}

// GET : get all todos
void TodoModel::getData() {
    this->setProcessing(true);
    int numRetry = this->controller->getNumRetry();
    QString hostName = "http://localhost:3000";
    QNetworkReply * reply;
    reply = controller->GET(hostName);
    if(reply->error() != QNetworkReply::NoError) {
        int initRetryTime = 1;
        do {
//            qDebug() << "Retry"<< initRetryTime;
            reply = controller->GET(hostName);
            if(reply->error() != QNetworkReply::NoError) {
//                qDebug() << reply->error();
                initRetryTime += 1;
                continue;
            } else {
                break;
            }
        }
        while(initRetryTime <= numRetry);
    }

    if(reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
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

    this->setProcessing(false);
}
 // PUT : toggle todo
void TodoModel::checkTodo(int index, QString content, bool status) {
    this->setProcessing(true);
    int numRetry = this->controller->getNumRetry();
    TodoItem foundTodo = listTodo.at(index);
    const QString hostName = "http://localhost:3000";
    // Call API to modify data at server
    QNetworkReply *reply = controller->PUT(hostName, index, content, status);
    if(reply->error() != QNetworkReply::NoError) {
        int initRetryTime = 1;
        do {
            reply = controller->PUT(hostName, index, content, status);
            if(reply->error() != QNetworkReply::NoError) {
                initRetryTime += 1;
                continue;
            } else {
                break;
            }
        } while(initRetryTime <= numRetry);
    }
    if(reply->error() == QNetworkReply::NoError) {
        // Modify data in the model at a specified index in the list todo
        setData(createIndex(index, IsDoneRole), status, IsDoneRole);
        foundTodo.setStatus(status);
    }

    this->setProcessing(false);
    return;
}
bool TodoModel::isProcessing() const {
    return this->pIsProcessing;
}
void TodoModel::setProcessing(bool value) {
    this->pIsProcessing = value;
    emit isProcessingChanged();
    return;
}
// ###### End TodoModel class ######
