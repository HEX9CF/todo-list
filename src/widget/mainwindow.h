#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QTableWidgetItem>

#include "../model/todoitem.h"
#include "todocreatedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

   public:
	MainWindow(QWidget* parent = nullptr);	// 构造函数
	~MainWindow();							// 析构函数

   private slots:
	void onCreateClicked();
	void onCopyClicked();
	void onDeleteClicked();
	void onSearchClicked();
	void onSortByChanged(int index);
	void onCategorySelected(QListWidgetItem* item);
	void onItemChanged(QTableWidgetItem* item);
	void onItemDoubleClicked(int row, int column);

   private:
	Ui::MainWindow* ui;	 // UI 指针
	QList<TodoItem> m_todoItems;
	QString m_currentCategoryFilter;

	void loadData();
	void refreshTableWidget();
	void refreshCategoryList();
};
#endif	// MAINWINDOW_H
