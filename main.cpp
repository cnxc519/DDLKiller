#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "DataSql.h"
#include "Date.h"
#include "jsonGenerator.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    DataSql dataSql;
    Date date;
    JsonGenerator jsonGenerator;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("dataSql",&dataSql);
    engine.rootContext()->setContextProperty("date",&date);
    engine.rootContext()->setContextProperty("jsonGenerator",&jsonGenerator);

    const QUrl url(u"qrc:/DDLonline/Main.qml"_qs);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("DDLonline", "Main");
    //问题反正是解决了,咱也不知道两个代码为什么那个行这个不行,用上面这个反正可以了
    //engine.load(url);

    dataSql.initializeDatabase();

    return app.exec();
}
