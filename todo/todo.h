
#ifndef TODO_H
#define TODO_H

#include <QObject>
#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "networkcontroller.h"

class TodoItem {
private:
    QString pContent;
    bool pStatus;
public :
    explicit TodoItem(QString const &name, bool const &isDone);
    QString content() const;
    void setContent(QString const &value);
    bool status() const;
    void setStatus(bool const &value);
};

class TodoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TodoModel(QObject *parent = nullptr) : QAbstractListModel(parent) {
        this->controller = new NetworkController();
    };
    enum TodoItemRoles {
        ContentRoles = Qt::UserRole+1,
        IsDoneRole
    };
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Q_INVOKABLE void addTodo(const QString &value);
    Q_INVOKABLE void removeTodo(int index);
    Q_INVOKABLE void getData();
signals :
public slots :
protected :
    QHash<int, QByteArray> roleNames() const override;
private :
    QList<TodoItem> listTodo;
    NetworkController *controller;
};

#endif // TODO_H
