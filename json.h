//json模板处理分析

#ifndef JSON_H
#define JSON_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QVariant>

class JsonProcessor : public QObject
{
    Q_OBJECT

public:
    explicit JsonProcessor(QObject *parent = nullptr);

    Q_INVOKABLE QVariantMap parseAndProcessJson(const QString &jsonData);
    Q_INVOKABLE QString generateFullUpdateJson(const QVariantList &items);
    Q_INVOKABLE QString generateSyncJson(const QVariantMap &syncData); // 生成同步JSON

signals:
    void jsonProcessed(const QString &operation, bool success, const QString &message);
    void jsonError(const QString &errorMessage);

private:
    QVariantList parseFullUpdateContent(const QJsonArray &contentArray);
    QVariantMap parseModificationContent(const QJsonObject &contentObject);
};

#endif // JSON_H
