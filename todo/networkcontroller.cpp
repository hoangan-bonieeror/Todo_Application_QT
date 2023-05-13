
#include "networkcontroller.h"
#include <QNetworkAccessManager>
#include <QHttpPart>

NetworkController::NetworkController()
{
    this->networkManager = new QNetworkAccessManager(this);
    headers["User-Agent"] = "Request Manager 1.0";
}

// GET : get all todo
QByteArray NetworkController::GET(QString const hostName)
{
    QEventLoop loop;
    QNetworkRequest request = this->constructNetworkRequest(hostName, this->headers);

    QNetworkReply *reply = this->networkManager->get(request);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

    loop.exec();
    return reply->readAll();
}

// POST : create new todo
void NetworkController::POST(const QString hostName, int id, QString content, bool status) {
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
    return;
}
// PUT : update todo
void NetworkController::PUT(QString hostName, const int id, const QString content, bool status) {
    QEventLoop loop;
    QString idAsString;
    idAsString.setNum(id);
    hostName.append("?id=");
    hostName.append(idAsString);

    qDebug() << hostName;
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
    qDebug() << putData;
    QNetworkReply *reply = this->networkManager->put(request, putData);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

    loop.exec();
    return;
}
// DELETE : delete toto
void NetworkController::DELETE(QString hostName, const int id) {
    QEventLoop loop;
    QString idAsString;
    idAsString.setNum(id);
    hostName.append("?id=");
    hostName.append(idAsString);

    QNetworkRequest request = this->constructNetworkRequest(hostName, this->headers);

    QNetworkReply *reply = this->networkManager->deleteResource(request);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

    loop.exec();
    return;
}

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
