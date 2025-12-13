#include "Date.h"
#include<QDebug>
#include<QDate>


QDate curDate = QDate::currentDate();

int currentDayOfWeek=curDate.dayOfWeek();//获取今日星期

Date::Date(QObject *parent):QObject(parent) {}

QString Date::getExplicitDate(int year,int month, int day)
{
    // qDebug()<<"This is C++ talking,name:"<<name<<"age:"<<age;
    // return QString(name+":"+QString::number(age)+"years old");

    // qDebug()<<"This is C++ talking,month:"<<month<<"day:"<<day;

    // int a=Date::calculatejuzhezhou0duoshaotian(month,day);

    // return QString(QString::number((month-11)*30+(day-4)));

    // int jujin=julitianshu(2025,10,5,2025,10,15);

    // qDebug()<<calculatejuzhezhou0duoshaotian(jujin);


    //jujinkaifang=julitianshu(year,month,day);

    //return "666";
    return displayExplicitDate(year,month,day);
}

int daysFromZero(int daysDifference){//参数为距今多少天
    return daysDifference+int(currentDayOfWeek);
}

int Date::getmodelindex(int year,int month,int date){
    int daysDifference=calculateDaysDifference(year,month,date);

    if(daysDifference<=2) return 0;
    else if(daysDifference<=13) return 1;
    else return 2;

}

QString displayExplicitDate(int y,int m,int d){

    int daysDifference=calculateDaysDifference(y,m,d);

    if(daysDifference<=-2){
        //TODO:发信号,删除信息,refresh
        return "程序出错,未自动删除成功";
    }

    switch (daysDifference) {
    case -1:
        return "昨天结束";//可以加一个信号
        break;
    case 0:
        return "今天";//可以加一个信号
        break;
    case 1:
        return "明天";//可以加一个信号
        break;
    case 2:
        return "后天";
        break;
    case 3:
        return "大后天";
        break;
    default:

        break;
    }

    int cur=daysFromZero(daysDifference);

    switch ((cur-1)/7) {
    case 0:
        return "本周"+weekToChinese(cur%7);//TODO
        break;
    case 1:
        return "下周"+weekToChinese(cur%7);//实际上应该是QString类型函数
        break;
    case 2:
        return "下下周"+weekToChinese(cur%7);
        break;
    case 3:
        return "第三周周"+weekToChinese(cur%7);
        break;
    case 4:
        return "第四周周"+weekToChinese(cur%7);
        break;
    case 5:
        return "第五周周"+weekToChinese(cur%7);
        break;
    default:
        return monthsLaterText(daysDifference/30);
        break;
    }
}
QString monthsLaterText(int a){
    switch(a){
    case 1:
        return "一个月后";
        break;
    case 2:
        return "两个月后";
        break;
    case 3:
        return "三个月后";
        break;
    case 4:
        return "四个月后";
        break;
    case 5:
        return "五个月后";
        break;
    case 6:
        return "六个月后";
        break;
    default:
        return "很久以后";
        break;
    }
}

int calculateDaysDifference(int ddlyear,int ddlmonth,int ddlday){
    qDebug()<<curDate.year()<<"-"<<curDate.month()<<"-"<<curDate.day();

    int todayyear=curDate.year(),todaymonth=curDate.month(),todayday=curDate.day();

    struct tm time1 = {0};
    struct tm time2 = {0};

    //设置今日日期
    time1.tm_year = todayyear - 1900;
    time1.tm_mon = todaymonth - 1;
    time1.tm_mday = todayday;

    // 设置第二个日期
    time2.tm_year = ddlyear - 1900;
    time2.tm_mon = ddlmonth - 1;
    time2.tm_mday = ddlday;

    // 转换为 time_t
    time_t t1 = mktime(&time1);
    time_t t2 = mktime(&time2);

    // 计算天数差
    double difference = difftime(t2, t1);
    return static_cast<int>(difference / (60 * 60 * 24));
}

QString weekToChinese(int dayOfWeek){//从数字返回今天是周几

    switch (dayOfWeek) {
    case 1:
        return "一";
        break;
    case 2:
        return "二";
        break;
    case 3:
        return "三";
        break;
    case 4:
        return "四";
        break;
    case 5:
        return "五";
        break;
    case 6:
        return "六";
        break;
    case 0:
        return "日";
        break;

    default:
        break;
    }

}

//#include <iostream>
//#include <ctime>
//using namespace std;
//
//struct Date {
//    int year;
//    int month;
//    int day;
//};
//
//int CalculateDate(Date a, Date b) {
//    struct tm time1 = {0};
//    struct tm time2 = {0};
//
//    // 设置第一个日期
//    time1.tm_year = a.year - 1900;
//    time1.tm_mon = a.month - 1;
//    time1.tm_mday = a.day;
//
//    // 设置第二个日期
//    time2.tm_year = b.year - 1900;
//    time2.tm_mon = b.month - 1;
//    time2.tm_mday = b.day;
//
//    // 转换为 time_t
//    time_t t1 = mktime(&time1);
//    time_t t2 = mktime(&time2);
//
//    // 计算天数差
//    double difference = difftime(t2, t1);
//    return static_cast<int>(difference / (60 * 60 * 24));
//}
//
//int main() {
//    Date a, b;
//    cin >> a.year >> a.month >> a.day;
//    cin >> b.year >> b.month >> b.day;
//    cout << abs(CalculateDate(a, b));
//    return 0;
//}
//总程序,需要返回QString类型
//我们需要一个程序,返回int类型,距这周零多少天
//我们需要一个程序zhoulingToQString,把距这周0多少天按照逻辑返回qstring类型
//我们需要一个程序,只需要返回今天是周几(x)
//我们需要一个程序,需要返回目标日期距今多少天,我们希望向这个程序输入任务截止日期的年月日
//我们还希望向上一行的程序录入今天是几月几日,并把年月日传递给上一行的程序
