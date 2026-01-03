# DDLonline - 多端同步待办事项管理系统

一个基于WebSocket通信的跨平台待办事项管理应用，支持多设备实时同步，自动任务分级，离线操作等功能。

## 📋 目录

- [功能特性](#-功能特性)
- [技术架构](#-技术架构)
- [系统要求](#-系统要求)
- [快速开始](#-快速开始)
- [详细使用说明](#-详细使用说明)
- [配置说明](#-配置说明)
- [API文档](#api文档)
- [数据格式](#-数据格式)
- [架构详解](#架构详解)
- [故障排除](#-故障排除)
- [开发指南](#-开发指南)
- [贡献指南](#-贡献指南)

## 🚀 功能特性

### 核心功能
- **🔄 实时多端同步**：基于WebSocket实现多设备实时数据同步，任一设备的操作立即反映到其他设备
- **📊 智能任务分级**：根据截止日期自动将任务分为"三天内"、"两周内"、"长期"三个优先级
- **📱 离线优先**：支持离线添加、修改、删除任务，联网后自动同步，保证数据不丢失
- **⚡ 冲突解决**：智能处理多端数据冲突，采用时间戳优先策略，确保数据一致性
- **🎨 现代化界面**：基于QML开发的Material Design风格用户界面，支持深色/浅色主题
- **📅 日历集成**：内置美观的日历选择器，支持快速设置任务截止日期

### 高级特性
- **🔔 智能提醒**：根据任务紧急程度提供不同的视觉提醒
- **📈 数据统计**：实时显示任务完成情况和分布统计
- **🔍 快速搜索**：支持按标题、描述、日期等条件搜索任务
- **📤 数据导出**：支持导出任务数据为JSON、CSV格式
- **🌐 多语言支持**：支持中文、英文等多种语言界面
- **🔒 数据安全**：本地数据加密存储，传输过程SSL加密

## 🏗️ 技术架构

### 整体架构
```
┌─────────────────┐    WebSocket     ┌─────────────────┐
│   客户端A       │ ◄──────────────► │   服务器端      │
│  (Windows)     │                  │  (Node.js)     │
└─────────────────┘                  └─────────────────┘
                                            │
                                            ▼
┌─────────────────┐    WebSocket     ┌─────────────────┐
│   客户端B       │ ◄──────────────► │   数据库        │
│  (Android)     │                  │  (PostgreSQL)   │
└─────────────────┘                  └─────────────────┘
```

### 架构模式
- **C/S架构**：客户端负责交互和本地逻辑，服务端负责数据转发和持久化
- **事件驱动**：基于Qt信号槽机制的事件驱动架构
- **单例模式**：关键管理类采用单例模式确保全局唯一性
- **观察者模式**：UI组件监听数据模型变化，自动更新界面

### 技术栈详情
```yaml
前端技术:
  - 框架: Qt6.8 (Quick, Sql, WebSockets, Core)
  - 语言: C++17, QML, JavaScript
  - UI库: Qt Quick Controls 2.15
  - 图形: OpenGL ES 2.0+

后端技术:
  - 通信: WebSocket (RFC 6455)
  - 数据格式: JSON (RFC 8259)
  - 数据库: SQLite (本地), PostgreSQL (服务器)

开发工具:
  - 构建系统: CMake 3.16+
  - IDE: Qt Creator, Visual Studio Code
  - 版本控制: Git
  - 包管理: vcpkg, Conan
```

### 核心模块架构
```
┌─────────────────────────────────────────────────────────────┐
│                    应用层 (Application Layer)              │
├─────────────────────────────────────────────────────────────┤
│  Main.qml  │  CalendarSelector.qml  │  MaskCalender.qml   │
├─────────────────────────────────────────────────────────────┤
│                    业务逻辑层 (Business Layer)              │
├─────────────────────────────────────────────────────────────┤
│ DatabaseManager │ WebSocketClient │ JsonProcessor │ Date │
├─────────────────────────────────────────────────────────────┤
│                    数据访问层 (Data Layer)                 │
├─────────────────────────────────────────────────────────────┤
│     SQLite Database     │    JSON Messages    │    Files    │
└─────────────────────────────────────────────────────────────┘
```

## 📋 系统要求

### 开发环境要求
```yaml
最低要求:
  - 操作系统: Windows 10 1903+ / macOS 10.15+ / Ubuntu 20.04+
  - Qt版本: Qt 6.8.0 或更高
  - 编译器: MSVC 2019+ / GCC 9+ / Clang 10+
  - CMake: 3.16.0 或更高
  - 内存: 8GB RAM (推荐 16GB+)
  - 存储: 2GB 可用空间

推荐配置:
  - 操作系统: Windows 11 / macOS 12+ / Ubuntu 22.04+
  - Qt版本: Qt 6.8.1 或更高
  - 编译器: MSVC 2022 / GCC 12+ / Clang 14+
  - CMake: 3.25.0 或更高
  - 内存: 16GB+ RAM
  - 存储: 5GB+ 可用空间 (包含开发工具)
```

### 运行时环境
```yaml
桌面端:
  - Windows: Windows 10 1903+ (x64)
  - macOS: macOS 10.15+ (Intel/Apple Silicon)
  - Linux: Ubuntu 20.04+, Fedora 35+, openSUSE Leap 15.4+

移动端:
  - Android: Android 5.0+ (API Level 21+)
  - iOS: iOS 13.0+ (理论上支持，需要额外配置)

网络要求:
  - 协议: WebSocket (ws://) 或 Secure WebSocket (wss://)
  - 端口: 8090 (默认) 或自定义端口
  - 带宽: 最低 1 Mbps (推荐 10 Mbps+)
  - 延迟: < 500ms (推荐 < 100ms)
```

## 🚀 快速开始

### 1. 环境准备

#### Windows环境
```powershell
# 安装Qt (使用vcpkg)
vcpkg install qt6:x64-windows
vcpkg integrate install

# 或下载Qt安装包
# https://www.qt.io/download-qt-installer
```

#### macOS环境
```bash
# 使用Homebrew安装
brew install qt@6 cmake

# 或下载Qt安装包
# https://www.qt.io/download-qt-installer
```

#### Linux环境
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install qt6-base-dev qt6-declarative-dev qt6-websockets-dev cmake build-essential

# 或使用vcpkg
vcpkg install qt6:x64-linux
```

### 2. 获取源码
```bash
git clone https://github.com/cnxc519/DDLKiller.git
cd DDLonline
git submodule update --init --recursive
```

### 3. 配置项目
```bash
# 创建构建目录
mkdir build
cd build

# 配置CMake (Windows)
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake

# 配置CMake (macOS/Linux)
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6
```

### 4. 编译项目
```bash
# Windows (Visual Studio)
cmake --build . --config Release

# macOS/Linux
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
```

### 5. 运行应用
```bash
# Windows
.\Release\DDLonline.exe

# macOS
./DDLonline.app/Contents/MacOS/DDLonline

# Linux
./DDLonline
```

## 📱 详细使用说明

### 首次启动配置

1. **启动应用**：双击可执行文件或在终端中运行
2. **网络连接**：应用会自动尝试连接默认服务器
3. **数据库初始化**：首次运行会自动创建本地数据库
4. **界面加载**：加载QML界面，显示任务列表

### 主界面功能详解

#### 任务列表区域
```
┌─────────────────────────────────────────┐
│  三天内 (3)                            │
│  ┌─────────────────────────────────────┐ │
│  │ ○ 完成作业              2025-01-05 │ │
│  │   今天                     10:30   │ │
│  └─────────────────────────────────────┘ │
│                                         │
│  两周内 (5)                            │
│  ┌─────────────────────────────────────┐ │
│  │ ○ 项目报告              2025-01-12 │ │
│  │   下周二                   14:00   │ │
│  └─────────────────────────────────────┘ │
└─────────────────────────────────────────┘
```

#### 操作按钮说明
- **添加事项**：右下角蓝色按钮，打开任务添加界面
- **全量更新**：右下角灰色按钮，强制同步所有数据
- **任务操作**：
  - 左侧圆形按钮：删除任务
  - 长按任务项：进入编辑模式
  - 点击任务项：查看详细信息

### 任务管理操作

#### 添加新任务
1. 点击"添加事项"按钮
2. 填写任务信息：
   ```
   名称: [必填] 任务标题
   日期: [必填] 截止日期 (点击"更改日期"选择)
   ```
3. 点击"Add"按钮保存
4. 系统自动分配UUID和时间戳
5. 任务根据截止日期自动分级显示

#### 修改现有任务
1. 长按目标任务项
2. 界面切换到编辑模式
3. 修改任务名称或截止日期
4. 点击"Modify"按钮确认修改
5. 系统更新last_modified时间戳

#### 删除任务
1. 点击任务项左侧的圆形删除按钮
2. 任务立即从界面移除
3. 如在线，立即同步到服务器
4. 如离线，标记为离线删除，联网后同步

### 日历选择器使用

#### 日历界面布局
```
┌─────────────────────────────────┐
│        2025年01月              │
│  ←  一月  →                   │
│ 日 一 二 三 四 五 六            │
│                              │
│  1  2  3  4  5  6  7         │
│  8  9 10 11 12 13 14         │
│ 15 16 17 18 19 20 21         │
│ 22 23 24 25 26 27 28         │
│ 29 30 31                     │
│                              │
│        [确定] [取消]           │
└─────────────────────────────────┘
```

#### 操作说明
- **月份导航**：使用左右箭头切换月份
- **日期选择**：点击具体日期进行选择
- **今日标识**：当前日期有特殊边框标识
- **选中状态**：选中日期有蓝色背景

### 任务分级规则

#### 自动分级算法
```cpp
int Date::getmodelindex(int year, int month, int date) {
    int daysDifference = calculateDaysDifference(year, month, date);
    
    if (daysDifference <= 2)    return 0;  // 三天内
    else if (daysDifference <= 13) return 1;  // 两周内
    else return 2;  // 长期
}
```

#### 分级显示规则
- **索引0 (三天内)**：橙色背景 `#FFE8D6`
- **索引1 (两周内)**：蓝色背景 `#E6F3FF`
- **索引2 (长期)**：紫色背景 `#F0E6FF`

#### 日期显示格式
- **今天**：显示"今天"
- **明天**：显示"明天"
- **本周**：显示"周X"
- **下周**：显示"下周周X"
- **更远**：显示"X个月后"

## 🔧 配置说明

### 服务器配置

#### 默认配置
```cpp
// main.cpp 第62行
websocket->connectToServer("ws://8.148.4.26:8090");
```

#### 自定义服务器
```cpp
// 修改服务器地址
websocket->connectToServer("ws://your-server.com:8090");

// 或使用安全WebSocket
websocket->connectToServer("wss://your-server.com:8090");
```

### 数据库配置

#### 数据库文件位置
```cpp
// database.cpp 第57-63行
QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
QString dbPath = dataPath + "/todo.db";
```

#### 各平台路径
- **Windows**: `%APPDATA%/DDLonline/todo.db`
- **macOS**: `~/Library/Application Support/DDLonline/todo.db`
- **Linux**: `~/.local/share/DDLonline/todo.db`

#### 数据库表结构
```sql
CREATE TABLE IF NOT EXISTS todo_items (
    uuid TEXT PRIMARY KEY,           -- 唯一标识符
    last_modified TEXT NOT NULL,      -- 最后修改时间戳
    title TEXT NOT NULL,              -- 任务标题
    description TEXT,                 -- 任务描述
    due_date TEXT,                   -- 截止日期
    complete_flag INTEGER DEFAULT 0,  -- 完成状态 (0/1)
    offline_add INTEGER DEFAULT 0,     -- 离线添加标记 (0/1)
    offline_delete INTEGER DEFAULT 0   -- 离线删除标记 (0/1)
);
```

### 应用配置

#### 界面配置
```qml
// Main.qml
ApplicationWindow {
    visible: true
    width: 400
    height: 650
    title: "DDLonline"
    
    // 可配置参数
    property bool autoSync: true        // 自动同步
    property int syncInterval: 300000   // 同步间隔 (5分钟)
    property bool showNotifications: true // 显示通知
}
```

#### 日志配置
```cpp
// 在main.cpp中设置日志级别
QLoggingCategory::setFilterRules("ddlonline.debug=true");
QLoggingCategory::setFilterRules("ddlonline.network.debug=true");
```

## 📊 API文档

### WebSocket消息协议

#### 客户端发送消息

##### 1. 全量同步请求
```json
{
    "type": "full_update_request",
    "client_id": "client_001",
    "timestamp": "1678888888000"
}
```

##### 2. 添加任务
```json
{
    "type": "modification",
    "content": {
        "operation": "add",
        "uuid": "1678888888001",
        "last_modified": "1678888888",
        "title": "新任务",
        "description": "任务描述",
        "due_date": "2025-01-10",
        "complete_flag": false
    }
}
```

##### 3. 修改任务
```json
{
    "type": "modification",
    "content": {
        "operation": "modify",
        "target_uuid": "1678888888001",
        "last_modified": "1678888889",
        "title": "修改后的标题",
        "description": "",
        "due_date": "2025-01-08",
        "complete_flag": false
    }
}
```

##### 4. 删除任务
```json
{
    "type": "modification",
    "content": {
        "operation": "delete",
        "target_uuid": "1678888888001"
    }
}
```

#### 服务端发送消息

##### 1. 全量更新响应
```json
{
    "type": "full_update",
    "content": [
        {
            "uuid": "1678888888001",
            "last_modified": "1678888888",
            "title": "完成作业",
            "description": "",
            "due_date": "2025-01-05",
            "complete_flag": false
        }
    ],
    "timestamp": "1678888888000"
}
```

##### 2. 增量更新广播
```json
{
    "type": "modification",
    "content": {
        "operation": "add",
        "uuid": "1678888888002",
        "last_modified": "1678888889",
        "title": "新任务",
        "description": "",
        "due_date": "2025-01-12",
        "complete_flag": false
    },
    "source_client": "client_002",
    "timestamp": "1678888889000"
}
```

##### 3. 操作确认
```json
{
    "type": "response",
    "content": {
        "operation": "add",
        "uuid": "1678888888001",
        "status": "success",
        "message": "Task added successfully"
    },
    "timestamp": "1678888888000"
}
```

### C++ API接口

#### DatabaseManager类
```cpp
class DatabaseManager : public QObject {
public:
    // 单例模式
    static DatabaseManager* getInstance();
    static void destroyInstance();
    
    // 数据库操作
    Q_INVOKABLE bool createTable();
    Q_INVOKABLE QVariantList getTodoItems();
    Q_INVOKABLE bool processJsonResult(const QVariantMap &jsonResult);
    
    // 同步相关
    Q_INVOKABLE QVariantMap getSyncData();
    Q_INVOKABLE bool markItemAsSynced(const QString &uuid);
    Q_INVOKABLE bool resetOfflineFlags();
    
    // JSON导出
    Q_INVOKABLE QString getTodoItemsAsJsonString();
    
signals:
    void dataChanged();
    void operationCompleted(const QString &operation, bool success, const QString &message);
    void replyToServer();
};
```

#### WebSocketClient类
```cpp
class WebSocketClient : public QObject {
public:
    // 单例模式
    static WebSocketClient* getInstance(QObject *parent = nullptr);
    static void destroyInstance();
    
    // 连接管理
    Q_INVOKABLE void connectToServer(const QString &url);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void sendMessage(const QString &message);
    
    // 状态查询
    bool connected() const;
    QString status() const;
    QString lastMessage() const;
    
signals:
    void connectedChanged();
    void statusChanged();
    void messageReceived(const QString &jtr);
    void errorOccurred(const QString &error);
};
```

#### Date类
```cpp
class Date : public QObject {
public:
    // 日期处理
    Q_INVOKABLE QString getExplicitDate(int year, int month, int day);
    Q_INVOKABLE int getmodelindex(int year, int month, int date);
    
    // 工具函数
    static QString formatDate(int year, int month, int day);
    static QString generateUuid();
};
```

## 📊 数据格式详解

### 任务数据结构

#### 完整任务对象
```json
{
    "uuid": "1678888888001",           // 唯一标识符 (时间戳)
    "last_modified": "1678888888",      // 最后修改时间 (Unix时间戳)
    "title": "完成作业",                // 任务标题 (必填)
    "description": "数学作业第三章",    // 任务描述 (可选)
    "due_date": "2025-01-05",          // 截止日期 (YYYY-MM-DD格式)
    "complete_flag": false,             // 完成状态 (true/false)
    "offline_add": false,               // 离线添加标记 (内部使用)
    "offline_delete": false             // 离线删除标记 (内部使用)
}
```

#### 数据类型说明
```yaml
uuid:
  类型: String
  格式: 13位数字时间戳
  示例: "1678888888001"
  
last_modified:
  类型: String
  格式: 10位Unix时间戳
  示例: "1678888888"
  
title:
  类型: String
  长度: 1-255字符
  必填: true
  
description:
  类型: String
  长度: 0-1000字符
  必填: false
  
due_date:
  类型: String
  格式: YYYY-MM-DD
  示例: "2025-01-05"
  
complete_flag:
  类型: Boolean
  值: true/false
  默认: false
```

### 消息格式规范

#### 消息头格式
```json
{
    "type": "消息类型",
    "timestamp": "发送时间戳",
    "client_id": "客户端标识"
}
```

#### 消息类型枚举
```yaml
full_update_request:
  描述: 请求全量同步
  发送方: 客户端
  接收方: 服务器
  
full_update:
  描述: 全量数据更新
  发送方: 服务器
  接收方: 客户端
  
modification:
  描述: 增量数据修改
  发送方: 客户端/服务器
  接收方: 服务器/客户端
  
response:
  描述: 操作响应确认
  发送方: 服务器
  接收方: 客户端
```

## 🏗️ 架构详解

### 设计模式应用

#### 1. 单例模式 (Singleton Pattern)
```cpp
// WebSocketClient单例实现
class WebSocketClient : public QObject {
private:
    static WebSocketClient *instance;
    WebSocketClient(QObject *parent = nullptr);
    
public:
    static WebSocketClient *getInstance(QObject *parent = nullptr) {
        if (!instance) {
            instance = new WebSocketClient(parent);
        }
        return instance;
    }
};
```

**应用场景**: WebSocketClient, DatabaseManager
**优势**: 确保全局唯一实例，资源共享

#### 2. 观察者模式 (Observer Pattern)
```cpp
// Qt信号槽机制实现观察者模式
class DatabaseManager : public QObject {
signals:
    void dataChanged();           // 数据变化信号
    void operationCompleted(const QString &operation, bool success, const QString &message);
};

// 在QML中连接信号
Connections {
    target: databaseManager
    function onOperationCompleted(operation, success, message) {
        if (success) {
            refreshData();
        }
    }
}
```

**应用场景**: UI更新、数据同步通知
**优势**: 松耦合，易于扩展

#### 3. 工厂模式 (Factory Pattern)
```cpp
// JSON消息工厂
class JsonGenerator : public QObject {
public:
    QString generateAddJson(const QString &name, int year, int month, int day);
    QString generateDeleteJson(const QString &uuid);
    QString generateModifyJson(const QString &uuid, const QString &name, int year, int month, int day);
};
```

**应用场景**: JSON消息创建
**优势**: 统一创建接口，易于维护

### 数据流架构

#### 数据流向图
```
用户操作 → QML界面 → JsonGenerator → WebSocketClient → 服务器
    ↑                                                    ↓
界面更新 ← DatabaseManager ← JsonProcessor ← WebSocketClient ← 服务器
```

#### 详细流程说明

##### 1. 添加任务流程
```
1. 用户点击"添加事项"
2. QML界面显示输入对话框
3. 用户填写任务信息并确认
4. JsonGenerator.generateAddJson() 创建JSON消息
5. WebSocketClient.sendMessage() 发送到服务器
6. 同时调用本地DatabaseManager.insertTodoItem()
7. 服务器处理并广播给其他客户端
8. 收到response后清除离线标记
```

##### 2. 同步流程
```
1. 客户端连接服务器
2. 发送full_update_request
3. 服务器返回所有数据(full_update)
4. JsonProcessor解析JSON
5. DatabaseManager.processFullUpdate()处理数据
6. 应用冲突解决策略
7. 发送replyToServer信号
8. 将本地完整数据发送给服务器
9. 服务器广播给所有客户端
```

### 状态管理

#### 连接状态管理
```cpp
enum ConnectionState {
    Disconnected,    // 未连接
    Connecting,      // 连接中
    Connected,       // 已连接
    Reconnecting,    // 重连中
    Error           // 连接错误
};
```

#### 数据状态管理
```cpp
enum DataState {
    Online,         // 在线数据
    OfflineAdd,     // 离线添加
    OfflineDelete,  // 离线删除
    Conflict        // 数据冲突
};
```

### 性能优化

#### 1. 数据库优化
```sql
-- 创建索引提高查询性能
CREATE INDEX idx_due_date ON todo_items(due_date);
CREATE INDEX idx_last_modified ON todo_items(last_modified);
CREATE INDEX idx_offline_flags ON todo_items(offline_add, offline_delete);
```

#### 2. 内存管理
```cpp
// 使用QML的ListModel进行懒加载
ListView {
    model: ListModel {
        // 动态加载，只渲染可见项
        delegate: ItemDelegate {
            // 复用组件，减少内存占用
        }
    }
}
```

#### 3. 网络优化
```cpp
// 消息压缩和批量发送
class WebSocketClient {
private:
    QStringList messageQueue;
    QTimer *batchTimer;
    
    void batchSendMessages() {
        if (!messageQueue.isEmpty()) {
            QString batchMessage = createBatchMessage(messageQueue);
            sendMessage(batchMessage);
            messageQueue.clear();
        }
    }
};
```

## 🐛 故障排除

### 常见问题及解决方案

#### 1. 连接问题

##### 问题：无法连接到服务器
**症状**: 应用启动后显示"Disconnected"或"Error"状态
**可能原因**:
- 网络连接问题
- 服务器地址配置错误
- 防火墙阻止连接
- 服务器宕机

**解决方案**:
```bash
# 1. 检查网络连接
ping 8.148.4.26

# 2. 测试WebSocket连接
wscat -c ws://8.148.4.26:8090

# 3. 检查防火墙设置
# Windows: 控制面板 → 系统和安全 → Windows防火墙
# macOS: 系统偏好设置 → 安全性与隐私 → 防火墙

# 4. 修改服务器地址
# 编辑 main.cpp 第62行
websocket->connectToServer("ws://your-server:8090");
```

##### 问题：连接频繁断开
**症状**: 连接后短时间内自动断开
**可能原因**:
- 网络不稳定
- 心跳包超时
- 服务器主动断开

**解决方案**:
```cpp
// 在WebSocketClient中增加心跳机制
void WebSocketClient::startHeartbeat() {
    QTimer *heartbeatTimer = new QTimer(this);
    connect(heartbeatTimer, &QTimer::timeout, this, [this]() {
        if (m_connected) {
            sendMessage("{\"type\":\"heartbeat\"}");
        }
    });
    heartbeatTimer->start(30000); // 30秒间隔
}
```

#### 2. 数据同步问题

##### 问题：数据不同步
**症状**: 多设备间数据不一致
**可能原因**:
- 网络延迟
- 冲突解决策略问题
- 时间戳不同步

**解决方案**:
```cpp
// 1. 强制全量同步
websocket->fullUpdate = true;
websocket->connectToServer("ws://8.148.4.26:8090");

// 2. 检查时间戳同步
qDebug() << "Current timestamp:" << QDateTime::currentMSecsSinceEpoch();

// 3. 清除本地缓存
DatabaseManager::getInstance()->clearAllData();
```

##### 问题：离线操作丢失
**症状**: 离线时添加的任务联网后消失
**可能原因**:
- 离线标记未正确设置
- 同步逻辑错误
- 数据库写入失败

**解决方案**:
```sql
-- 检查离线标记
SELECT * FROM todo_items WHERE offline_add = 1 OR offline_delete = 1;

-- 手动修复离线标记
UPDATE todo_items SET offline_add = 1 WHERE uuid = 'problematic_uuid';
```

#### 3. 界面问题

##### 问题：界面显示异常
**症状**: 任务列表不显示或显示错乱
**可能原因**:
- QML加载失败
- 数据模型更新问题
- 样式文件缺失

**解决方案**:
```qml
// 1. 检查QML控制台输出
// 在Qt Creator中查看Application Output面板

// 2. 强制刷新界面
function refreshData() {
    groupModels[0].clear()
    groupModels[1].clear()
    groupModels[2].clear()
    // 重新加载数据
}

// 3. 检查数据绑定
// 确保ListView正确绑定到ListModel
```

### 调试工具和方法

#### 1. 日志系统
```cpp
// 启用详细日志
QLoggingCategory::setFilterRules("ddlonline.*.debug=true");

// 自定义日志输出
QDebug debug = qDebug();
debug << "WebSocket Status:" << m_connected;
debug << "Database Path:" << m_database.databaseName();
```

#### 2. 网络调试
```bash
# 使用Wireshark抓包分析
# 过滤条件: tcp.port == 8090

# 使用WebSocket测试工具
npm install -g wscat
wscat -c ws://8.148.4.26:8090
```

#### 3. 数据库调试
```sql
-- 查看数据库状态
SELECT count(*) FROM todo_items;
SELECT * FROM todo_items ORDER BY last_modified DESC LIMIT 10;

-- 检查数据完整性
SELECT uuid, COUNT(*) FROM todo_items GROUP BY uuid HAVING COUNT(*) > 1;
```

### 性能监控

#### 1. 内存使用监控
```cpp
// 监控内存使用
#include <QProcess>
void monitorMemory() {
    QProcess process;
    process.start("tasklist", QStringList() << "/FI" << "IMAGENAME eq DDLonline.exe");
    process.waitForFinished();
    qDebug() << process.readAllStandardOutput();
}
```

#### 2. 网络性能监控
```cpp
// 监控网络延迟
QElapsedTimer timer;
timer.start();
sendMessage(testMessage);
qDebug() << "Network latency:" << timer.elapsed() << "ms";
```

## 🛠️ 开发指南

### 开发环境搭建

#### 1. Qt开发环境
```bash
# 下载Qt安装包
wget https://download.qt.io/official_releases/qt/6.8/6.8.1/qt-opensource-windows-x64-6.8.1.exe

# 安装必要组件
- Qt 6.8.1
- Qt Creator 11.0
- MinGW 11.2.0 64-bit
- Qt Quick 3D
- Qt WebSockets
```

#### 2. CMake配置
```cmake
# CMakeLists.txt 关键配置
cmake_minimum_required(VERSION 3.16)

project(DDLonline VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql WebSockets)

qt_standard_project_setup(REQUIRES 6.8)
```

#### 3. 代码规范

##### C++编码规范
```cpp
// 命名规范
class DatabaseManager;        // 类名：PascalCase
void processJsonResult();     // 函数名：camelCase
QString m_database;          // 成员变量：m_前缀
const int MAX_RETRY = 3;     // 常量：UPPER_CASE

// 注释规范
/**
 * @brief 处理JSON操作结果
 * @param jsonResult JSON操作结果
 * @return 处理是否成功
 */
bool processJsonResult(const QVariantMap &jsonResult);
```

##### QML编码规范
```qml
// 属性命名
property bool isModifying: false    // 布尔值：is前缀
property string modifyUUID: ""      // 字符串：camelCase
property int tempYear: 0           // 数字：camelCase

// 信号命名
signal dateSelected(date selectedDate)  // 信号：camelCase
signal operationCompleted(string operation, bool success)

// 函数命名
function refreshData()                 // 函数：camelCase
function parseDate(dateString)         // 参数：camelCase
```

### 添加新功能

#### 1. 添加新的任务属性
```cpp
// 1. 修改数据结构
struct TodoItem {
    // 现有属性...
    int priority;        // 新增：优先级
    QStringList tags;    // 新增：标签
};

// 2. 更新数据库表
QString alterTableSQL = R"(
    ALTER TABLE todo_items ADD COLUMN priority INTEGER DEFAULT 0;
    ALTER TABLE todo_items ADD COLUMN tags TEXT;
)";

// 3. 修改JSON处理
void addNewFieldsToJson(QJsonObject &itemObj, const TodoItem &item) {
    itemObj["priority"] = item.priority;
    itemObj["tags"] = QJsonArray::fromStringList(item.tags);
}
```

#### 2. 添加新的同步类型
```cpp
// 1. 扩展消息类型
enum MessageType {
    FullUpdate,
    Modification,
    Response,
    PriorityUpdate,    // 新增：优先级更新
    TagUpdate        // 新增：标签更新
};

// 2. 添加处理函数
bool DatabaseManager::processPriorityUpdate(const QVariantMap &operationData) {
    QString uuid = operationData["uuid"].toString();
    int priority = operationData["priority"].toInt();
    return updateTaskPriority(uuid, priority);
}
```

### 测试策略

#### 1. 单元测试
```cpp
// 使用Qt Test框架
#include <QtTest>

class TestDatabaseManager : public QObject {
    Q_OBJECT
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    void testInsertTodoItem();
    void testDeleteTodoItem();
    void testSyncData();
};

void TestDatabaseManager::testInsertTodoItem() {
    QVariantMap item;
    item["title"] = "Test Task";
    item["due_date"] = "2025-01-10";
    
    QVERIFY(dbManager->insertTodoItem(item));
    QVariantList items = dbManager->getTodoItems();
    QCOMPARE(items.size(), 1);
}
```

#### 2. 集成测试
```cpp
// 测试WebSocket通信
class TestWebSocketIntegration : public QObject {
    Q_OBJECT
    
private slots:
    void testFullSync();
    void testIncrementalSync();
    void testConflictResolution();
};

void TestWebSocketIntegration::testFullSync() {
    // 模拟客户端连接
    WebSocketClient *client = WebSocketClient::getInstance();
    QSignalSpy spy(client, SIGNAL(messageReceived(QString)));
    
    client->connectToServer("ws://localhost:8090");
    QVERIFY(spy.wait(5000));
    
    // 验证接收到的消息
    QList<QVariant> arguments = spy.takeFirst();
    QString message = arguments.at(0).toString();
    QVERIFY(message.contains("\"type\":\"full_update\""));
}
```


## 🤝 贡献指南

### 贡献流程

#### 1. Fork项目
```bash
# 1. 在GitHub上Fork项目
# 2. 克隆你的Fork
git clone https://github.com/your-username/DDLonline.git
cd DDLonline

# 3. 添加上游仓库
git remote add upstream https://github.com/original-repo/DDLonline.git
```

#### 2. 创建开发分支
```bash
# 同步最新代码
git fetch upstream
git checkout main
git merge upstream/main

# 创建功能分支
git checkout -b feature/your-feature-name
```

#### 3. 开发和测试
```bash
# 进行开发
# ... 编写代码 ...

# 运行测试
mkdir build && cd build
cmake .. && make
./tests/run_all_tests

# 代码格式化
clang-format -i src/*.cpp src/*.h
```

#### 4. 提交代码
```bash
# 添加文件
git add .

# 提交 (遵循约定式提交规范)
git commit -m "feat: add task priority feature

- Add priority field to TodoItem structure
- Update database schema to support priority
- Implement priority-based sorting in UI
- Add unit tests for priority functionality

Closes #123"

# 推送到你的Fork
git push origin feature/your-feature-name
```

#### 5. 创建Pull Request
1. 在GitHub上创建Pull Request
2. 填写详细的PR描述
3. 等待代码审查
4. 根据反馈修改代码
5. 合并到主分支

### 代码审查标准

#### 1. 代码质量检查
- [ ] 代码符合项目编码规范
- [ ] 包含适当的注释和文档
- [ ] 通过所有单元测试
- [ ] 没有内存泄漏或性能问题
- [ ] 错误处理完善

#### 2. 功能性检查
- [ ] 新功能正常工作
- [ ] 不影响现有功能
- [ ] UI/UX符合设计规范
- [ ] 多端同步功能正常
- [ ] 离线功能正常

#### 3. 安全性检查
- [ ] 输入验证充分
- [ ] 没有SQL注入风险
- [ ] 网络通信安全
- [ ] 数据加密存储





**DDLonline** - 让待办事项管理更简单，让多端协作更高效！ 🚀