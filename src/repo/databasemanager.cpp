#include "databasemanager.h"

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

DatabaseManager& DatabaseManager::instance() {
	static DatabaseManager instance;
	return instance;
}

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager() { closeDatabase(); }

bool DatabaseManager::openDatabase() {
	m_db = QSqlDatabase::addDatabase("QSQLITE");

	// Store database in a standard location
	QString path =
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	if (!dir.exists()) {
		dir.mkpath(path);
	}
	QString dbPath = path + "/todo_list.db";
	m_db.setDatabaseName(dbPath);

	if (!m_db.open()) {
		qDebug() << "Error: connection with database failed";
		return false;
	}
	return true;
}

void DatabaseManager::closeDatabase() {
	if (m_db.isOpen()) {
		m_db.close();
	}
}

bool DatabaseManager::initTable() {
	QSqlQuery query;
	QString createTableQuery =
		"CREATE TABLE IF NOT EXISTS todo_items ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"title TEXT NOT NULL, "
		"description TEXT, "
		"completed INTEGER DEFAULT 0)";
	return query.exec(createTableQuery);
}

bool DatabaseManager::addTodo(TodoItem& item) {
	QSqlQuery query;
	query.prepare(
		"INSERT INTO todo_items (title, description, completed) VALUES "
		"(:title, :description, :completed)");
	query.bindValue(":title", item.title);
	query.bindValue(":description", item.description);
	query.bindValue(":completed", item.completed ? 1 : 0);

	if (query.exec()) {
		item.id = query.lastInsertId().toInt();
		return true;
	}
	qDebug() << "Add todo failed: " << query.lastError();
	return false;
}

bool DatabaseManager::removeTodo(int id) {
	QSqlQuery query;
	query.prepare("DELETE FROM todo_items WHERE id = :id");
	query.bindValue(":id", id);
	return query.exec();
}

bool DatabaseManager::updateTodo(const TodoItem& item) {
	QSqlQuery query;
	query.prepare(
		"UPDATE todo_items SET title = :title, description = :description, "
		"completed = :completed WHERE id = :id");
	query.bindValue(":title", item.title);
	query.bindValue(":description", item.description);
	query.bindValue(":completed", item.completed ? 1 : 0);
	query.bindValue(":id", item.id);
	return query.exec();
}

QList<TodoItem> DatabaseManager::getAllTodos() {
	QList<TodoItem> list;
	QSqlQuery query("SELECT id, title, description, completed FROM todo_items");
	while (query.next()) {
		int id = query.value(0).toInt();
		QString title = query.value(1).toString();
		QString description = query.value(2).toString();
		bool completed = query.value(3).toBool();
		list.append(TodoItem(id, title, description, completed));
	}
	return list;
}
