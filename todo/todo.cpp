
#include "todo.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
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
TodoModel::TodoModel(QObject *parent) : QAbstractListModel(parent){
    this->controller = new NetworkController();
    this->setProcessing(false);
};
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
// Implement parent class methods
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
    QString hostName = "http://localhost:3000";
    int initNum = 0;
    int numRetry = this->controller->getNumRetry();
    QNetworkReply *reply = this->controller->POST(hostName, item.id(), item.content(), item.status());
    do {
        if(reply->error() == QNetworkReply::NoError) {
            break;
        }

        reply = this->controller->POST(hostName, item.id(), item.content(), item.status());
        initNum += 1;
    } while(initNum < numRetry);

    if(initNum == numRetry) {
        return;
    }

    beginInsertRows(QModelIndex(), rowCount() , rowCount());
    listTodo.append(item);
    listId.append(newID);
    endInsertRows();
    this->setProcessing(false);
}

void TodoModel::removeTodo(int index){
    this->setProcessing(true);
    int initNumRetry = 0;
    int numRetry = controller->getNumRetry();
    TodoItem foundTodo = listTodo.at(index);
    const QString hostName = "http://localhost:3000";
    QNetworkReply *reply = controller->DELETE(hostName, foundTodo.id());

    do {
        if(reply->error() == QNetworkReply::NoError) {
            break;
        }

        reply = controller->DELETE(hostName, foundTodo.id());
        initNumRetry += 1;
    } while(initNumRetry < numRetry);

    if(initNumRetry == numRetry) {
        this->setProcessing(false);
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    listTodo.removeAt(index);
    endRemoveRows();
    this->setProcessing(false);
}

QHash<int, QByteArray> TodoModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ContentRoles] = "content";
    roles[IsDoneRole] = "status";
    roles[IdRole] = "id";
    return roles;
}

void TodoModel::getData() {
    this->setProcessing(true);
    int initNum = 0;
    int numTry = controller->getNumRetry();
    QString hostName = "http://localhost:3000";
    QNetworkReply * reply = controller->GET(hostName);
    do {
        if(reply->error() == QNetworkReply::NoError) {
            break;
        }

        reply =  controller->GET(hostName);
        initNum += 1;
    } while (initNum < numTry);

    if(initNum == numTry) {
        this->setProcessing(false);
        return;
    }

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

    this->setProcessing(false);
}

void TodoModel::checkTodo(int index, QString content, bool status) {
    this->setProcessing(true);
    int initNumRetry = 0;
    int numRetry = this->controller->getNumRetry();
    TodoItem foundTodo = listTodo.at(index);
    const QString hostName = "http://localhost:3000";
    QNetworkReply *reply = controller->PUT(hostName, index, foundTodo.content(), status);
    do {
        if(reply->error() == QNetworkReply::NoError) {
            break;
        }

        reply = controller->PUT(hostName, index, foundTodo.content(), status);
        initNumRetry += 1;
    } while(initNumRetry < numRetry);

    if(initNumRetry == numRetry) {
        this->setProcessing(false);
        return;
    }
    bool result = setData(createIndex(index, IsDoneRole), status, IsDoneRole);
    qDebug() << result;
    foundTodo.setStatus(status);
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
