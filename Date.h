#ifndef DATE_H
#define DATE_H

#include <QObject>
#include<QDate>

class Date:public QObject{
    Q_OBJECT
public:
    explicit Date(QObject *parent=nullptr);

    Q_INVOKABLE QString getExplicitDate(int year,int month,int day);

    Q_INVOKABLE int jujinkaifang;

    Q_INVOKABLE int getmodelindex(int,int,int);
};

int julitianshu(int,int,int);
int calculatejuzhezhou0duoshaotian(int);

// QDate curDate;

QString xianshiyikanriqi(int,int ,int);

QString weektohanzi(int );

QString jigeyue(int);

#endif // DATE_H
