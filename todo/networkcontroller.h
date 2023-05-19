
#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H
#include <QNetworkAccessManager>
#include <QMap>
#include <QObject>
#include <QtCore>
#include <QNetworkReply>
#include <QUrl>


class NetworkController : QObject
{
    Q_OBJECT
public:
    NetworkController();
    QNetworkAccessManager *manager;

    QNetworkReply* GET(const QString hostName);
    QNetworkReply* POST(const QString hostName, int id, QString content, bool status);
    QNetworkReply* PUT(QString hostName, const int id, const QString content, bool status);
    QNetworkReply* DELETE(QString hostName, const int id);
    int getRequestTimeout();
private:
    QNetworkRequest constructNetworkRequest(const QString hostName, QMap<QString, QString> headers);
    QNetworkAccessManager *networkManager;
    QMap<QString, QString> headers;
};

#endif // NETWORKCONTROLLER_H
