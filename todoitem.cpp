#include "todoitem.h"

TodoItem::TodoItem(const QString& title, const QString& description,
				   bool completed)
	: title(title), description(description), completed(completed) {}

QJsonObject TodoItem::toJson() const {
	QJsonObject json;
	json["title"] = title;
	json["description"] = description;
	json["completed"] = completed;
	return json;
}

TodoItem TodoItem::fromJson(const QJsonObject& json) {
	return TodoItem(json["title"].toString(), json["description"].toString(),
					json["completed"].toBool());
}
