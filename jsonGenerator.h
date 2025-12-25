//json模板生成

#ifndef JSONGENERATOR_H
#define JSONGENERATOR_H

#include <QObject>
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

class JsonGenerator:public QObject{
    Q_OBJECT
public:
    explicit JsonGenerator(QObject *parent=nullptr);

    Q_INVOKABLE QString generateAddJson(const QString &name,int year,int month,int day);

    Q_INVOKABLE QString generateDeleteJson(const QString &uuid);

    Q_INVOKABLE QString generateModifyJson(const QString &uuid,const QString &name,int year,int month,int day);
signals:
    void runJson(const QString &jtr);

    // {/"type/":/"modification/",/"content/":{/"operation/":/"delete/",/"target_id/":/"20240615093000123/"}}

    // {/"type/":/"modification/",/"content/": {/"operation/":/"modify/",/"target_id/":/"20251129001659123/",/"last_modified/":/"20240620093000123/",/"title/":/"Modified Title/",/"description/":/"/",/"due_date/":/"2024-12-31/",/"complete_flag/": false}}
};

QString formatDate(int year,int month,int day);

QString generateUuid();

#endif // JSONGENERATOR_H
