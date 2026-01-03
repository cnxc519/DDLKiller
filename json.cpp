#include "json.h"
#include "database.h"

JsonProcessor::JsonProcessor(QObject *parent)
    : QObject(parent)
{}

QVariantMap JsonProcessor::parseAndProcessJson(const QString &jsonData)
{
    QVariantMap result;
    result["success"] = false;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        result["error"] = "JSON parse error: " + parseError.errorString();
        emit jsonError(result["error"].toString());
        return result;
    }

    if (!doc.isObject()) {
        result["error"] = "JSON document is not an object";
        emit jsonError(result["error"].toString());
        return result;
    }

    QJsonObject rootObject = doc.object();

    // 检查类型字段
    if (!rootObject.contains("type")) {
        result["error"] = "Missing 'type' field in JSON";
        emit jsonError(result["error"].toString());
        return result;
    }

    QString type = rootObject["type"].toString();
    result["type"] = type;

    if (type == "full_update") {
        if (!rootObject.contains("content") || !rootObject["content"].isArray()) {
            result["error"] = "Invalid or missing 'content' array for full_update";
            emit jsonError(result["error"].toString());
            return result;
        }

        QVariantList items = parseFullUpdateContent(rootObject["content"].toArray());
        if (items.isEmpty()) {
            result["error"] = "No valid items found in full_update content";
            emit jsonError(result["error"].toString());
            return result;
        }

        result["success"] = true;
        result["operation"] = "full_update";
        result["items"] = items;
        result["message"] = QString("Parsed %1 items for full update").arg(items.size());
    } else if (type == "full_update_noresponse") {
        if (!rootObject.contains("content") || !rootObject["content"].isArray()) {
            result["error"] = "Invalid or missing 'content' array for full_update";
            emit jsonError(result["error"].toString());
            return result;
        }

        QVariantList items = parseFullUpdateContent(rootObject["content"].toArray());
        if (items.isEmpty()) {
            result["error"] = "No valid items found in full_update content";
            emit jsonError(result["error"].toString());
            return result;
        }

        result["success"] = true;
        result["operation"] = "full_update_noresponse";
        result["items"] = items;
        result["message"] = QString("Parsed %1 items for full update").arg(items.size());
    } else if (type == "modification") {
        if (!rootObject.contains("content") || !rootObject["content"].isObject()) {
            result["error"] = "Invalid or missing 'content' object for modification";
            emit jsonError(result["error"].toString());
            return result;
        }

        QVariantMap modificationResult = parseModificationContent(rootObject["content"].toObject());
        if (!modificationResult.contains("operation")) {
            result["error"] = modificationResult["error"].toString();
            emit jsonError(result["error"].toString());
            return result;
        }

        result["success"] = true;
        result["operation"] = modificationResult["operation"];
        result["data"] = modificationResult;
        result["message"] = QString("Parsed %1 operation")
                                .arg(modificationResult["operation"].toString());
    } else if (type == "sync") {
        // 处理同步数据
        result["success"] = true;
        result["operation"] = "sync";
        result["data"] = rootObject.toVariantMap();
        result["message"] = "Sync data received";
    } else if (type == "response") {
        if (!rootObject.contains("content") || !rootObject["content"].isObject()) {
            result["error"] = "Invalid or missing 'content' object for modification";
            emit jsonError(result["error"].toString());
            return result;
        }
        if (!rootObject.value("content").toObject().contains("operation")
            || rootObject.value("content").toObject()["operation"].isObject()) {
            return result;
        }
        QJsonValue value = rootObject.value("content").toObject().value("operation");
        QString opr = value.toString();
        if (opr == "add") {
            QString uuid = rootObject.value("content").toObject().value("operation").toString();

            //将这个uuid的offline_add设为false
            DatabaseManager::getInstance()->markOnlineAdd(uuid);
        }
        if (opr == "delete") {
            QString uuid = rootObject.value("content").toObject().value("operation").toString();

            //将这个uuid执行真删除
            DatabaseManager::getInstance()->markOnlineDelete(uuid);
        }
    } else {
        result["error"] = "Unknown type: " + type;
        emit jsonError(result["error"].toString());
        return result;
    }

    emit jsonProcessed(result["operation"].toString(), true, result["message"].toString());
    return result;
}

QVariantList JsonProcessor::parseFullUpdateContent(const QJsonArray &contentArray)
{
    QVariantList items;

    for (const QJsonValue &value : contentArray) {
        if (value.isObject()) {
            QJsonObject itemObj = value.toObject();
            QVariantMap item;

            if (itemObj.contains("uuid"))
                item["uuid"] = itemObj["uuid"].toString();
            if (itemObj.contains("last_modified"))
                item["last_modified"] = itemObj["last_modified"].toString();
            if (itemObj.contains("title"))
                item["title"] = itemObj["title"].toString();
            if (itemObj.contains("description"))
                item["description"] = itemObj["description"].toString();
            if (itemObj.contains("due_date"))
                item["due_date"] = itemObj["due_date"].toString();
            if (itemObj.contains("complete_flag"))
                item["complete_flag"] = itemObj["complete_flag"].toBool();

            // 确保必需字段存在
            if (item.contains("uuid") && item.contains("title")) {
                items.append(item);
            }
        }
    }

    return items;
}

QVariantMap JsonProcessor::parseModificationContent(const QJsonObject &contentObject)
{
    QVariantMap result;

    if (!contentObject.contains("operation")) {
        result["error"] = "Missing 'operation' field in modification content";
        return result;
    }

    QString operation = contentObject["operation"].toString();
    result["operation"] = operation;

    if (operation == "add") {
        if (!contentObject.contains("uuid")) {
            result["error"] = "Missing 'uuid' field for add operation";
            return result;
        }

        QVariantMap item;
        if (contentObject.contains("uuid"))
            item["uuid"] = contentObject["uuid"].toString();
        if (contentObject.contains("last_modified"))
            item["last_modified"] = contentObject["last_modified"].toString();
        if (contentObject.contains("title"))
            item["title"] = contentObject["title"].toString();
        if (contentObject.contains("description"))
            item["description"] = contentObject["description"].toString();
        if (contentObject.contains("due_date"))
            item["due_date"] = contentObject["due_date"].toString();
        if (contentObject.contains("complete_flag"))
            item["complete_flag"] = contentObject["complete_flag"].toBool();

        result["item"] = item;
    } else if (operation == "delete") {
        if (!contentObject.contains("target_uuid")) {
            result["error"] = "Missing 'target_uuid' field for delete operation";
            return result;
        }
        result["target_uuid"] = contentObject["target_uuid"].toString();
    } else if (operation == "modify") {
        if (!contentObject.contains("target_uuid")) {
            result["error"] = "Missing 'target_uuid' field for modify operation";
            return result;
        }

        result["target_uuid"] = contentObject["target_uuid"].toString();

        QVariantMap item;
        if (contentObject.contains("last_modified"))
            item["last_modified"] = contentObject["last_modified"].toString();
        if (contentObject.contains("title"))
            item["title"] = contentObject["title"].toString();
        if (contentObject.contains("description"))
            item["description"] = contentObject["description"].toString();
        if (contentObject.contains("due_date"))
            item["due_date"] = contentObject["due_date"].toString();
        if (contentObject.contains("complete_flag"))
            item["complete_flag"] = contentObject["complete_flag"].toBool();

        result["item"] = item;
    } else {
        result["error"] = "Unknown operation: " + operation;
        return result;
    }

    return result;
}

QString JsonProcessor::generateFullUpdateJson(const QVariantList &items)
{
    QJsonObject rootObject;
    rootObject["type"] = "full_update";

    QJsonArray contentArray;
    for (const QVariant &itemVar : items) {
        QVariantMap itemMap = itemVar.toMap();
        QJsonObject itemObj;

        itemObj["uuid"] = itemMap["uuid"].toString();
        itemObj["last_modified"] = itemMap["last_modified"].toString();
        itemObj["title"] = itemMap["title"].toString();
        itemObj["description"] = itemMap["description"].toString();
        itemObj["due_date"] = itemMap["due_date"].toString();
        itemObj["complete_flag"] = itemMap["complete_flag"].toBool();

        contentArray.append(itemObj);
    }

    rootObject["content"] = contentArray;

    QJsonDocument doc(rootObject);
    return doc.toJson(QJsonDocument::Indented);
}

QString JsonProcessor::generateSyncJson(const QVariantMap &syncData)
{
    QJsonObject rootObject;
    rootObject["type"] = "sync";
    rootObject["timestamp"] = QString::number(QDateTime::currentMSecsSinceEpoch());

    // 添加新增项目
    QJsonArray additionsArray;
    QVariantList additions = syncData["additions"].toList();
    for (const QVariant &itemVar : additions) {
        QVariantMap itemMap = itemVar.toMap();
        QJsonObject itemObj;

        itemObj["uuid"] = itemMap["uuid"].toString();
        itemObj["last_modified"] = itemMap["last_modified"].toString();
        itemObj["title"] = itemMap["title"].toString();
        itemObj["description"] = itemMap["description"].toString();
        itemObj["due_date"] = itemMap["due_date"].toString();
        itemObj["complete_flag"] = itemMap["complete_flag"].toBool();

        additionsArray.append(itemObj);
    }
    rootObject["additions"] = additionsArray;

    // 添加删除项目UUID列表
    QJsonArray deletionsArray;
    QVariantList deletions = syncData["deletions"].toList();
    for (const QVariant &uuidVar : deletions) {
        deletionsArray.append(uuidVar.toString());
    }
    rootObject["deletions"] = deletionsArray;

    QJsonDocument doc(rootObject);
    return doc.toJson(QJsonDocument::Indented);
}
