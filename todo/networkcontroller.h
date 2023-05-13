
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

    QByteArray GET(const QString hostName);
    void POST(const QString hostName, int id, QString content, bool status);
    void PUT(QString hostName, const int id, const QString content, bool status);
    void DELETE(QString hostName, const int id);
private:
    QNetworkRequest constructNetworkRequest(const QString hostName, QMap<QString, QString> headers);
    QNetworkAccessManager *networkManager;
    QMap<QString, QString> headers;
};

#endif // NETWORKCONTROLLER_H
