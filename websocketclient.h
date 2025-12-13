#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QUrl>

class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)//一个读取函数status(),一个信号函数,还有一个设置函数
    Q_PROPERTY(QString lastMessage READ lastMessage NOTIFY messageReceived)

public:
    // 单例获取方法
    static WebSocketClient* getInstance(QObject* parent = nullptr);
    static void destroyInstance();

    Q_INVOKABLE void connectToServer(const QString &url);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendMessage(const QString &message);

    bool connected() const { return m_connected; }
    QString status() const { return m_status; }
    QString lastMessage() const { return m_lastMessage; }



    //创建全局维护的json_to_run
    QString json_to_run="";

    bool fullUpdate=0;


signals:
    void connectedChanged();
    void statusChanged();
    void messageReceived(const QString &jtr);
    void errorOccurred(const QString &error);


private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);

private:
    // 私有构造函数
    explicit WebSocketClient(QObject *parent = nullptr);
    // 禁止拷贝和赋值
    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;

    static WebSocketClient* instance;

    QWebSocket *m_webSocket;
    bool m_connected;
    QString m_status;
    QString m_lastMessage;
    void setConnected(bool connected);
    void setStatus(const QString &status);
};

#endif // WEBSOCKETCLIENT_H
