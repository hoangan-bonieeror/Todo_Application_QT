
#include "networkcontroller.h"
#include <QNetworkAccessManager>
#include <QHttpPart>
#include <QNetworkReply>

const qint16 PORT = 3000;
const QString HOST = "http://localhost";
const int REQUEST_TIMEOUT = 3000;
const int NUM_RETRY = 3;
NetworkController::NetworkController()
{
    this->networkManager = new QNetworkAccessManager(this);
    headers["User-Agent"] = "Request Manager 1.0";
    this->networkManager->connectToHost(HOST, PORT);
    // Specify request timeout
    this->networkManager->setTransferTimeout(REQUEST_TIMEOUT);
    this->pNumRetry = NUM_RETRY;
}

// ###### Define http methods ######
// GET : get all todo
QNetworkReply* NetworkController::GET(QString const hostName)
{
    QEventLoop loop;
    QNetworkRequest request = this->constructNetworkRequest(hostName, this->headers);
    QNetworkReply *reply = this->networkManager->get(request);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    return reply;
}

// POST : create new todo
QNetworkReply* NetworkController::POST(const QString hostName, int id, QString content, bool status) {
    QEventLoop loop;
    QNetworkRequest request = this->constructNetworkRequest(hostName, this->headers);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject bodyJson;
    bodyJson.insert("id", id);
    bodyJson.insert("content", content);
    bodyJson.insert("status", status);
    QJsonDocument docJson;
    docJson.setObject(bodyJson);
    QByteArray postData = docJson.toJson();
    QNetworkReply *reply = this->networkManager->post(request, postData);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    return reply;
}
// PUT : update todo
QNetworkReply* NetworkController::PUT(QString hostName, const int id, const QString content, bool status) {
    QEventLoop loop;
    QString idAsString;
    idAsString.setNum(id);
    hostName.append("?id=");
    hostName.append(idAsString);

    QNetworkRequest request = this->constructNetworkRequest(hostName, this->headers);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject bodyJson;
    if(!content.isNull()) {
        bodyJson.insert("content", content);
    }
    bodyJson.insert("status", status);
    QJsonDocument doc;
    doc.setObject(bodyJson);
    QByteArray putData = doc.toJson();
    QNetworkReply *reply = this->networkManager->put(request, putData);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

    loop.exec();
    return reply;
}
// DELETE : delete toto
QNetworkReply* NetworkController::DELETE(QString hostName, const int id) {
    QEventLoop loop;
    QString idAsString;
    idAsString.setNum(id);
    hostName.append("?id=");
    hostName.append(idAsString);

    QNetworkRequest request = this->constructNetworkRequest(hostName, this->headers);

    QNetworkReply *reply = this->networkManager->deleteResource(request);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

    loop.exec();
    return reply;
}
// ###### End define ######

// ###### Network Utils ######
QNetworkRequest NetworkController::constructNetworkRequest(QString const hostName, QMap<QString, QString> headers) {
    QNetworkRequest request;
    request.setUrl(QUrl(hostName));

    if(!headers.isEmpty()) {
        QMapIterator<QString, QString> iterator(headers);
        while(iterator.hasNext()) {
            iterator.next();
            request.setRawHeader(QByteArray::fromStdString(iterator.key().toStdString()), QByteArray::fromStdString(iterator.value().toStdString()));

        }
    }

    return request;
}
int NetworkController::getNumRetry() {
    return this->pNumRetry;
}

