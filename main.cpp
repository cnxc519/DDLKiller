#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Date.h"
#include "jsonGenerator.h"
#include "database.h"
#include "json.h"
#include "websocketclient.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //DataSql dataSql;
    Date date;
    JsonGenerator jsonGenerator;

    // 初始化JSON处理器
    JsonProcessor *jsonProcessor = new JsonProcessor();

    // 使用单例模式创建Websocket实例
    WebSocketClient *webSocketClient = WebSocketClient::getInstance(&app);

    // 使用单例模式获取数据库管理器实例
    DatabaseManager *dbManager = DatabaseManager::getInstance();
    // 创建数据库表
    if (!dbManager->createTable()) {
        qWarning() << "Failed to create database table";
    }

    QQmlApplicationEngine engine;

    //engine.rootContext()->setContextProperty("dataSql",&dataSql);
    engine.rootContext()->setContextProperty("date",&date);
    engine.rootContext()->setContextProperty("jsonGenerator",&jsonGenerator);
    engine.rootContext()->setContextProperty("databaseManager", dbManager);
    engine.rootContext()->setContextProperty("jsonProcessor", jsonProcessor);
    engine.rootContext()->setContextProperty("websocket", webSocketClient);


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

    webSocketClient->fullUpdate=true;
    webSocketClient->connectToServer("ws://8.148.4.26:8090");

    int result = app.exec();

    // 程序退出前销毁单例
    DatabaseManager::destroyInstance();

    return result;
}
