#include "DataSql.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSqlRecord>
#include <QSqlError>
//8.148.4.26:8090
DataSql::DataSql(QObject *parent) : QObject(parent)
{
    // 设置数据库类型为SQLite
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

DataSql::~DataSql()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DataSql::initializeDatabase()
{
    // 获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create app data directory:" << dataPath;
            emit errorOccurred("Failed to create app data directory");
            return false;
        }
    }

    // 设置数据库文件路径
    QString dbPath = dataPath + "/appdata.db";
    qDebug() << "Database path:" << dbPath;
    m_db.setDatabaseName(dbPath);

    // 打开数据库
    if (!m_db.open()) {
        QString error = m_db.lastError().text();
        qWarning() << "Failed to open database:" << error;
        emit errorOccurred("Database error: " + error);
        return false;
    }

    qDebug() << "Database opened successfully";
    //更新数据表样式前调用
    //模拟器同样也需要调用一次
    QString deleteTable="DROP TABLE IF EXISTS users";

    if (!executeQuery(deleteTable)) {
        return false;
    }

    // 创建示例表
    QString createTable = "CREATE TABLE IF NOT EXISTS users ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "name TEXT, "
                          "year INTEGER,"
                          "month INTEGER,"
                          "day INTEGER)";

    if (!executeQuery(createTable)) {
        return false;
    }

    // QString createTable = R"(
    //     CREATE TABLE IF NOT EXISTS todo_items (
    //         uuid TEXT PRIMARY KEY,
    //         last_modified TEXT NOT NULL,
    //         title TEXT NOT NULL,
    //         description TEXT,
    //         due_date TEXT,
    //         complete_flag INTEGER DEFAULT 0,
    //         offline_add INTEGER DEFAULT 0,
    //         offline_delete INTEGER DEFAULT 0
    //     )
    // )";

    // 检查表中是否有数据
    // QSqlQuery checkQuery("SELECT COUNT(*) FROM users");
    // if (checkQuery.next() && checkQuery.value(0).toInt() == 0) {
    //     // 只有表中没有数据时才插入示例数据
    //     QString insertData = "INSERT INTO users (name, email, age) VALUES "
    //                          "('John Doe', 'john@example.com', 30), "
    //                          "('Jane Smith', 'jane@example.com', 25)";

    //     if (!executeQuery(insertData)) {
    //         return false;
    //     }
    // }

    emit databaseInitialized(true);
    return true;
}

bool DataSql::executeQuery(const QString &query)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database not open when executing query:" << query;
        emit errorOccurred("Database is not open");
        return false;
    }

    QSqlQuery q;
    if (!q.exec(query)) {
        QString error = q.lastError().text();
        qWarning() << "Query failed:" << query << "Error:" << error;
        emit errorOccurred("Query error: " + error);
        return false;
    }

    m_query = q; // 保存查询结果
    emit queryExecuted(true);
    return true;
}

bool DataSql::executeQueryWithParams(const QString &query, const QVariantList &params)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database not open when executing query with params:" << query;
        emit errorOccurred("Database is not open");
        return false;
    }

    QSqlQuery q;
    q.prepare(query);

    for (const QVariant &param : params) {
        q.addBindValue(param);
    }

    if (!q.exec()) {
        QString error = q.lastError().text();
        qWarning() << "Query with params failed:" << query << "Error:" << error;
        emit errorOccurred("Query error: " + error);
        return false;
    }

    m_query = q; // 保存查询结果
    emit queryExecuted(true);
    return true;
}

QVariantList DataSql::getQueryResults()
{
    QVariantList results;

    QSqlQuery cnt(QSqlDatabase::database());
    cnt.exec("SELECT COUNT(*) FROM users");
    cnt.next();
    qDebug() << "数据库里真实行数" << cnt.value(0).toInt();

    emit queryHangShu(cnt.value(0).toInt());


    if (!m_query.isActive()) {
        qWarning() << "Query is not active";
        return results;
    }

    // 重置查询位置
    m_query.seek(-1);

    while (m_query.next()) {
        QVariantMap row;
        QSqlRecord record = m_query.record();
        for (int i = 0; i < record.count(); i++) {
            QString fieldName = record.fieldName(i);
            row[fieldName] = m_query.value(i);
        }
        results.append(row);
    }

    return results;
}
