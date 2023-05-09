
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
void NetworkController::POST(const QString hostName, QString content, bool status) {
    QEventLoop loop;
    QNetworkRequest request = this->constructNetworkRequest(hostName, this->headers);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject bodyJson;
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

