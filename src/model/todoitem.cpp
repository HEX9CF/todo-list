#include "todoitem.h"

// 构造函数实现
TodoItem::TodoItem(const QString& title, const QString& description,
				   bool completed)
	: title(title), description(description), completed(completed) {}

// 将 TodoItem 转换为 JSON 对象
QJsonObject TodoItem::toJson() const {
	QJsonObject json;
	json["title"] = title;
	json["description"] = description;
	json["completed"] = completed;
	return json;
}

// 从 JSON 对象创建 TodoItem
TodoItem TodoItem::fromJson(const QJsonObject& json) {
	return TodoItem(json["title"].toString(), json["description"].toString(),
					json["completed"].toBool());
}
