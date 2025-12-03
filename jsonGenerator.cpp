#include "jsonGenerator.h"
#include<QDebug>

JsonGenerator::JsonGenerator(QObject *parent):QObject(parent) {}

QString JsonGenerator::generateAddJson(const QString &name,int year,int month,int day){
    QString jsonwaited=QString("{\"type\": \"modification\",\"content\": {\"operation\": \"add\",\"uuid\":\"%1\",\"title\": \"%2\",\"description\":\"\",\"due_date\": \"%3\", \"complete_flag\": false}}").arg(generateUuid()).arg(name).arg(formatDate(year,month,day));
    qDebug()<<jsonwaited;
    return jsonwaited;
    //QString json = QString("{\"name\": \"%1\", \"age\": %2}").arg(name).arg(age);
}

QString JsonGenerator::generateDeleteJson(const QString &uuid){

    QString jsonwaited=QString("{\"type\":\"modification\",\"content\":{\"operation\":\"delete\",\"target_id\":\"%1\"}}").arg(uuid);
    qDebug()<<jsonwaited;
    return jsonwaited;
}

QString JsonGenerator::generateModifyJson(const QString &uuid,const QString &name,int year,int month,int day){
    QString jsonwaited=QString("{\"type\":\"modification\",\"content\": {\"operation\":\"modify\",\"target_id\":\"%1\",\"last_modified\":\"%2\",\"title\":\"%3\",\"description\":\"\",\"due_date\":\"%4\",\"complete_flag\": false}}").arg(uuid).arg(generateUuid()).arg(name).arg(formatDate(year,month,day));
    qDebug()<<jsonwaited;
    return jsonwaited;
}

QString formatDate(int year,int month,int day) {
    QDate date(year, month, day);
    if (date.isValid()) {
        return date.toString("yyyy-MM-dd");
    } else {
        return "invaliddate";//QString(); // 或者处理错误情况
    }
}

QString generateUuid()
{
    return QString::number(QDateTime::currentMSecsSinceEpoch());
}
