#ifndef DATASQL_H
#define DATASQL_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariantList>

class DataSql : public QObject
{
    Q_OBJECT
public:
    explicit DataSql(QObject *parent = nullptr);
    ~DataSql();

    // 初始化数据库
    Q_INVOKABLE bool initializeDatabase();

    // 执行SQL查询
    Q_INVOKABLE bool executeQuery(const QString &query);

    // 执行带参数的SQL查询
    Q_INVOKABLE bool executeQueryWithParams(const QString &query, const QVariantList &params);

    // 获取查询结果
    Q_INVOKABLE QVariantList getQueryResults();

signals:
    void databaseInitialized(bool success);
    void queryExecuted(bool success);
    void errorOccurred(const QString &errorMessage);
    void queryHangShu(int hangshu);

private:
    QSqlDatabase m_db;
    QSqlQuery m_query;
};

#endif // DATASQL_H
