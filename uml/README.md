# DDLKiller UML 图表

本目录包含 DDLKiller 项目的 PlantUML 图表，已拆分为多个独立文件。

## 图表文件

### 类图 (Class Diagrams)

1. **01_data_model.puml** - 数据模型结构
   - TodoItem 结构体及所有字段
   - 显示离线同步标志

2. **02_database_manager.puml** - DatabaseManager 类
   - 单例模式实现
   - 所有公共和私有方法
   - 数据库操作和同步逻辑

3. **03_websocket_client.puml** - WebSocketClient 类
   - 单例模式实现
   - 连接管理
   - 消息处理

4. **04_json_processing.puml** - JSON 处理类
   - JsonProcessor 用于解析传入的 JSON
   - JsonGenerator 用于创建 JSON 消息

5. **05_utility_classes.puml** - 工具类
   - Date 类用于日期格式化和分组
   - DataSql 类用于 SQL 查询执行

6. **06_qml_components.puml** - QML 组件
   - Main.qml 主 UI 组件
   - CalendarSelector 组件
   - MaskCalender 组件

### 架构图 (Architecture Diagram)

7. **07_architecture.puml** - 系统架构
   - 分层架构视图
   - 组件关系
   - 层间数据流

### 时序图 (Sequence Diagrams)

8. **08_sequence_add.puml** - 添加任务流程
   - 用户添加新任务
   - JSON 生成和发送
   - 在线/离线处理

9. **09_sequence_sync.puml** - 全量同步流程
   - 全量更新请求流程
   - 服务器数据接收
   - 向服务器发送响应

10. **10_sequence_receive.puml** - 接收服务器消息流程
    - 处理不同消息类型
    - modification, full_update_blanket, full_update_mix, response

11. **11_initialization.puml** - 应用初始化流程
    - 应用启动序列
    - 单例初始化
    - QML 上下文设置
    - 自动连接服务器

## 使用方法

### 渲染图表

可以使用以下方式渲染这些图表：

1. **在线工具**
   - 访问 [PlantText](https://www.planttext.com/)
   - 复制并粘贴任何 .puml 文件的内容
   - 下载为 PNG、SVG 或其他格式

2. **VS Code 扩展**
   - 安装 "PlantUML" 扩展
   - 打开任何 .puml 文件
   - 按 Alt+D 预览
   - 导出为所需格式

3. **命令行**
   ```bash
   plantuml diagram.puml
   ```

### 插入 Word 文档

1. 将每个图表渲染为 PNG 或 SVG 格式
2. 将图片插入 Word 文档
3. 每个图表都是独立的，大小适中

## 图表概述

DDLKiller 应用程序是一个基于 Qt/QML 的待办事项管理器，具有以下特性：

- **SQLite 数据库**: 支持离线的本地数据存储
- **WebSocket 通信**: 与服务器实时同步
- **JSON 协议**: 数据交换的消息格式
- **QML UI**: 现代、响应式的用户界面
- **离线/在线同步**: 自动冲突解决

### 主要功能

- 三个任务分组：三天内、两周内、长期
- 添加、修改、删除操作
- 与服务器全量同步
- 离线模式支持
- 自动冲突解决
- 日历集成
