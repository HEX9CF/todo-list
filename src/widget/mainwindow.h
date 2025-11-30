#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>

#include "../model/todoitem.h"

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
	void onAddClicked();
	void onDeleteClicked();
	void onItemChanged(QListWidgetItem* item);

   private:
	Ui::MainWindow* ui;	 // UI 指针
	QList<TodoItem> m_todoItems;

	void loadData();
	void refreshListWidget();
};
#endif	// MAINWINDOW_H
