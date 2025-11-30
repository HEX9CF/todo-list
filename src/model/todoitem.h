#ifndef TODOITEM_H
#define TODOITEM_H

#include <QJsonObject>
#include <QString>

class TodoItem {
   public:
	int id;				  // ID
	QString title;		  // 标题
	QString description;  // 描述
	bool completed;		  // 是否完成

	// 构造函数
	TodoItem(int id = -1, const QString& title = "",
			 const QString& description = "", bool completed = false);
};

#endif	// TODOITEM_H
