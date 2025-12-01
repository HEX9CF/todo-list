#include "mainwindow.h"

#include <QHeaderView>
#include <QListWidget>
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
	ui->todoTableWidget->setColumnCount(4);
	QStringList headers;
	headers << "完成" << "标题" << "优先级" << "截止日期";
	ui->todoTableWidget->setHorizontalHeaderLabels(headers);

	// Set column resize modes
	QHeaderView* header = ui->todoTableWidget->horizontalHeader();
	header->setSectionResizeMode(0,
								 QHeaderView::ResizeToContents);  // Completed
	header->setSectionResizeMode(1, QHeaderView::Stretch);		  // Title
	header->setSectionResizeMode(2, QHeaderView::ResizeToContents);	 // Priority
	header->setSectionResizeMode(3, QHeaderView::ResizeToContents);	 // Deadline

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
	connect(ui->copyButton, &QPushButton::clicked, this,
			&MainWindow::onCopyClicked);
	connect(ui->deleteButton, &QPushButton::clicked, this,
			&MainWindow::onDeleteClicked);
	connect(ui->searchButton, &QPushButton::clicked, this,
			&MainWindow::onSearchClicked);
	connect(ui->searchLineEdit, &QLineEdit::returnPressed, this,
			&MainWindow::onSearchClicked);
	connect(ui->sortByComboBox,
			QOverload<int>::of(&QComboBox::currentIndexChanged), this,
			&MainWindow::onSortByChanged);
	connect(ui->categoryListWidget, &QListWidget::itemClicked, this,
			&MainWindow::onCategorySelected);
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
			refreshCategoryList();
			refreshTableWidget();
		} else {
			QMessageBox::warning(this, "Error", "Failed to add todo item!");
		}
	}
}

void MainWindow::onCopyClicked() {
	QList<QTableWidgetItem*> selectedItems =
		ui->todoTableWidget->selectedItems();
	QSet<int> rows;
	for (auto item : selectedItems) {
		rows.insert(item->row());
	}

	if (rows.isEmpty()) return;

	QList<int> sortedRows = rows.values();
	std::sort(sortedRows.begin(), sortedRows.end());

	for (int row : sortedRows) {
		QTableWidgetItem* idItem = ui->todoTableWidget->item(row, 0);
		if (!idItem) continue;
		int id = idItem->data(Qt::UserRole).toInt();

		// Find original item
		TodoItem originalItem;
		bool found = false;
		for (const auto& item : m_todoItems) {
			if (item.id == id) {
				originalItem = item;
				found = true;
				break;
			}
		}

		if (found) {
			TodoItem newItem = originalItem;
			newItem.id = -1;  // Reset ID so DB assigns a new one

			if (DatabaseManager::instance().addTodo(newItem)) {
				m_todoItems.append(newItem);
			} else {
				QMessageBox::warning(this, "Error",
									 "Failed to copy todo item!");
			}
		}
	}
	refreshCategoryList();
	refreshTableWidget();
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
		// Map visual row to actual item index
		// Since we might be filtering, we need to find the item in m_todoItems
		// that corresponds to the visual row.
		// However, the simplest way is to use the ID stored in the item data.
		QTableWidgetItem* idItem = ui->todoTableWidget->item(row, 0);
		if (!idItem) continue;

		int id = idItem->data(Qt::UserRole).toInt();

		if (DatabaseManager::instance().removeTodo(id)) {
			// Remove from m_todoItems
			for (int i = 0; i < m_todoItems.size(); ++i) {
				if (m_todoItems[i].id == id) {
					m_todoItems.removeAt(i);
					break;
				}
			}
		} else {
			QMessageBox::warning(this, "Error", "Failed to delete todo item!");
		}
	}

	refreshCategoryList();
	refreshTableWidget();
}

void MainWindow::onSearchClicked() {
	QString searchText = ui->searchLineEdit->text();
	if (searchText.isEmpty()) {
		loadData();
	} else {
		m_todoItems = DatabaseManager::instance().searchTodos(searchText);
		onSortByChanged(ui->sortByComboBox->currentIndex());  // Apply sort
		refreshCategoryList();
		refreshTableWidget();
	}
}

void MainWindow::onSortByChanged(int index) {
	switch (index) {
		case 0:	 // ID
			std::sort(m_todoItems.begin(), m_todoItems.end(),
					  [](const TodoItem& a, const TodoItem& b) {
						  return a.id < b.id;
					  });
			break;
		case 1:	 // Title
			std::sort(m_todoItems.begin(), m_todoItems.end(),
					  [](const TodoItem& a, const TodoItem& b) {
						  return a.title < b.title;
					  });
			break;
		case 2:	 // Priority (High to Low: 2 > 1 > 0)
			std::sort(m_todoItems.begin(), m_todoItems.end(),
					  [](const TodoItem& a, const TodoItem& b) {
						  return a.priority > b.priority;
					  });
			break;
		case 3:	 // Deadline (Earliest first)
			std::sort(m_todoItems.begin(), m_todoItems.end(),
					  [](const TodoItem& a, const TodoItem& b) {
						  if (!a.deadline.isValid())
							  return false;	 // Invalid last
						  if (!b.deadline.isValid()) return true;
						  return a.deadline < b.deadline;
					  });
			break;
	}
	refreshTableWidget();
}

void MainWindow::onCategorySelected(QListWidgetItem* item) {
	m_currentCategoryFilter = item->text();
	if (m_currentCategoryFilter == "所有分类") {
		m_currentCategoryFilter.clear();
	}
	refreshTableWidget();
}

void MainWindow::onItemChanged(QTableWidgetItem* item) {
	// Only handle changes in the "Completed" column (index 0)
	if (item->column() != 0) return;

	int id = item->data(Qt::UserRole).toInt();
	bool completed = (item->checkState() == Qt::Checked);

	// Find item in m_todoItems
	for (auto& todo : m_todoItems) {
		if (todo.id == id) {
			if (todo.completed != completed) {
				todo.completed = completed;
				if (!DatabaseManager::instance().updateTodo(todo)) {
					QMessageBox::warning(this, "Error",
										 "Failed to update todo item!");
				}
			}
			break;
		}
	}
}

void MainWindow::onItemDoubleClicked(int row, int column) {
	QTableWidgetItem* idItem = ui->todoTableWidget->item(row, 0);
	if (!idItem) return;
	int id = idItem->data(Qt::UserRole).toInt();

	TodoItem* targetItem = nullptr;
	for (auto& item : m_todoItems) {
		if (item.id == id) {
			targetItem = &item;
			break;
		}
	}

	if (targetItem) {
		TodoDetailDialog dialog(*targetItem, this);
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
	onSortByChanged(ui->sortByComboBox->currentIndex());  // Apply sort
	refreshCategoryList();
	refreshTableWidget();
}

void MainWindow::refreshCategoryList() {
	QSet<QString> categories;
	for (const auto& item : m_todoItems) {
		if (!item.category.isEmpty()) {
			categories.insert(item.category);
		}
	}

	ui->categoryListWidget->clear();
	ui->categoryListWidget->addItem("所有分类");
	for (const auto& category : categories) {
		ui->categoryListWidget->addItem(category);
	}
}

void MainWindow::refreshTableWidget() {
	// 暂时断开信号，防止在添加 item 时触发 itemChanged
	ui->todoTableWidget->blockSignals(true);

	ui->todoTableWidget->setRowCount(0);  // Clear table

	int row = 0;
	for (const auto& item : m_todoItems) {
		if (!m_currentCategoryFilter.isEmpty() &&
			item.category != m_currentCategoryFilter) {
			continue;
		}

		ui->todoTableWidget->insertRow(row);

		// Column 0: Completed (Checkbox)
		QTableWidgetItem* checkItem = new QTableWidgetItem();
		checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
							Qt::ItemIsSelectable);
		checkItem->setCheckState(item.completed ? Qt::Checked : Qt::Unchecked);
		// Store ID in the first column item for reference if needed, though we
		// use row index
		checkItem->setData(Qt::UserRole, item.id);
		ui->todoTableWidget->setItem(row, 0, checkItem);

		// Column 1: Title
		ui->todoTableWidget->setItem(row, 1, new QTableWidgetItem(item.title));

		// Column 2: Priority
		QString priorityStr =
			(item.priority == 0) ? "低" : (item.priority == 1 ? "中" : "高");
		ui->todoTableWidget->setItem(row, 2, new QTableWidgetItem(priorityStr));

		// Column 3: Deadline
		QString deadlineStr = item.deadline.isValid()
								  ? item.deadline.toString("yyyy-MM-dd HH:mm")
								  : "";
		ui->todoTableWidget->setItem(row, 3, new QTableWidgetItem(deadlineStr));

		row++;
	}

	ui->todoTableWidget->blockSignals(false);
}
