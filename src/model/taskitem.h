#ifndef TODOITEM_H
#define TODOITEM_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>

class TaskItem {
   public:
	int id;				  // ID
	QString title;		  // 标题
	QString description;  // 描述
	bool completed;		  // 是否完成
	QString category;	  // 分类
	int priority;		  // 优先级 (0: Low, 1: Medium, 2: High)
	QDateTime deadline;	  // 截止日期

	// 构造函数
	TaskItem(int id = -1, const QString& title = "",
			 const QString& description = "", bool completed = false,
			 const QString& category = "", int priority = 0,
			 const QDateTime& deadline = QDateTime());
};

#endif	// TODOITEM_H
