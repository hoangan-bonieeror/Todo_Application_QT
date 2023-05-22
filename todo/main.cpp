
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <todo.h>
#include <networkcontroller.h>
#include <QQuickView>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<TodoModel>("TodoModel", 1, 0, "TodoModel");
//    qmlRegisterType<NetworkController>("NetworkController", 1, 0, "NetworkController");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("todo", "Main");

    return app.exec();
}
