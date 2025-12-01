#include "mainwindow.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTableWidget>

#include "../repo/databasemanager.h"
#include "todocreatedialog.h"
#include "tododetaildialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);	// 设置 UI 界面

	// Set stretch factors for horizontal layout (1:3)
	ui->horizontalLayout->setStretch(0, 1);
	ui->horizontalLayout->setStretch(1, 3);

	// Setup Table
	ui->todoTableWidget->setColumnCount(5);
	QStringList headers;
	headers << "完成" << "标题" << "分类" << "优先级" << "截止日期";
	ui->todoTableWidget->setHorizontalHeaderLabels(headers);
	ui->todoTableWidget->horizontalHeader()->setSectionResizeMode(
		QHeaderView::Stretch);
	ui->todoTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->todoTableWidget->setEditTriggers(
		QAbstractItemView::NoEditTriggers);	 // Disable editing

	// Initialize Database
	if (DatabaseManager::instance().openDatabase()) {
		DatabaseManager::instance().initTable();
	} else {
		QMessageBox::critical(this, "Error", "Failed to open database!");
	}

	// 连接信号和槽
	connect(ui->createButton, &QPushButton::clicked, this,
			&MainWindow::onCreateClicked);
	connect(ui->deleteButton, &QPushButton::clicked, this,
			&MainWindow::onDeleteClicked);
	connect(ui->searchButton, &QPushButton::clicked, this,
			&MainWindow::onSearchClicked);
	connect(ui->searchLineEdit, &QLineEdit::returnPressed, this,
			&MainWindow::onSearchClicked);
	connect(ui->todoTableWidget, &QTableWidget::itemChanged, this,
			&MainWindow::onItemChanged);
	connect(ui->todoTableWidget, &QTableWidget::cellDoubleClicked, this,
			&MainWindow::onItemDoubleClicked);

	// 加载数据
	loadData();
}

MainWindow::~MainWindow() {
	delete ui;	// 释放 UI 内存
}

void MainWindow::onCreateClicked() {
	TodoCreateDialog dialog(this);
	if (dialog.exec() == QDialog::Accepted) {
		TodoItem newItem = dialog.getTodoItem();
		if (DatabaseManager::instance().addTodo(newItem)) {
			m_todoItems.append(newItem);
			refreshTableWidget();
		} else {
			QMessageBox::warning(this, "Error", "Failed to add todo item!");
		}
	}
}

void MainWindow::onDeleteClicked() {
	// Get selected rows
	QList<QTableWidgetItem*> selectedItems =
		ui->todoTableWidget->selectedItems();
	QSet<int> rows;
	for (auto item : selectedItems) {
		rows.insert(item->row());
	}

	QList<int> sortedRows = rows.values();
	std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

	if (sortedRows.isEmpty()) return;

	for (int row : sortedRows) {
		if (row >= 0 && row < m_todoItems.size()) {
			int id = m_todoItems[row].id;
			if (DatabaseManager::instance().removeTodo(id)) {
				m_todoItems.removeAt(row);
			} else {
				QMessageBox::warning(this, "Error",
									 "Failed to delete todo item!");
			}
		}
	}

	refreshTableWidget();
}

void MainWindow::onSearchClicked() {
	QString searchText = ui->searchLineEdit->text();
	if (searchText.isEmpty()) {
		loadData();
	} else {
		m_todoItems = DatabaseManager::instance().searchTodos(searchText);
		refreshTableWidget();
	}
}

void MainWindow::onItemChanged(QTableWidgetItem* item) {
	// Only handle changes in the "Completed" column (index 0)
	if (item->column() != 0) return;

	int row = item->row();
	if (row >= 0 && row < m_todoItems.size()) {
		bool completed = (item->checkState() == Qt::Checked);
		if (m_todoItems[row].completed != completed) {
			m_todoItems[row].completed = completed;
			if (!DatabaseManager::instance().updateTodo(m_todoItems[row])) {
				QMessageBox::warning(this, "Error",
									 "Failed to update todo item!");
			}
		}
	}
}

void MainWindow::onItemDoubleClicked(int row, int column) {
	if (row >= 0 && row < m_todoItems.size()) {
		TodoDetailDialog dialog(m_todoItems[row], this);
		if (dialog.exec() == QDialog::Accepted) {
			TodoItem updatedItem = dialog.getTodoItem();
			if (DatabaseManager::instance().updateTodo(updatedItem)) {
				loadData();	 // Refresh all data
			} else {
				QMessageBox::warning(this, "Error",
									 "Failed to update todo item!");
			}
		}
	}
}

void MainWindow::loadData() {
	m_todoItems = DatabaseManager::instance().getAllTodos();
	refreshTableWidget();
}

void MainWindow::refreshTableWidget() {
	// 暂时断开信号，防止在添加 item 时触发 itemChanged
	ui->todoTableWidget->blockSignals(true);

	ui->todoTableWidget->setRowCount(0);  // Clear table

	for (int i = 0; i < m_todoItems.size(); ++i) {
		const auto& item = m_todoItems[i];
		ui->todoTableWidget->insertRow(i);

		// Column 0: Completed (Checkbox)
		QTableWidgetItem* checkItem = new QTableWidgetItem();
		checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
							Qt::ItemIsSelectable);
		checkItem->setCheckState(item.completed ? Qt::Checked : Qt::Unchecked);
		// Store ID in the first column item for reference if needed, though we
		// use row index
		checkItem->setData(Qt::UserRole, item.id);
		ui->todoTableWidget->setItem(i, 0, checkItem);

		// Column 1: Title
		ui->todoTableWidget->setItem(i, 1, new QTableWidgetItem(item.title));

		// Column 2: Category
		ui->todoTableWidget->setItem(i, 2, new QTableWidgetItem(item.category));

		// Column 3: Priority
		QString priorityStr =
			(item.priority == 0) ? "低" : (item.priority == 1 ? "中" : "高");
		ui->todoTableWidget->setItem(i, 3, new QTableWidgetItem(priorityStr));

		// Column 4: Deadline
		QString deadlineStr = item.deadline.isValid()
								  ? item.deadline.toString("yyyy-MM-dd HH:mm")
								  : "";
		ui->todoTableWidget->setItem(i, 4, new QTableWidgetItem(deadlineStr));
	}

	ui->todoTableWidget->blockSignals(false);
}
