#ifndef TODOITEM_H
#define TODOITEM_H

#include <QJsonObject>
#include <QString>

class TodoItem {
   public:
	QString title;       // 待办事项标题
	QString description; // 待办事项描述
	bool completed;      // 是否完成

	// 构造函数
	TodoItem(const QString& title = "", const QString& description = "",
			 bool completed = false);

	// 序列化辅助函数
	QJsonObject toJson() const; // 转换为 JSON 对象
	static TodoItem fromJson(const QJsonObject& json); // 从 JSON 对象创建 TodoItem
};

#endif	// TODOITEM_H
