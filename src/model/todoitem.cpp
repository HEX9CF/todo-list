#include "todoitem.h"

// 构造函数实现
TodoItem::TodoItem(int id, const QString& title, const QString& description,
				   bool completed)
	: id(id), title(title), description(description), completed(completed) {}
