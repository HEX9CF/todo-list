#ifndef TODOITEM_H
#define TODOITEM_H

#include <QJsonObject>
#include <QString>

class TodoItem {
   public:
	QString title;
	QString description;
	bool completed;

	TodoItem(const QString& title = "", const QString& description = "",
			 bool completed = false);

	// Serialization helpers
	QJsonObject toJson() const;
	static TodoItem fromJson(const QJsonObject& json);
};

#endif	// TODOITEM_H
