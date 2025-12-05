#include "websocketclient.h"
#include <QDebug>

// 静态成员初始化
WebSocketClient* WebSocketClient::instance = nullptr;

WebSocketClient* WebSocketClient::getInstance(QObject* parent)
{
    if (!instance) {
        instance = new WebSocketClient(parent);
    }
    return instance;
}

void WebSocketClient::destroyInstance()
{
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent)
    , m_webSocket(new QWebSocket)
    , m_connected(false)
    , m_status("Disconnected")
{
    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &WebSocketClient::onError);
}

// 以下保持原有函数不变
void WebSocketClient::connectToServer(const QString &url)
{
    setStatus("Connecting...");
    m_webSocket->open(QUrl(url));
}

void WebSocketClient::disconnectFromServer()
{
    m_webSocket->close();
}

void WebSocketClient::sendMessage(const QString &message)
{
    if (m_connected) {
        m_webSocket->sendTextMessage(message);
    }
}

void WebSocketClient::onConnected()
{
    setConnected(true);
    sendMessage(json_to_run);
    emit runJson(json_to_run);
    setStatus("Connected");
    qDebug() << "WebSocket connected";
}

void WebSocketClient::onDisconnected()
{
    setConnected(false);
    setStatus("Disconnected");
    emit runJson(json_to_run);
    qDebug() << "WebSocket disconnected";
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    m_lastMessage = message;
    qDebug() << "Message received:" << message;

    emit messageReceived(message);
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    QString errorString = QString("Error: %1").arg(m_webSocket->errorString());
    setStatus(errorString);
    emit errorOccurred(errorString);
    qDebug() << "WebSocket error:" << errorString;
}

void WebSocketClient::setConnected(bool connected)
{
    if (m_connected != connected) {
        m_connected = connected;
        emit connectedChanged();
    }
}

void WebSocketClient::setStatus(const QString &status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}
