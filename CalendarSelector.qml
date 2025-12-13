// CalendarSelector.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle{
    id: root
    width: 250
    height:350
    implicitWidth: 250
    implicitHeight: 350
    z:2
    color:"white"
    radius: 10

    // 开放的属性
    property date selectedDate: new Date()
    property alias currentDate: calendar.currentDate

    // 优雅颜色
    property color primaryColor: "#6366f1"
    property color secondaryColor: "#8b5cf6"
    property color accentColor: "#06b6d4"
    property color textColor: "#1e293b"
    property color lightTextColor: "#64748b"
    property color hoverColor: "#f1f5f9"
    property color shadowColor: "#00000020"

    // 当日期被选择时发信号
    signal dateSelected(date selectedDate)


    Column {
        x: 12
        y: 12
        width: parent.width - 24
        height: parent.height - 24
        spacing: 8

        // 显示当前选中日期
        Rectangle {
            width: parent.width
            height: 32
            radius: 8
            color: "white"
            border.color: "#e2e8f0"
            border.width: 1

            // 模拟阴影效果
            Rectangle {
                x: 1
                y: 1
                width: parent.width - 2
                height: parent.height - 2
                radius: parent.radius - 1
                color: "transparent"
                border.color: "#00000008"
                border.width: 1
            }

            Label {
                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                text: Qt.formatDate(root.selectedDate, "yyyy年MM月dd日")
                font.pixelSize: 14
                font.bold: true
                color: root.primaryColor
            }
        }

        // 月份导航
        Rectangle {
            width: parent.width
            height: 36
            radius: 8
            color: "white"
            border.color: "#e2e8f0"
            border.width: 1

            // 模拟阴影效果
            Rectangle {
                x: 1
                y: 1
                width: parent.width - 2
                height: parent.height - 2
                radius: parent.radius - 1
                color: "transparent"
                border.color: "#00000008"
                border.width: 1
            }

            Row {
                x: 6
                y: 3
                width: parent.width - 12
                height: parent.height - 6
                spacing: (parent.width - 12 - 30 - 30 - monthText.width) / 2

                // 上月按钮
                Rectangle {
                    id: prevButton
                    width: 30
                    height: 30
                    radius: 15
                    color: prevMouseArea.pressed ? root.primaryColor : (prevMouseArea.containsMouse ? root.hoverColor : "transparent")
                    border.color: root.primaryColor
                    border.width: 1

                    Text {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2
                        text: "←"
                        font.pixelSize: 12
                        font.bold: true
                        color: prevMouseArea.pressed ? "white" : root.primaryColor
                    }

                    MouseArea {
                        id: prevMouseArea
                        width: parent.width
                        height: parent.height
                        hoverEnabled: true
                        onClicked: calendar.showPreviousMonth()
                    }
                }

                // 月份显示
                Label {
                    id: monthText
                    y: (parent.height - height) / 2
                    text: Qt.formatDate(calendar.currentDate, "yyyy年MM月")
                    font.pixelSize: 14
                    font.bold: true
                    color: root.textColor
                }

                // 下月按钮
                Rectangle {
                    id: nextButton
                    width: 30
                    height: 30
                    radius: 15
                    color: nextMouseArea.pressed ? root.primaryColor : (nextMouseArea.containsMouse ? root.hoverColor : "transparent")
                    border.color: root.primaryColor
                    border.width: 1

                    Text {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2
                        text: "→"
                        font.pixelSize: 12
                        font.bold: true
                        color: nextMouseArea.pressed ? "white" : root.primaryColor
                    }

                    MouseArea {
                        id: nextMouseArea
                        width: parent.width
                        height: parent.height
                        hoverEnabled: true
                        onClicked: calendar.showNextMonth()
                    }
                }
            }
        }

        // 星期标题
        Row {
            width: parent.width
            height: 24
            spacing: 1

            Repeater {
                model: ["日", "一", "二", "三", "四", "五", "六"]

                Rectangle {
                    width: (parent.width - 6) / 7
                    height: parent.height
                    radius: 4
                    color: "transparent"

                    Label {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2
                        text: modelData
                        font.pixelSize: 11
                        font.bold: true
                        color: index === 0 || index === 6 ? "#ef4444" : root.lightTextColor
                    }
                }
            }
        }

        // 日历网格
        Grid {
            width: parent.width
            height: parent.height - 32 - 36 - 24 - 24
            rows: 6
            columns: 7
            rowSpacing: 3
            columnSpacing: 3

            Repeater {
                id: dayRepeater
                model: 42 // 6×7日历

                Item {
                    id: dayItem
                    width: (parent.width - 18) / 7
                    height: (parent.height - 15) / 6

                    property bool isCurrentMonth: index >= calendar.firstDayIndex &&
                                                index < calendar.firstDayIndex + calendar.daysInMonth
                    property bool isSelected: {
                        if (!isCurrentMonth) return false

                        var day = index - calendar.firstDayIndex + 1
                        var itemDate = new Date(calendar.currentDate.getFullYear(),
                                              calendar.currentDate.getMonth(),
                                              day)

                        return itemDate.getTime() === root.selectedDate.getTime()
                    }
                    property bool isToday: isCurrentMonth &&
                                         (index - calendar.firstDayIndex + 1) === new Date().getDate() &&
                                         calendar.currentDate.getMonth() === new Date().getMonth() &&
                                         calendar.currentDate.getFullYear() === new Date().getFullYear()
                    property int dayNumber: {
                        if (index < calendar.firstDayIndex) {
                            return calendar.daysInPreviousMonth - calendar.firstDayIndex + index + 1
                        } else if (index >= calendar.firstDayIndex + calendar.daysInMonth) {
                            return index - calendar.firstDayIndex - calendar.daysInMonth + 1
                        } else {
                            return index - calendar.firstDayIndex + 1
                        }
                    }

                    // 圆形背景
                    Rectangle {
                        id: dayBackground
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2
                        width: Math.min(parent.width, parent.height) - 2//原4
                        height: width
                        radius: width / 2
                        color: {
                            if (dayItem.isSelected) return root.primaryColor
                            if (dayMouseArea.containsMouse && dayItem.isCurrentMonth) return root.hoverColor
                            return "transparent"
                        }
                        border.color: {
                            if (dayItem.isToday) return root.accentColor
                            if (dayItem.isSelected) return root.primaryColor
                            return "transparent"
                        }
                        border.width: dayItem.isToday ? 1 : 0

                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }

                    // 日期文本
                    Text {
                        x: (parent.width - width) / 2//(parent.width - width) / 2
                        y: (parent.height - height) / 2
                        text: dayItem.dayNumber
                        font.pixelSize: dayItem.isSelected ? 16 : 15//11:10
                        font.bold: dayItem.isSelected
                        color: {
                            if (!dayItem.isCurrentMonth) return "#cbd5e1"
                            if (dayItem.isSelected) return "white"
                            if (index % 7 === 0 || index % 7 === 6) return "#ef4444"
                            return root.textColor
                        }
                    }


                    // 鼠标交互区域
                    MouseArea {
                        id: dayMouseArea
                        width: parent.width
                        height: parent.height
                        hoverEnabled: true
                        onClicked: {
                            if (dayItem.isCurrentMonth) {
                                var day = index - calendar.firstDayIndex + 1
                                var newDate = new Date(calendar.currentDate.getFullYear(),
                                                      calendar.currentDate.getMonth(),
                                                      day)
                                root.selectedDate = newDate
                                root.dateSelected(newDate)
                            }
                        }

                        onEntered: {
                            if (dayItem.isCurrentMonth && !dayItem.isSelected) {
                                dayBackground.scale = 1.1
                            }
                        }
                        onExited: {
                            if (!dayItem.isSelected) {
                                dayBackground.scale = 1.0
                            }
                        }
                    }

                    Behavior on scale {
                        NumberAnimation { duration: 100; easing.type: Easing.OutCubic }
                    }
                }
            }
        }
    }

    // 日历逻辑
    QtObject {
        id: calendar

        property date currentDate: new Date()

        property int firstDayIndex: new Date(currentDate.getFullYear(), currentDate.getMonth(), 1).getDay()

        property int daysInMonth: new Date(currentDate.getFullYear(), currentDate.getMonth() + 1, 0).getDate()

        property int daysInPreviousMonth: new Date(currentDate.getFullYear(), currentDate.getMonth(), 0).getDate()

        function showPreviousMonth() {
            currentDate = new Date(currentDate.getFullYear(), currentDate.getMonth() - 1, 1)
        }

        function showNextMonth() {
            currentDate = new Date(currentDate.getFullYear(), currentDate.getMonth() + 1, 1)
        }
    }

    // 公共方法
    function goToToday() {
        var today = new Date()
        calendar.currentDate = new Date(today.getFullYear(), today.getMonth(), 1)
        root.selectedDate = today
    }

    function goToDate(year, month, day) {
        //calendar.currentDate = new Date(year, month - 1, 1)
        root.selectedDate = new Date(year, month - 1, day)
    }
}
