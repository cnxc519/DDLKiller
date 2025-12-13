#include "database.h"
#include "websocketclient.h"
//#include "websocketclient.h"

//有个向服务器发送成功的消息,收到那个后所以offline变为0
//任何full_update后都代表连接着,可以执行reset

// 单例实例初始化
DatabaseManager* DatabaseManager::instance = nullptr;

DatabaseManager* DatabaseManager::getInstance()
{
    if (!instance) {
        instance = new DatabaseManager();
    }
    return instance;
}

void DatabaseManager::destroyInstance()
{
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)//, m_isOnline(true) // 默认在线
{
    initializeDatabase();
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

// bool DatabaseManager::isOnline() const
// {
//     return m_isOnline;
// }

// void DatabaseManager::setIsOnline(bool online)
// {
//     if (m_isOnline != online) {
//         m_isOnline = online;
//         emit isOnlineChanged(online);
//         qInfo() << "Online status changed to:" << (online ? "Online" : "Offline");
//     }
// }

bool DatabaseManager::initializeDatabase()
{
    // 获取应用数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString dbPath = dataPath + "/todo.db";

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qWarning() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }

    qInfo() << "Database opened successfully:" << dbPath;
    return true;
}

bool DatabaseManager::createTable()
{
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return false;
    }
    // QSqlQuery queryy;
    // QString dcreateTableSQL = R"(
    //     DROP TABLE IF EXISTS todo_items
    // )";

    // if (!queryy.exec(dcreateTableSQL)) {
    //     qWarning() << "Failed to create table:" << queryy.lastError().text();
    //     return false;
    // }
    QSqlQuery query;
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS todo_items (
            uuid TEXT PRIMARY KEY,
            last_modified TEXT NOT NULL,
            title TEXT NOT NULL,
            description TEXT,
            due_date TEXT,
            complete_flag INTEGER DEFAULT 0,
            offline_add INTEGER DEFAULT 0,
            offline_delete INTEGER DEFAULT 0
        )
    )";

    if (!query.exec(createTableSQL)) {
        qWarning() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    qInfo() << "Todo items table created or already exists";
    return true;
}

QJsonObject DatabaseManager::getTodoItemsAsJson()
{
    QVariantList todoItems = getTodoItems();
    QJsonObject jsonObject;
    QJsonArray contentArray;

    // 设置type字段
    jsonObject["type"] = "full_update";

    // 遍历todo items并构建JSON数组
    for (const QVariant &itemVariant : todoItems) {
        QVariantMap itemMap = itemVariant.toMap();
        QJsonObject itemObject;

        itemObject["uuid"] = itemMap["uuid"].toString();
        itemObject["last_modified"] = itemMap["last_modified"].toString();
        itemObject["title"] = itemMap["title"].toString();
        itemObject["description"] = itemMap["description"].toString();
        itemObject["due_date"] = itemMap["due_date"].toString();
        itemObject["complete_flag"] = itemMap["complete_flag"].toBool();

        contentArray.append(itemObject);
    }

    // 设置content字段
    jsonObject["content"] = contentArray;

    return jsonObject;
}

QString DatabaseManager::getTodoItemsAsJsonString()
{
    QJsonObject jsonObject = getTodoItemsAsJson();
    QJsonDocument doc(jsonObject);
    return doc.toJson(QJsonDocument::Compact);
}

QVariantList DatabaseManager::getTodoItems()
{

    //WebSocketClient* webSocketClient = WebSocketClient::getInstance();
    QVariantList items;

    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return items;
    }

    // 在线状态下不显示标记为离线删除的项目
    //webSocketClient->connected() ?
    QString querySql =   "SELECT uuid, last_modified, title, description, due_date, complete_flag, offline_add, offline_delete FROM todo_items WHERE offline_delete = 0 ORDER BY due_date";
    //"SELECT uuid, last_modified, title, description, due_date, complete_flag, offline_add, offline_delete FROM todo_items ORDER BY due_date";

    QSqlQuery query(querySql);

    while (query.next()) {
        QVariantMap item;
        item["uuid"] = query.value(0).toString();
        item["last_modified"] = query.value(1).toString();
        item["title"] = query.value(2).toString();
        item["description"] = query.value(3).toString();
        item["due_date"] = query.value(4).toString();
        item["complete_flag"] = query.value(5).toInt() == 1;
        item["offline_add"] = query.value(6).toInt() == 1;
        item["offline_delete"] = query.value(7).toInt() == 1;
        items.append(item);
    }

    return items;
}

bool DatabaseManager::processJsonResult(const QVariantMap &jsonResult)
{
    if (!jsonResult.contains("success") || !jsonResult["success"].toBool()) {
        QString error = jsonResult.contains("error") ? jsonResult["error"].toString() : "Unknown error";
        emit operationCompleted("parse", false, "JSON parsing failed: " + error);
        return false;
    }

    QString operation = jsonResult["operation"].toString();
    bool success = false;
    QString message;

    if (operation == "full_update") {
        QVariantList items = jsonResult["items"].toList();
        success = processFullUpdate(items);
        message = success ?
                      QString("Full update completed with %1 items").arg(items.size()) :
                      "Full update failed";
    }
    else if (operation=="full_update_noresponse"){
        QVariantList items = jsonResult["items"].toList();
        success = processFullUpdateNoresponse(items);
        message = success ?
                      QString("Full update completed with %1 items").arg(items.size()) :
                      "Full update failed";
    }
    else if (operation == "add") {
        QVariantMap operationData = jsonResult["data"].toMap();
        success = processAddOperation(operationData);
        message = success ? "Add operation completed" : "Add operation failed";
    }
    else if (operation == "delete") {
        QVariantMap operationData = jsonResult["data"].toMap();
        success = processDeleteOperation(operationData);
        message = success ? "Delete operation completed" : "Delete operation failed";
    }
    else if (operation == "modify") {
        QVariantMap operationData = jsonResult["data"].toMap();
        success = processModifyOperation(operationData);
        message = success ? "Modify operation completed" : "Modify operation failed";
    }
    else {
        message = "Unknown operation: " + operation;
    }

    emit operationCompleted(operation, success, message);

    if (success) {
        emit dataChanged();
    }

    return success;
}

bool DatabaseManager::processFullUpdate(const QVariantList &items)
{
    return applyServerChanges(items);
}

bool DatabaseManager::applyServerChanges(const QVariantList &serverItems)
{
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return false;
    }

    // 获取当前所有UUID（包括标记为删除的）
    QSet<QString> currentUuids = getCurrentUuids();

    // 获取服务器数据的UUID集合
    QSet<QString> serverUuids;
    for (const QVariant &itemVar : serverItems) {
        QVariantMap item = itemVar.toMap();
        serverUuids.insert(item["uuid"].toString());
    }
    // 处理服务器添加的项目
    handleServerAddtions(serverUuids, currentUuids);

    // 处理数据冲突（相同UUID但最后修改时间不同,或离线offline_delete）
    handleDataConflicts(serverItems);

    // 处理服务器添加的项目
    //handleServerDeletions(serverUuids, currentUuids);



    // 新增：处理服务器没有但本地有的数据（需要删除）
    handleServerDeletions(serverUuids, currentUuids);

    // 处理离线添加的项目
    handleOfflineAdditions(serverUuids);


    //目前问题:服务器没有的但是本地有的,没有正确删除,已解决
    //emit dataChanged();

    emit replyToServer();

    return true;
}

void DatabaseManager::handleServerDeletions(const QSet<QString> &serverUuids, const QSet<QString> &currentUuids)
{
    // 查找本地有但服务器没有的项目
    QSet<QString> localOnlyUuids = currentUuids - serverUuids;

    if (localOnlyUuids.isEmpty()) {
        qInfo() << "No local-only items to delete";
        return;
    }

    qInfo() << "Found" << localOnlyUuids.size() << "items maybe to delete (local only)";

    // 删除这些项目
    QSqlQuery query;
    query.prepare("DELETE FROM todo_items WHERE uuid = ? AND offline_add = 0");

    for (const QString &uuid : localOnlyUuids) {
        query.addBindValue(uuid);
        if (!query.exec()) {
            qWarning() << "Failed to delete local-only item:" << query.lastError().text();
        } else {
            qInfo() << "Deleted local-only item:" << uuid;
        }
    }
}

QSet<QString> DatabaseManager::getCurrentUuids()
{
    QSet<QString> uuids;

    if (!m_database.isOpen()) {
        return uuids;
    }

    QSqlQuery query("SELECT uuid FROM todo_items");
    while (query.next()) {
        uuids.insert(query.value(0).toString());
    }

    return uuids;
}

void DatabaseManager::handleOfflineAdditions(const QSet<QString> &serverUuids)
{
    // 查找本地有但服务器没有的项目，且是离线添加的
    QSqlQuery query;
    query.prepare("SELECT uuid FROM todo_items WHERE offline_add = 1");

    QSet<QString> offlineAddUuids;
    while (query.next()) {
        QString uuid = query.value(0).toString();
        if (!serverUuids.contains(uuid)) {
            offlineAddUuids.insert(uuid);
        }
    }

    // 对于离线添加但服务器没有的项目，保持原样（等待同步）
    qInfo() << "Found" << offlineAddUuids.size() << "offline additions to sync";
}

void DatabaseManager::handleServerAddtions(const QSet<QString> &serverUuids, const QSet<QString> &currentUuids)
{
    // 查找服务器有但本地没有的项目
    QSet<QString> serverOnlyUuids = serverUuids - currentUuids;

    // 这些是其他设备添加的项目，需要插入到本地
    qInfo() << "Found" << serverOnlyUuids.size() << "items added by other devices";
}

void DatabaseManager::handleDataConflicts(const QVariantList &serverItems)
{
    // 使用事务处理数据冲突
    m_database.transaction();

    for (const QVariant &itemVar : serverItems) {
        QVariantMap serverItem = itemVar.toMap();
        QString uuid = serverItem["uuid"].toString();
        QString serverLastModified = serverItem["last_modified"].toString();

        // 检查本地是否存在此项目
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT last_modified, offline_delete FROM todo_items WHERE uuid = ?");
        checkQuery.addBindValue(uuid);

        if (checkQuery.exec() && checkQuery.next()) {
            QString localLastModified = checkQuery.value(0).toString();
            bool offlineDelete = checkQuery.value(1).toInt() == 1;

            // 如果本地标记为离线删除，但服务器有此项，说明离线删除,应该执行硬删除
            if (offlineDelete) {
                QSqlQuery updateQuery;
                updateQuery.prepare(R"(
                    DELETE FROM todo_items WHERE uuid = ?
                )");

                updateQuery.addBindValue(uuid);

                if (!updateQuery.exec()) {
                    qWarning() << "Failed to restore deleted item:" << updateQuery.lastError().text();
                }
            }
            // 比较最后修改时间，使用最新的数据
            else if (serverLastModified > localLastModified) {
                QSqlQuery updateQuery;
                updateQuery.prepare(R"(
                    UPDATE todo_items
                    SET last_modified = ?, title = ?, description = ?, due_date = ?, complete_flag = ?
                    WHERE uuid = ?
                )");

                updateQuery.addBindValue(serverLastModified);
                updateQuery.addBindValue(serverItem["title"].toString());
                updateQuery.addBindValue(serverItem["description"].toString());
                updateQuery.addBindValue(serverItem["due_date"].toString());
                updateQuery.addBindValue(serverItem["complete_flag"].toBool() ? 1 : 0);
                updateQuery.addBindValue(uuid);

                if (!updateQuery.exec()) {
                    qWarning() << "Failed to update item with server data:" << updateQuery.lastError().text();
                }
            }
        } else {
            // 本地没有此项，插入服务器数据
            QSqlQuery insertQuery;
            insertQuery.prepare(R"(
                INSERT INTO todo_items
                (uuid, last_modified, title, description, due_date, complete_flag, offline_add, offline_delete)
                VALUES (?, ?, ?, ?, ?, ?, 0, 0)
            )");

            insertQuery.addBindValue(uuid);
            insertQuery.addBindValue(serverLastModified);
            insertQuery.addBindValue(serverItem["title"].toString());
            insertQuery.addBindValue(serverItem["description"].toString());
            insertQuery.addBindValue(serverItem["due_date"].toString());
            insertQuery.addBindValue(serverItem["complete_flag"].toBool() ? 1 : 0);

            if (!insertQuery.exec()) {
                qWarning() << "Failed to insert server item:" << insertQuery.lastError().text();
            }
        }
    }

    if (!m_database.commit()) {
        qWarning() << "Failed to commit transaction:" << m_database.lastError().text();
        m_database.rollback();
    }
}

bool DatabaseManager::processAddOperation(const QVariantMap &operationData)
{
    //WebSocketClient* webSocketClient = WebSocketClient::getInstance();
    QVariantMap item = operationData["item"].toMap();
    return insertTodoItem(item, 1); // 离线状态下添加标记为离线添加!webSocketClient->connected(),默认离线,收到发送成功信号后在线
}

bool DatabaseManager::processDeleteOperation(const QVariantMap &operationData)
{
    //WebSocketClient* webSocketClient = WebSocketClient::getInstance();
    QString targetId = operationData["target_uuid"].toString();
    return deleteTodoItem(targetId, 1); // 离线状态下删除标记为离线删除!webSocketClient->connected(),默认离线,收到发送成功信号后在线
}

bool DatabaseManager::processModifyOperation(const QVariantMap &operationData)
{
    QString targetId = operationData["target_uuid"].toString();
    QVariantMap itemData = operationData["item"].toMap();
    return updateTodoItem(targetId, itemData);
}

bool DatabaseManager::processFullUpdateNoresponse(const QVariantList &items)
{
    if (!clearAllData()) {
        return false;
    }

    bool allSuccess = true;
    for (const QVariant &itemVar : items) {
        if (!insertTodoItem(itemVar.toMap())) {
            allSuccess = false;
        }
    }


    return allSuccess;
}

bool DatabaseManager::clearAllData()
{
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return false;
    }

    QSqlQuery query;
    if (!query.exec("DELETE FROM todo_items")) {
        qWarning() << "Failed to clear todo items:" << query.lastError().text();
        return false;
    }

    qInfo() << "All todo items cleared from database";
    return true;
}

bool DatabaseManager::insertTodoItem(const QVariantMap &item, bool isOfflineAdd)
{
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return false;
    }

    QString uuid = item["uuid"].toString();
    QString lastModified = item.contains("last_modified") && !item["last_modified"].toString().isEmpty() ?
                               item["last_modified"].toString() : generateUuid();
    QString title = item["title"].toString();
    QString description = item["description"].toString();
    QString dueDate = item["due_date"].toString();
    bool completeFlag = item["complete_flag"].toBool();

    QSqlQuery query;
    query.prepare(R"(
        INSERT OR REPLACE INTO todo_items
        (uuid, last_modified, title, description, due_date, complete_flag, offline_add, offline_delete)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(uuid);
    query.addBindValue(lastModified);
    query.addBindValue(title);
    query.addBindValue(description);
    query.addBindValue(dueDate);
    query.addBindValue(completeFlag ? 1 : 0);
    query.addBindValue(isOfflineAdd ? 1 : 0);
    query.addBindValue(0); // 新插入的项目不是离线删除

    if (!query.exec()) {
        qWarning() << "Failed to insert todo item:" << query.lastError().text();
        return false;
    }

    qInfo() << "Inserted todo item:" << title << (isOfflineAdd ? "(Offline)" : "(Online)");
    emit dataChanged();
    return true;
}

bool DatabaseManager::deleteTodoItem(const QString &uuid, bool isOfflineDelete)
{
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return false;
    }

    if (isOfflineDelete) {
        QSqlQuery query;

        // 先查询该条记录的offline_add状态
        query.prepare("SELECT offline_add FROM todo_items WHERE uuid = ?");
        query.addBindValue(uuid);

        if (!query.exec() || !query.next()) {
            qWarning() << "Failed to query todo item offline_add status:" << query.lastError().text();
            return false;
        }

        bool isOfflineAdd = query.value(0).toBool();

        if (!isOfflineAdd) {
            // offline_add = 0：标记为离线删除但不实际删除
            query.prepare("UPDATE todo_items SET offline_delete = 1 WHERE uuid = ?");
            query.addBindValue(uuid);

            if (!query.exec()) {
                qWarning() << "Failed to mark todo item as offline deleted:" << query.lastError().text();
                return false;
            }

            qInfo() << "Marked todo item as offline deleted:" << uuid;
        } else {
            // offline_add = 1：硬删除
            query.prepare("DELETE FROM todo_items WHERE uuid = ?");
            query.addBindValue(uuid);

            if (!query.exec()) {
                qWarning() << "Failed to delete todo item:" << query.lastError().text();
                return false;
            }

            qInfo() << "Permanently deleted todo item:" << uuid;
        }
    }else {
        // 在线删除：实际删除
        QSqlQuery query;
        query.prepare("DELETE FROM todo_items WHERE uuid = ?");
        query.addBindValue(uuid);

        if (!query.exec()) {
            qWarning() << "Failed to delete todo item:" << query.lastError().text();
            return false;
        }

        qInfo() << "Deleted todo item with UUID:" << uuid;
    }

    emit dataChanged();
    return true;
}

bool DatabaseManager::updateTodoItem(const QString &uuid, const QVariantMap &itemData)
{
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return false;
    }

    QString lastModified = itemData.contains("last_modified") && !itemData["last_modified"].toString().isEmpty() ?
                               itemData["last_modified"].toString() : generateUuid();
    QString title = itemData["title"].toString();
    QString description = itemData["description"].toString();
    QString dueDate = itemData["due_date"].toString();
    bool completeFlag = itemData["complete_flag"].toBool();

    QSqlQuery query;
    query.prepare(R"(
        UPDATE todo_items
        SET last_modified = ?, title = ?, description = ?, due_date = ?, complete_flag = ?
        WHERE uuid = ?
    )");

    query.addBindValue(lastModified);
    query.addBindValue(title);
    query.addBindValue(description);
    query.addBindValue(dueDate);
    query.addBindValue(completeFlag ? 1 : 0);
    query.addBindValue(uuid);

    if (!query.exec()) {
        qWarning() << "Failed to update todo item:" << query.lastError().text();
        return false;
    }

    qInfo() << "Updated todo item:" << title;
    emit dataChanged();
    return true;
}

QVariantMap DatabaseManager::getSyncData()
{
    QVariantMap syncData;
    QVariantList additions;
    QVariantList deletions;

    if (!m_database.isOpen()) {
        return syncData;
    }

    // 获取需要同步的离线添加项目
    QSqlQuery addQuery("SELECT uuid, last_modified, title, description, due_date, complete_flag FROM todo_items WHERE offline_add = 1");
    while (addQuery.next()) {
        QVariantMap item;
        item["uuid"] = addQuery.value(0).toString();
        item["last_modified"] = addQuery.value(1).toString();
        item["title"] = addQuery.value(2).toString();
        item["description"] = addQuery.value(3).toString();
        item["due_date"] = addQuery.value(4).toString();
        item["complete_flag"] = addQuery.value(5).toInt() == 1;
        additions.append(item);
    }

    // 获取需要同步的离线删除项目
    QSqlQuery deleteQuery("SELECT uuid FROM todo_items WHERE offline_delete = 1");
    while (deleteQuery.next()) {
        deletions.append(deleteQuery.value(0).toString());
    }

    syncData["additions"] = additions;
    syncData["deletions"] = deletions;
    syncData["hasData"] = !additions.isEmpty() || !deletions.isEmpty();

    return syncData;
}

bool DatabaseManager::markItemAsSynced(const QString &uuid)
{
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return false;
    }

    // 清除离线标记
    QSqlQuery query;
    query.prepare("UPDATE todo_items SET offline_add = 0, offline_delete = 0 WHERE uuid = ?");
    query.addBindValue(uuid);

    if (!query.exec()) {
        qWarning() << "Failed to mark item as synced:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::resetOfflineFlags()
{
    if (!m_database.isOpen()) {
        qWarning() << "Database is not open";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE todo_items SET offline_add = 0, offline_delete = 0");

    if (!query.exec()) {
        qWarning() << "Failed to reset offline flags:" << query.lastError().text();
        return false;
    }

    qInfo() << "All offline flags have been reset to 0";
    emit dataChanged(); // 通知UI数据已更改
    return true;
}
//若服务器收到更新,则将服务器收到的改为online
void DatabaseManager::markOnlineAdd(const QString &uuid){
    QSqlQuery query;

    query.prepare("UPDATE todo_items SET offline_add = 0 WHERE uuid = ?");

    query.addBindValue(uuid);

    if (!query.exec()) {
        qWarning() << "Failed to markOnlineAdd:" << query.lastError().text();
    }
    else qDebug()<<"Success to markOnlineAdd";

}
void DatabaseManager::markOnlineDelete(const QString &uuid){
    QSqlQuery query;

    query.prepare("DELETE FROM todo_items WHERE uuid = ?");

    query.addBindValue(uuid);

    if (!query.exec()) {
        qWarning() << "Failed to markOnlineDelete:" << query.lastError().text();
    }
    else qDebug()<<"Success to markOnlineDelete";
}

QString DatabaseManager::generateUuid()
{
    return QString::number(QDateTime::currentMSecsSinceEpoch());
}
