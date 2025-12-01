#include "taskitem.h"

// 构造函数实现
TaskItem::TaskItem(int id, const QString& title, const QString& description,
				   bool completed, const QString& category, int priority,
				   const QDateTime& deadline)
	: id(id),
	  title(title),
	  description(description),
	  completed(completed),
	  category(category),
	  priority(priority),
	  deadline(deadline) {}
