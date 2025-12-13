#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include <QDateTime>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

//记得给一个连接按钮

struct TodoItem {
    QString uuid;
    QString lastModified;
    QString title;
    QString description;
    QString dueDate;
    bool completeFlag;
    bool offlineAdd;      // 新增：标记是否为离线添加
    bool offlineDelete;   // 新增：标记是否为离线删除

    TodoItem() : completeFlag(false), offlineAdd(false), offlineDelete(false) {}
};

class DatabaseManager : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(bool isOnline READ isOnline WRITE setIsOnline NOTIFY isOnlineChanged)

private:
    static DatabaseManager* instance;
    explicit DatabaseManager(QObject *parent = nullptr);

public:
    ~DatabaseManager();

    // 单例模式相关
    static DatabaseManager* getInstance();
    static void destroyInstance();

    Q_INVOKABLE bool createTable();
    Q_INVOKABLE QVariantList getTodoItems();
    Q_INVOKABLE bool processJsonResult(const QVariantMap &jsonResult);

    // 模拟在线状态管理
    // bool isOnline() const;
    // Q_INVOKABLE void setIsOnline(bool online);

    // 同步相关方法
    Q_INVOKABLE QVariantMap getSyncData(); // 获取需要同步的数据（离线添加和删除的）
    Q_INVOKABLE bool markItemAsSynced(const QString &uuid); // 标记已同步
    Q_INVOKABLE bool resetOfflineFlags();

    Q_INVOKABLE QString getTodoItemsAsJsonString();
    QJsonObject getTodoItemsAsJson();

    void markOnlineAdd(const QString &uuid);
    void markOnlineDelete(const QString &uuid);

signals:
    void dataChanged();
    void operationCompleted(const QString &operation, bool success, const QString &message);
    //void isOnlineChanged(bool online);
    void syncDataAvailable();
    void replyToServer();

private:
    QSqlDatabase m_database;
    //bool m_isOnline;

    bool initializeDatabase();
    QString generateUuid();
    bool clearAllData();
    bool insertTodoItem(const QVariantMap &item, bool isOfflineAdd = false);
    bool deleteTodoItem(const QString &uuid, bool isOfflineDelete = false);
    bool updateTodoItem(const QString &uuid, const QVariantMap &itemData);
    bool processFullUpdate(const QVariantList &items);
    bool processFullUpdateNoresponse(const QVariantList &items);
    bool processAddOperation(const QVariantMap &operationData);
    bool processDeleteOperation(const QVariantMap &operationData);
    bool processModifyOperation(const QVariantMap &operationData);

    // 新增的同步处理方法
    bool applyServerChanges(const QVariantList &serverItems);
    QSet<QString> getCurrentUuids();
    void handleOfflineAdditions(const QSet<QString> &serverUuids);
    void handleServerDeletions(const QSet<QString> &serverUuids, const QSet<QString> &currentUuids);
    void handleServerAddtions(const QSet<QString> &serverUuids, const QSet<QString> &currentUuids);
    void handleDataConflicts(const QVariantList &serverItems);
};

#endif // DATABASE_H
