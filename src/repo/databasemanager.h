#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QList>
#include <QSqlDatabase>

#include "../model/taskitem.h"

class DatabaseManager {
   public:
	static DatabaseManager& instance();
	bool openDatabase();
	void closeDatabase();
	bool initTable();

	bool addTask(TaskItem& item);
	bool removeTask(int id);
	bool updateTask(const TaskItem& item);
	QList<TaskItem> getAllTasks();
	QList<TaskItem> searchTasks(const QString& query);

   private:
	DatabaseManager();
	~DatabaseManager();
	QSqlDatabase m_db;
};

#endif	// DATABASEMANAGER_H
