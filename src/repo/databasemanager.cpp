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

	// 将数据库存储在标准位置
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
		"completed INTEGER DEFAULT 0, "
		"category TEXT, "
		"priority INTEGER DEFAULT 0, "
		"deadline TEXT)";
	return query.exec(createTableQuery);
}

bool DatabaseManager::addTask(TaskItem& item) {
	QSqlQuery query;
	query.prepare(
		"INSERT INTO todo_items (title, description, completed, category, "
		"priority, deadline) VALUES "
		"(:title, :description, :completed, :category, :priority, :deadline)");
	query.bindValue(":title", item.title);
	query.bindValue(":description", item.description);
	query.bindValue(":completed", item.completed ? 1 : 0);
	query.bindValue(":category", item.category);
	query.bindValue(":priority", item.priority);
	query.bindValue(":deadline", item.deadline.isValid()
									 ? item.deadline.toString(Qt::ISODate)
									 : QVariant(QVariant::String));

	if (query.exec()) {
		item.id = query.lastInsertId().toInt();
		return true;
	}
	qDebug() << "Add todo failed: " << query.lastError();
	return false;
}

bool DatabaseManager::removeTask(int id) {
	QSqlQuery query;
	query.prepare("DELETE FROM todo_items WHERE id = :id");
	query.bindValue(":id", id);
	return query.exec();
}

bool DatabaseManager::updateTask(const TaskItem& item) {
	QSqlQuery query;
	query.prepare(
		"UPDATE todo_items SET title = :title, description = :description, "
		"completed = :completed, category = :category, priority = :priority, "
		"deadline = :deadline WHERE id = :id");
	query.bindValue(":title", item.title);
	query.bindValue(":description", item.description);
	query.bindValue(":completed", item.completed ? 1 : 0);
	query.bindValue(":category", item.category);
	query.bindValue(":priority", item.priority);
	query.bindValue(":deadline", item.deadline.isValid()
									 ? item.deadline.toString(Qt::ISODate)
									 : QVariant(QVariant::String));
	query.bindValue(":id", item.id);
	return query.exec();
}

QList<TaskItem> DatabaseManager::getAllTasks() {
	QList<TaskItem> list;
	QSqlQuery query(
		"SELECT id, title, description, completed, category, priority, "
		"deadline FROM todo_items");
	while (query.next()) {
		int id = query.value(0).toInt();
		QString title = query.value(1).toString();
		QString description = query.value(2).toString();
		bool completed = query.value(3).toBool();
		QString category = query.value(4).toString();
		int priority = query.value(5).toInt();
		QDateTime deadline =
			QDateTime::fromString(query.value(6).toString(), Qt::ISODate);
		list.append(TaskItem(id, title, description, completed, category,
							 priority, deadline));
	}
	return list;
}

QList<TaskItem> DatabaseManager::searchTasks(const QString& searchText) {
	QList<TaskItem> list;
	QSqlQuery query;
	query.prepare(
		"SELECT id, title, description, completed, category, priority, "
		"deadline FROM todo_items WHERE title LIKE :search OR description LIKE "
		":search");
	query.bindValue(":search", "%" + searchText + "%");
	query.exec();

	while (query.next()) {
		int id = query.value(0).toInt();
		QString title = query.value(1).toString();
		QString description = query.value(2).toString();
		bool completed = query.value(3).toBool();
		QString category = query.value(4).toString();
		int priority = query.value(5).toInt();
		QDateTime deadline =
			QDateTime::fromString(query.value(6).toString(), Qt::ISODate);
		list.append(TaskItem(id, title, description, completed, category,
							 priority, deadline));
	}
	return list;
}
