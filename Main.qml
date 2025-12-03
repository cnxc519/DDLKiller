import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 400
    height: 650
    title: "Styled Nested ListView"

    // 存储三个组的模型，以便外部访问
    property var groupModels: []
    // 数据库状态
    property bool dbInitialized: false
    property string dbStatus: "Initializing database..."
    property int hangshu:0

    //日历的开放存储
    property int zancunyear
    property int zancunmonth
    property int zancundatee
    property date cDate:new Date()
    property int yearrr:cDate.getFullYear()
    property int monthhh:cDate.getMonth()+1
    property int dateee:cDate.getDate()

    property bool isModifying:false
    property int modifyid

    // 连接数据库信号
    Connections {
        target: dataSql

        function onDatabaseInitialized(success) {
            dbInitialized = success
            if (success) {
                dbStatus = "Database initialized successfully"
                refreshData()
            } else {
                dbStatus = "Database initialization failed"
            }
        }

        function onQueryExecuted(success) {
            if (success) {
                dbStatus = "Query executed successfully"
            }
        }

        function onErrorOccurred(errorMessage) {
            dbStatus = "Error: " + errorMessage
        }

        function onQueryHangShu(hangshuu){
            hangshu=hangshuu
            dbStatus=hangshu
        }
    }

    // 刷新数据函数
    function refreshData() {
        if (!dbInitialized) {
            dbStatus = "Cannot refresh data: database not initialized"
            return
        }

        if (!dataSql.executeQuery("SELECT * FROM users ORDER BY year*2000+month*100+day")) {
            return
        }

        groupModels[0].clear()
        groupModels[1].clear()
        groupModels[2].clear()
        var results = dataSql.getQueryResults()
        for (var i = 0; i < results.length; i++) {
            groupModels[date.getmodelindex(results[i].year,results[i].month,results[i].day)].append(results[i])//20250
            // var l=results[i].month
            // print(1)
        }

        if (results.length === 0) {
            dbStatus = "No users found in database"
        }
        namefield.text = ""

        calendarSelector.selectedDate=calenderwhole.nowDate//实际上放在add进list那个按钮上比较好
        calendarSelector.currentDate=calenderwhole.nowDate

        yearrr=cDate.getFullYear()
        monthhh=cDate.getMonth()+1
        dateee=cDate.getDate()

        warning.visible=0
    }

    // 添加新项目函数
    function addUser() {
        if (!dbInitialized) {
            dbStatus = "Cannot add user: database not initialized"//
            return
        }

        var name = namefield.text
        var year = yearrr//parseInt(yearfield.text)
        var month = monthhh//parseInt(monthfield.text)
        var day = dateee//parseInt(dateefield.text)

        if (name === "" || month === NaN|| day === NaN || year===NaN) {
            dbStatus = "Name and date are required"
            return
        }

        var query = "INSERT INTO users (name,year, month,day) VALUES (?,?,?,?)"
        var params = [name,year,month,day]//isNaN(month) ? null : moth

        if (!dataSql.executeQueryWithParams(query, params)) {
            return
        }
        //sortData()
        refreshData()
        // 清空输入字段
        //yearfield.text=""
        //namefield.text = ""
        //monthfield.text = ""
        //dateefield.text=""


        //currentDate和selectedDate变为new Date()

        dbStatus = "User added successfully"
    }

    // 删除项目函数
    function deleteData(idd){

        var query = "DELETE FROM users WHERE id=?"
        var params=[idd]
        if (!dataSql.executeQueryWithParams(query, params)) {
            return
        }
        refreshData()
        dbStatus = "User deleted successfully"
        return
    }

    //添加新事项界面,待美化界面,将日历加入,日历加入已完成
    //点add就退出界面逻辑不对,应该是判断not null,还应该有个取消按钮是直接退出

    //两个问题
    //1.add应该判断name not null,已完成
    //2.输入框禁用,已完成
    //3.输入mask的accepted设为true,已完成
    Item{
        anchors.fill:parent
        id:tianruxinxi
        visible:false
        z:100
        Rectangle{//mask
            anchors.fill:parent
            color:"#ffffff"
            MouseArea{
                onClicked: {
                    mouse.accepted=true
                }
            }
        }
        Column{
            spacing:10
            Column{
                Row{
                    spacing:3
                    Text{
                        id:mingcheng
                        text:"名称:"
                        font.pixelSize: 16
                        font.bold: true
                    }
                    TextField{
                        id:namefield
                        text:"DDL"
                        height: 30
                        placeholderText: "Enter Name..."
                        width:150
                        font.pixelSize: 13
                    }
                }
                Text{
                    x:mingcheng.x+15
                    //height:15
                    id:warning
                    color:"red"
                    font.pixelSize:12
                    text:"Name is required."
                }
            }
            Row{
                spacing:3
                Text{
                    text:`日期: ${yearrr}-${monthhh}-${dateee}`
                    font.pixelSize: 16
                    font.bold: true
                }
                Rectangle{
                    height:25
                    width:80
                    color:"#6366f1"
                    radius:12.5
                    //border.color: "blue"
                    //border.width: 1
                    border.color: "#3730a3"  // 添加边框增加层次感
                    border.width: 1.5
                    Text{
                        text:"更改日期"
                        anchors.centerIn: parent
                        color:choosedate.pressed?"grey":"white"//"lightgreen"
                    }
                    MouseArea{
                        id:choosedate
                        anchors.fill:parent
                        onClicked: {
                            namefield.focus=false
                            calenderwhole.visible=1
                            var nDate = new Date(yearrr,monthhh-1,dateee)
                            calendarSelector.selectedDate = nDate
                            calendarSelector.dateSelected(nDate)
                        }
                    }
                }
            }
            Row{
                Button{
                    text:isModifying?"Modify":"Add"
                    onClicked:
                    {
                        if(namefield.text!=="")
                        {
                            if(!isModifying){
                                tianruxinxi.visible=0;
                                jsonGenerator.generateAddJson(namefield.text,yearrr,monthhh,dateee)
                                addUser()
                                namefield.text=""
                            }
                            else{
                                tianruxinxi.visible=0;
                                jsonGenerator.generateModifyJson(modifyid,namefield.text,yearrr,monthhh,dateee)
                                isModifying=0
                                modifyid=0//TODO
                                namefield.text=""
                            }
                        }
                        else warning.visible=true
                    }//TODO
                }
                Button{
                    text:"Cancel"
                    onClicked:{
                        refreshData()
                        isModifying=0
                        modifyid=0//TODO
                        tianruxinxi.visible=0
                    }
                }
            }
        }
    }

    //日历整体组件
    Item{
        visible:false
        anchors.fill:parent
        id:calenderwhole
        z:200

        property date nowDate: new Date()

        CalendarSelector {
            id: calendarSelector
            anchors.centerIn: parent
            // 可选：自定义颜色
            // primaryColor: "#ff6b6b"
            // accentColor: "#4ecdc4"
            onDateSelected: {
                console.log("选择的日期:", Qt.formatDate(selectedDate, "yyyy-MM-dd"))
                console.log("选择的年份:",selectedDate.getFullYear())
                console.log("选择的月份:",selectedDate.getMonth()+1)
                console.log("选择的日期:",selectedDate.getDate())
                zancunyear=selectedDate.getFullYear()
                zancunmonth=selectedDate.getMonth()+1
                zancundatee=selectedDate.getDate()
            }
            // 初始化时设置特定日期（可选）
            // Component.onCompleted: {
            //     calendarSelector.goToDate(currentDate.getFullYear(), currentDate.getMonth(), currentDate.getDate())
            // }
        }
        MaskCalender{
            id:maskCalender
            anchors.fill:parent
            MouseArea{//要设置大小或anchors,不然scale=0
                anchors.fill:parent
                onClicked:{
                    mouse.accepted=true
                }
            }

            //visible:false
        }
        Row{

            anchors {
                top: calendarSelector.bottom
                horizontalCenter: parent.horizontalCenter
                margins: 10
            }
            spacing:20
            Rectangle{

                id:confirmCalender
                width:80
                height:40
                radius: 20
                //color:blue
                visible:true
                Text {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    text: "确定"
                    font.pixelSize: 13
                    font.bold: true
                    color: preMouseArea.pressed ? "grey" : "green"
                }
                MouseArea {
                    id: preMouseArea
                    width: parent.width
                    height: parent.height
                    hoverEnabled: true
                    onClicked: {
                        yearrr=zancunyear
                        monthhh=zancunmonth
                        dateee=zancundatee
                        calenderwhole.visible=0
                    }

                }
            }
            Rectangle{

                id:cancelCalender
                width:80
                height:40
                radius: 20
                //color:blue
                visible:true
                Text {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    text: "取消"
                    font.pixelSize: 13
                    font.bold: true
                    color: preeMouseArea.pressed ? "grey" : "red"
                }
                MouseArea {
                    id: preeMouseArea
                    width: parent.width
                    height: parent.height
                    hoverEnabled: true
                    onClicked: {
                        calenderwhole.visible=0
                        // calendarSelector.selectedDate=calenderwhole.nowDate
                        // calendarSelector.currentDate=calenderwhole.nowDate
                    }
                }
            }
        }
    }

    //添加事项按钮,最终版本应该是可长按可点按的图案
    Button{
        text:"添加事项"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked:{
            tianruxinxi.visible=1
        }
    }

    //三个listview显示
    Column {
        anchors.fill: parent
        //C++方法返回index即可,已完成
        //进阶:根据sql中result中返回的index值进行添加,已完成
        ListView {
            id: outerList
            width: parent.width
            height: parent.height - 50
            clip: true
            spacing: 16

            model: ListModel {
                // ListElement { title: "三天内"; color: "#FFD6A5" }
                // ListElement { title: "两周内"; color: "#BDB2FF" }
                // ListElement { title: "长期"; color: "#A0C4FF" }
                ListElement { title: "三天内"; groupColor: "#FFE8D6" }  // 柔和的橙色
                ListElement { title: "两周内"; groupColor: "#E6F3FF" }  // 柔和的蓝色
                ListElement { title: "长期"; groupColor: "#F0E6FF" }    // 柔和的紫色
            }

            delegate: Item {
                width: parent.width
                height: contentColumn.implicitHeight + 16

                property var subModel: ListModel { }


                Component.onCompleted: {
                    // 将每个组的模型存储到全局数组中
                    if (groupModels.length <= index) {
                        groupModels.push(subModel)
                    } else {
                        groupModels[index] = subModel
                    }
                }

                Rectangle {
                    id: groupCard
                    width: parent.width - 24
                    height: contentColumn.implicitHeight + 12
                    radius: 12
                    color: model.groupColor
                    anchors.horizontalCenter: parent.horizontalCenter

                    Column {
                        id: contentColumn
                        width: parent.width
                        spacing: 10
                        anchors.margins: 12

                        // Group Title
                        Text {
                            text: title
                            font.bold: true
                            font.pointSize: 12
                            color: "#333"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        // Inner Task List

                        //id:innerList
                        //我们先只把下面这个注释
                        ListView {
                            id: innerList
                            width: parent.width
                            height: contentHeight
                            model: subModel
                            interactive: false
                            spacing: 6

                            delegate: Rectangle {
                                width: ListView.view.width-20//无-20
                                height: 56  // 列表项高度
                                color: "#ffffff" //index % 2 === 0 ? "#f5f5f5" : "#ffffff"
                                border.color: "#e0e0e0"
                                radius: 10//4
                                anchors.horizontalCenter: parent.horizontalCenter

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 12
                                    Rectangle{
                                        height:30
                                        width:30
                                        radius:height/2
                                        border.color: "#87CEEB"
                                        border.width: 2
                                        z:1
                                        Layout.alignment: Qt.AlignVCenter
                                        MouseArea{
                                            z:1
                                            anchors.fill:parent
                                            onClicked: {
                                                jsonGenerator.generateDeleteJson(model.id);
                                                deleteData(model.id)
                                            }
                                        }
                                    }

                                    // 左对齐的名称
                                    Text {
                                        Layout.fillWidth: true
                                        Layout.alignment: Qt.AlignVCenter
                                        text: model.name
                                        elide: Text.ElideRight
                                        font.pixelSize: 14
                                        font.bold: true
                                        color: "#333333"
                                    }

                                    // 右对齐的文本元素三
                                    Text {
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.preferredWidth: Math.min(implicitWidth, 100)
                                        text: date.getExplicitDate(model.year,model.month,model.day)//"explicitdate"
                                        horizontalAlignment: Text.AlignRight//20250
                                        font.pixelSize: 12
                                        color: "#666666"
                                        elide: Text.ElideLeft
                                    }

                                    // 分割线
                                    Rectangle {
                                        Layout.alignment: Qt.AlignVCenter
                                        width: 1
                                        height: 30
                                        color: "#e0e0e0"
                                        opacity: 0.6
                                    }

                                    // 右侧两列文本的容器
                                    Column {
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.preferredWidth: 60
                                        spacing: 2

                                        // 第一行文本
                                        Text {
                                            width: parent.width
                                            text: `${model.year}-${model.month}-${model.day}`//最好用dateformat那种形式yyyy-MM-dd
                                            font.pixelSize: 11
                                            color: "#666666"
                                            elide: Text.ElideRight
                                            horizontalAlignment: Text.AlignRight
                                        }

                                        // 第二行文本
                                        Text {
                                            width: parent.width
                                            text: "10:30"
                                            font.pixelSize: 10
                                            color: "#999999"
                                            elide: Text.ElideRight
                                            horizontalAlignment: Text.AlignRight
                                        }
                                    }
                                }

                                // 鼠标悬停效果
                                MouseArea {
                                    anchors.fill: parent
                                    anchors.leftMargin: 40
                                    z:0
                                    hoverEnabled: true
                                    onEntered: parent.border.color = "#2196F3"
                                    onExited: parent.border.color = "#e0e0e0"
                                    onClicked: console.log("点击了:", name)
                                    onPressAndHold: {
                                        isModifying=1
                                        modifyid=model.id
                                        namefield.text=model.name
                                        tianruxinxi.visible=1
                                        console.log( "方法1: 长按触发成功!")
                                    }
                                }
                            }//rectangle
                            //先只是注释delegate的Rectangle
                            // delegate: Rectangle {
                            //     width: contentColumn.width - 40
                            //     height: 36
                            //     radius: 8
                            //     color: index % 2 === 0 ? "#ffffff" : "#f9f9f9"
                            //     anchors.horizontalCenter: parent.horizontalCenter
                            //     border.color: "#ccc"

                            //     Text {
                            //         text: `${model.name}:${model.month}.${model.day}`
                            //         anchors.centerIn: parent
                            //         color: "#333"
                            //     }
                            //     MouseArea {
                            //         width: 30
                            //         height:30
                            //         Text{
                            //             anchors.fill:parent
                            //             anchors.leftMargin: 10
                            //             text:"x"
                            //             width:parent.width*2
                            //             height:parent.height*2
                            //             color:"red"
                            //         }

                            //         onClicked: {
                            //             deleteData(model.id)
                            //         }
                            //     }
                        }
                    }//listview

                    // Add Button
                    // Button {
                    //     text: "＋ Add Task"
                    //     anchors.horizontalCenter: parent.horizontalCenter
                    //     background: Rectangle {
                    //         color: Qt.lighter(color, 1.1)
                    //         radius: 6
                    //     }
                    //     onClicked: subModel.append({ "name": "New Task " + (subModel.count + 1) })
                    // }
                }
            }
        }
    }
}

