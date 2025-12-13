import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

//TODO
//1.进入客户端在main.cpp自动连接,并在连接时执行全量更新,执行成功后所有offline设置为0,连接失败则websocket.connected设为false
//2.每次添加/删除/修改时,先进行一次连接尝试
//在一些情况下是客户端发送信息,在一些情况下是服务器端发送信息
//客户端向服务器发送:1.我上线了/我需要执行全量更新了
//                  ->服务器发送自己端所有数据,向访问设备
//                  ->访问设备更新为结合后的在线数据,转化为json格式全部打包给服务器
//                  ->服务器器接收全量更新逻辑
//                  ->服务器向所有设备发送最新数据,所有设备重载数据
//                2.我添加/删除/修改了什么,type为modification
//                  ->服务器接受数据,并把原样json直接广播给其他所有在线设备
//                  ->服务器端自己也执行一次
//服务器端向客户端发送:1.只有代表操作(full_update,modification)的json格式
//                  2.这是发送给刚上线设备的,代表要进行特殊full_update后还要返回给服务器
//离线相关:1.离线状态点击全量更新显示error
//        2.离线状态进行add/delete/modify,会自动生成一个全局储存的json格式,随后尝试connect,在onconnected/onerror后进行对储存的json进行process
//          ->我们必须保证这不是"登录/full_update"的connect结果,或许我们可以存储一个自动为不执行的json格式,比如type=hold,每次json处理成功后转为hold的type,只有在需要时短暂转化为所需保存的json
//
//全局待执行json,默认为hold,每次用完改成hold
//
//我们需要的功能函数:尝试连接->发送connect>连接成功->接受full_update_mix->执行processjson->update结束后发信号
//                                           ->一个函数接受信号->该函数将目前所有数据库内容转为json发送给服务器
//                                    ->连接失败,isconnected=false(实际上用不到这个值)
//一接收到消息就processjson,后面逻辑都在processjson中处理
//收到的消息是add,delete,modify,full_update_blanket,直接执行,没有任何问题
//收到的消息是full_update_mix,执行原fullupdate函数,finish后将自己全部数据库按照特定json格式发送
//接受消息的所有可能如上
//
//
//自己add/delete/modify逻辑
//生成对应json,保存在c++->尝试连接->发送connect->onconnected->生成的json发给服务器->m_isconnected作为最新在线离线标识全局可访,用它执行processjson
//                                         ->onerror                        /^^^
//
//点击全量更新->连接失败并不会有提示,除非想每次离线使用打开应用就弹出提示词
//
//modify要改到ddlthird那,并且输入ui应该优化
//当执行modify操作时,应该一定有last_modified为generateuuid
//执行add操作时,last_modified和uuid默认为generateuuid
//
//具体到我需要写的函数与调用流程:
//1.创建static的json_to_run,默认为{"type":"hold"},直接""的QString就可以,因为处理json文本的时候会return这种情况
//2.在onconnected后和onerror后都执行databasemanager.processjson(json_to_run)
//3.ddlthird长按可modify
//4.add操作时生成uuid和last_modified
//5.记得处理一个{"type":"response"}
//6.full_update_noresponce:clearall并覆盖,按照对话中原来的那个函数就可以
//7.full_update之后要执行向服务器发送消息
//8.一个函数,将目前的数据库生成为json格式,准备发送给服务器
//9.每次接受服务器消息都直接processjson,不用管其他的
//10.processjson需要把接受的消息due_date转化为year,month,day存储
//11.准备add/delete/modify->先生成json格式json_to_run->执行connect->json_to_run初始化
//12.结合json解析和上线两段代码
//13.ddlthird输入UI优化,修改时弹出输入的UI,但是按钮是确认修改
//14.ddlthird中把add和delete的json格式生成并保存
//15.手动改isonline全可以删去
//16.进入即json_to_run变成request并connect

//12.4
//1.genneratejson里面对jsontorun复制
//2.每次执行完都执行jsontorun=""
//3.groupmodel整合,注意日期提取

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

    //日历的开放存储
    property int tempYear
    property int tempMonth
    property int tempDay
    property date cDate:new Date()
    property int storedYear:cDate.getFullYear()
    property int storedMonth:cDate.getMonth()+1
    property int storedDay:cDate.getDate()

    property bool isModifying:false
    property string modifyUUID

    Rectangle{
        width:80
        height:40
        radius:20
        //border.width:1
        color:"lightgrey"
        anchors.right: parent.right
        anchors.bottom:parent.bottom
        Text{
            anchors.centerIn: parent
            horizontalAlignment:Text.AlignHCenter
            verticalAlignment:Text.AlignVCenter
            text:"全量更新"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:{
                websocket.fullUpdate=true;

                websocket.connectToServer("ws://8.148.4.26:8090");
            }
        }
    }

    // 刷新数据函数
    function refreshData() {

        groupModels[0].clear()
        groupModels[1].clear()
        groupModels[2].clear()
        //var results = dataSql.getQueryResults()

        var results = databaseManager.getTodoItems();

        for (var i = 0; i < results.length; i++) {

            var due_time=parseDate(results[i].due_date)
            console.log("年:", due_time.year);   // 2025
            console.log("月:", due_time.month);  // 7
            console.log("日:", due_time.day);

            groupModels[date.getmodelindex(due_time.year,due_time.month,due_time.day)].append(results[i])//20250
            // var l=results[i].month
            // print(1)
        }

        if (results.length === 0) {
            dbStatus = "No users found in database"
        }
        namefield.text = ""

        calendarSelector.selectedDate=calenderwhole.nowDate//实际上放在add进list那个按钮上比较好
        calendarSelector.currentDate=calenderwhole.nowDate

        storedYear=cDate.getFullYear()
        storedMonth=cDate.getMonth()+1
        storedDay=cDate.getDate()

        warning.visible=0
    }

    Component.onCompleted: {
        console.log("Application loaded");
        refreshData();

        // 连接信号
        databaseManager.operationCompleted.connect(function(operation, success, message) {
            //statusText.text = message;
            if (success) {
                refreshData();
            }
        });


        jsonProcessor.jsonError.connect(function(errorMessage) {
            //statusText.text = "JSON Error: " + errorMessage;
        });

        // 加载一个默认示例
        loadExample("full_update");
    }

    Connections{
        target:websocket


        function onMessageReceived(jtr){
            var jsonText = jtr;
            if (jsonText.trim() === "") {
                statusText.text = "Please enter JSON data";
                return;
            }

            console.log("Processing JSON...");
            var parseResult = jsonProcessor.parseAndProcessJson(jsonText);

            if (parseResult.success) {
                console.log("JSON parsed successfully, operation:", parseResult.operation);
                databaseManager.processJsonResult(parseResult);
                websocket.json_to_run="";
            }
            else {
                statusText.text = "JSON Error: " + parseResult.error;
                websocket.json_to_run="";
            }
        }
    }

    Connections{
        target:jsonGenerator

        function onRunJson(jtr){
            var jsonText = jtr;
            if (jsonText.trim() === "") {
                statusText.text = "Please enter JSON data";
                return;
            }

            console.log("Processing JSON...");
            var parseResult = jsonProcessor.parseAndProcessJson(jsonText);

            if (parseResult.success) {
                console.log("JSON parsed successfully, operation:", parseResult.operation);
                databaseManager.processJsonResult(parseResult);
                websocket.json_to_run="";
            }
            else {
                statusText.text = "JSON Error: " + parseResult.error;
                websocket.json_to_run="";
            }
        }
    }

    Connections{
        target:databaseManager
        function onReplyToServer(){
            websocket.json_to_run=databaseManager.getTodoItemsAsJsonString();
            console.log(websocket.json_to_run)
            websocket.sendMessage(websocket.json_to_run);
            websocket.json_to_run="";
            databaseManager.resetOfflineFlags();
        }
    }



    //添加新事项界面,待美化界面,将日历加入,日历加入已完成
    //点add就退出界面逻辑不对,应该是判断not null,还应该有个取消按钮是直接退出

    //两个问题
    //1.add应该判断name not null,已完成
    //2.输入框禁用,已完成
    //3.输入mask的accepted设为true,已完成
    Item{
        //anchors.fill:parent
        anchors.centerIn: parent
        width:250
        height:350
        id:tianruxinxi
        visible:false
        z:100
        MaskCalender{
            //id:maskCalender
            anchors.centerIn:parent
            width:3000
            height:3000
            MouseArea{//要设置大小或anchors,不然scale=0
                anchors.fill:parent
                onClicked:{
                    mouse.accepted=true
                }
            }

            //visible:false
        }
        Rectangle{//mask
            anchors.fill:parent
            color:"#ffffff"
            radius:10
            MouseArea{
                onClicked: {
                    mouse.accepted=true
                }
            }
        }
        Column{
            Rectangle{
                color:"transparent"
                height:10
                width:10
            }

            spacing:30
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
                    text:`日期: ${storedYear}-${storedMonth}-${storedDay}`
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
                            var nDate = new Date(storedYear,storedMonth-1,storedDay)
                            calendarSelector.selectedDate = nDate
                            calendarSelector.dateSelected(nDate)
                        }
                    }
                }
            }
            Row{
                Rectangle{
                    width:80
                    height:40
                    radius:20
                    border.width:1
                    border.color:"lightgrey"
                    Text{
                        anchors.centerIn: parent
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment:Text.AlignVCenter
                        color:"green"
                        text:isModifying?"Modify":"Add"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked:
                        {
                            if(namefield.text!=="")
                            {
                                if(!isModifying){
                                    tianruxinxi.visible=0;
                                    jsonGenerator.generateAddJson(namefield.text,storedYear,storedMonth,storedDay)
                                    //addUser()
                                    namefield.text=""
                                }
                                else{
                                    tianruxinxi.visible=0;
                                    jsonGenerator.generateModifyJson(modifyUUID,namefield.text,storedYear,storedMonth,storedDay)
                                    isModifying=0
                                    modifyUUID=0//TODO
                                    namefield.text=""
                                }
                            }
                            else warning.visible=true
                        }//TODO
                    }
                }

                Rectangle{
                    width:80
                    height:40
                    radius:20
                    border.width:1
                    border.color:"lightgrey"
                    Text{
                        color:"red"
                        anchors.centerIn: parent
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment:Text.AlignVCenter
                        text:"Cancel"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked:{
                            refreshData()
                            isModifying=0
                            modifyUUID=0//TODO
                            tianruxinxi.visible=0
                        }
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
                tempYear=selectedDate.getFullYear()
                tempMonth=selectedDate.getMonth()+1
                tempDay=selectedDate.getDate()
            }
            // 初始化时设置特定日期
            // Component.onCompleted: {
            //     calendarSelector.goToDate(currentDate.getFullYear(), currentDate.getMonth(), currentDate.getDate())
            // }
        }
        MaskCalender{
            id:maskCalender
            anchors.fill:parent
            opacity:0.8
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
                        storedYear=tempYear
                        storedMonth=tempMonth
                        storedDay=tempDay
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


    Rectangle{
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width:80
        height:40
        radius:20
        color:"lightgrey"
        Text{
            anchors.centerIn: parent
            horizontalAlignment:Text.AlignHCenter
            verticalAlignment:Text.AlignVCenter
            text:"添加事项"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:{
                tianruxinxi.visible=1
            }
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

                        // 空Item，用于撑开顶部空间
                            Item {
                                width: parent.width
                                height: 0.5
                            }
                        // Group Title
                        Text {
                            text: title
                            font.bold: true
                            font.pointSize: 12
                            color: "#333"
                            anchors.horizontalCenter: parent.horizontalCenter
                            //anchors.topMargin: 50
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
                                                jsonGenerator.generateDeleteJson(model.uuid);
                                                //deleteData(model.uuid)
                                            }
                                        }
                                    }

                                    // 左对齐的名称
                                    Text {
                                        Layout.fillWidth: true
                                        Layout.alignment: Qt.AlignVCenter
                                        text: model.title
                                        elide: Text.ElideRight
                                        font.pixelSize: 14
                                        font.bold: true
                                        color: "#333333"
                                    }

                                    // 右对齐的文本元素三
                                    Text {
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.preferredWidth: Math.min(implicitWidth, 100)


                                        text: date.getExplicitDate(parseDate(model.due_date).year,parseDate(model.due_date).month,parseDate(model.due_date).day)//"explicitdate"
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
                                            text: `${parseDate(model.due_date).year}-${parseDate(model.due_date).month}-${parseDate(model.due_date).day}`//最好用dateformat那种形式yyyy-MM-dd
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
                                        modifyUUID=model.uuid
                                        namefield.text=model.title
                                        tianruxinxi.visible=1
                                        console.log( "方法1: 长按触发成功!")
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    function parseDate(dateString) {
        var parts = dateString.split("-");
        if (parts.length === 3||parts.length === 4) {
            return {
                year: parseInt(parts[0]),
                month: parseInt(parts[1]),
                day: parseInt(parts[2])
            };
        }
        return { year: 0, month: 0, day: 0 };
    }
}

