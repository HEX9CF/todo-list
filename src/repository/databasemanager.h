#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QList>
#include <QSqlDatabase>

#include "../model/todoitem.h"

class DatabaseManager {
   public:
	static DatabaseManager& instance();
	bool openDatabase();
	void closeDatabase();
	bool initTable();

	bool addTodo(TodoItem& item);
	bool removeTodo(int id);
	bool updateTodo(const TodoItem& item);
	QList<TodoItem> getAllTodos();

   private:
	DatabaseManager();
	~DatabaseManager();
	QSqlDatabase m_db;
};

#endif	// DATABASEMANAGER_H
