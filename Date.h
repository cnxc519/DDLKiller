#ifndef DATE_H
#define DATE_H

#include <QObject>
#include<QDate>

class Date:public QObject{
    Q_OBJECT
public:
    explicit Date(QObject *parent=nullptr);

    Q_INVOKABLE QString getExplicitDate(int year,int month,int day);

    Q_INVOKABLE int getmodelindex(int,int,int);
};

int calculateDaysDifference(int,int,int);
int daysFromZero(int);

// QDate curDate;

QString displayExplicitDate(int,int ,int);

QString weekToChinese(int dayOfWeek);

QString monthsLaterText(int);

#endif // DATE_H
