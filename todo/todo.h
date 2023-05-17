
#ifndef TODO_H
#define TODO_H

#include <QObject>
#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "networkcontroller.h"

class TodoItem {
private:
    int pId;
    QString pContent;
    bool pStatus;
public :
    explicit TodoItem(int const &id, QString const &name, bool const &isDone);
    QString content() const;
    void setContent(QString const &value);
    bool status() const;
    void setStatus(bool const &value);
    int id() const;
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
        IsDoneRole,
        IdRole
    };
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Q_INVOKABLE void addTodo(const QString &value);
    Q_INVOKABLE void removeTodo(int index);
    Q_INVOKABLE void getData();
    Q_INVOKABLE void checkTodo(int index, QString content, bool status);
signals :
protected :
    QHash<int, QByteArray> roleNames() const override;
private :
    QList<TodoItem> listTodo;
    NetworkController *controller;
    QList<int> listId;
};

#endif // TODO_H
