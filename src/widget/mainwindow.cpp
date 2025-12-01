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

	// 设置水平布局的拉伸因子 (1:3)
	ui->horizontalLayout->setStretch(0, 1);
	ui->horizontalLayout->setStretch(1, 3);

	// 设置表格
	ui->todoTableWidget->setColumnCount(4);
	QStringList headers;
	headers << "完成" << "标题" << "优先级" << "截止日期";
	ui->todoTableWidget->setHorizontalHeaderLabels(headers);

	// 设置列调整模式
	QHeaderView* header = ui->todoTableWidget->horizontalHeader();
	header->setSectionResizeMode(0,
								 QHeaderView::ResizeToContents);	 // 完成
	header->setSectionResizeMode(1, QHeaderView::Stretch);			 // 标题
	header->setSectionResizeMode(2, QHeaderView::ResizeToContents);	 // 优先级
	header->setSectionResizeMode(3, QHeaderView::ResizeToContents);	 // 截止日期

	ui->todoTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->todoTableWidget->setEditTriggers(
		QAbstractItemView::NoEditTriggers);	 // 禁用编辑

	// 初始化数据库
	if (DatabaseManager::instance().openDatabase()) {
		DatabaseManager::instance().initTable();
	} else {
		QMessageBox::critical(this, "错误", "打开数据库失败！");
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
	connect(ui->sortOrderButton, &QPushButton::clicked, this,
			&MainWindow::onSortOrderClicked);
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
			QMessageBox::warning(this, "错误", "添加待办事项失败！");
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

		// 查找原始项目
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
			newItem.id = -1;  // 重置 ID，以便数据库分配新 ID

			if (DatabaseManager::instance().addTodo(newItem)) {
				m_todoItems.append(newItem);
			} else {
				QMessageBox::warning(this, "错误", "复制待办事项失败！");
			}
		}
	}
	refreshCategoryList();
	refreshTableWidget();
}

void MainWindow::onDeleteClicked() {
	// 获取选中的行
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
		// 将可视行映射到实际项目索引
		// 由于可能正在过滤，我们需要在 m_todoItems 中找到该项目
		// 对应于可视行。
		// 但是，最简单的方法是使用存储在项目数据中的 ID。
		QTableWidgetItem* idItem = ui->todoTableWidget->item(row, 0);
		if (!idItem) continue;

		int id = idItem->data(Qt::UserRole).toInt();

		if (DatabaseManager::instance().removeTodo(id)) {
			// 从 m_todoItems 中移除
			for (int i = 0; i < m_todoItems.size(); ++i) {
				if (m_todoItems[i].id == id) {
					m_todoItems.removeAt(i);
					break;
				}
			}
		} else {
			QMessageBox::warning(this, "错误", "删除待办事项失败！");
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
		onSortByChanged(ui->sortByComboBox->currentIndex());  // 应用排序
		refreshCategoryList();
		refreshTableWidget();
	}
}

void MainWindow::onSortByChanged(int index) {
	switch (index) {
		case 0:	 // ID
			std::sort(m_todoItems.begin(), m_todoItems.end(),
					  [this](const TodoItem& a, const TodoItem& b) {
						  return m_isAscending ? (a.id < b.id) : (a.id > b.id);
					  });
			break;
		case 1:	 // 标题
			std::sort(m_todoItems.begin(), m_todoItems.end(),
					  [this](const TodoItem& a, const TodoItem& b) {
						  return m_isAscending ? (a.title < b.title)
											   : (a.title > b.title);
					  });
			break;
		case 2:	 // 优先级 (从高到低: 2 > 1 > 0)
			std::sort(m_todoItems.begin(), m_todoItems.end(),
					  [this](const TodoItem& a, const TodoItem& b) {
						  // 优先级逻辑通常是 高 > 低，所以
						  // “升序”可能意味着 0->2 或 2->0，取决于
						  // 解释。让我们假设升序意味着 0
						  // (低) -> 2 (高)
						  return m_isAscending ? (a.priority < b.priority)
											   : (a.priority > b.priority);
					  });
			break;
		case 3:	 // 截止日期 (最早的在前)
			std::sort(m_todoItems.begin(), m_todoItems.end(),
					  [this](const TodoItem& a, const TodoItem& b) {
						  if (!a.deadline.isValid())
							  return false;	 // 无效的排在最后
						  if (!b.deadline.isValid()) return true;
						  return m_isAscending ? (a.deadline < b.deadline)
											   : (a.deadline > b.deadline);
					  });
			break;
	}
	refreshTableWidget();
}

void MainWindow::onSortOrderClicked() {
	m_isAscending = !m_isAscending;
	ui->sortOrderButton->setText(m_isAscending ? "↑" : "↓");
	onSortByChanged(ui->sortByComboBox->currentIndex());
}

void MainWindow::onCategorySelected(QListWidgetItem* item) {
	m_currentCategoryFilter = item->text();
	if (m_currentCategoryFilter == "所有分类") {
		m_currentCategoryFilter.clear();
	}
	refreshTableWidget();
}

void MainWindow::onItemChanged(QTableWidgetItem* item) {
	// 仅处理“已完成”列（索引 0）的更改
	if (item->column() != 0) return;

	int id = item->data(Qt::UserRole).toInt();
	bool completed = (item->checkState() == Qt::Checked);

	// 在 m_todoItems 中查找项目
	for (auto& todo : m_todoItems) {
		if (todo.id == id) {
			if (todo.completed != completed) {
				todo.completed = completed;
				if (!DatabaseManager::instance().updateTodo(todo)) {
					QMessageBox::warning(this, "错误", "更新待办事项失败！");
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
				loadData();	 // 刷新所有数据
			} else {
				QMessageBox::warning(this, "错误", "更新待办事项失败！");
			}
		}
	}
}

void MainWindow::loadData() {
	m_todoItems = DatabaseManager::instance().getAllTodos();
	onSortByChanged(ui->sortByComboBox->currentIndex());  // 应用排序
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

	ui->todoTableWidget->setRowCount(0);  // 清空表格

	int row = 0;
	for (const auto& item : m_todoItems) {
		if (!m_currentCategoryFilter.isEmpty() &&
			item.category != m_currentCategoryFilter) {
			continue;
		}

		ui->todoTableWidget->insertRow(row);

		// 第 0 列：已完成（复选框）
		QTableWidgetItem* checkItem = new QTableWidgetItem();
		checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
							Qt::ItemIsSelectable);
		checkItem->setCheckState(item.completed ? Qt::Checked : Qt::Unchecked);
		// 将 ID 存储在第一列项目中以备参考，尽管我们使用行索引
		checkItem->setData(Qt::UserRole, item.id);
		ui->todoTableWidget->setItem(row, 0, checkItem);

		// 第 1 列：标题
		ui->todoTableWidget->setItem(row, 1, new QTableWidgetItem(item.title));

		// 第 2 列：优先级
		QString priorityStr =
			(item.priority == 0) ? "低" : (item.priority == 1 ? "中" : "高");
		ui->todoTableWidget->setItem(row, 2, new QTableWidgetItem(priorityStr));

		// 第 3 列：截止日期
		QString deadlineStr = item.deadline.isValid()
								  ? item.deadline.toString("yyyy-MM-dd HH:mm")
								  : "";
		ui->todoTableWidget->setItem(row, 3, new QTableWidgetItem(deadlineStr));

		row++;
	}

	ui->todoTableWidget->blockSignals(false);
}
