#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QTableWidgetItem>

#include "../model/taskitem.h"
#include "taskcreatedialog.h"

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
	void onResetFilterClicked();
	void onSortByChanged(int index);
	void onSortOrderClicked();
	void onCategorySelected(QListWidgetItem* item);
	void onItemChanged(QTableWidgetItem* item);
	void onItemDoubleClicked(int row, int column);
	void onCompletedFilterChanged(int index);
	void onExpiredFilterChanged(int index);

   private:
	Ui::MainWindow* ui;	 // UI 指针
	QList<TaskItem> m_todoItems;
	QString m_currentCategoryFilter;
	bool m_isAscending = true;
	int m_completedFilterIndex = 0;	 // 0: 全部, 1: 未完成, 2: 已完成
	int m_expiredFilterIndex = 0;	 // 0: 全部, 1: 未过期, 2: 已过期

	void loadData();
	void refreshTableWidget();
	void refreshCategoryList();
};
#endif	// MAINWINDOW_H
