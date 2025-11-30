#include "mainwindow.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);	// 设置 UI 界面

	// 连接信号和槽
	connect(ui->addButton, &QPushButton::clicked, this,
			&MainWindow::onAddClicked);
	connect(ui->deleteButton, &QPushButton::clicked, this,
			&MainWindow::onDeleteClicked);
	connect(ui->todoListWidget, &QListWidget::itemChanged, this,
			&MainWindow::onItemChanged);

	// 加载数据
	loadData();
}

MainWindow::~MainWindow() {
	delete ui;	// 释放 UI 内存
}

void MainWindow::onAddClicked() {
	QString title = ui->titleInput->text().trimmed();
	QString description = ui->descriptionInput->text().trimmed();

	if (title.isEmpty()) {
		QMessageBox::warning(this, "警告", "标题不能为空！");
		return;
	}

	TodoItem newItem(title, description, false);
	m_todoItems.append(newItem);

	ui->titleInput->clear();
	ui->descriptionInput->clear();

	refreshListWidget();
	saveData();
}

void MainWindow::onDeleteClicked() {
	QList<QListWidgetItem*> selectedItems = ui->todoListWidget->selectedItems();
	if (selectedItems.isEmpty()) return;

	// 从后往前删除，避免索引问题
	QList<int> rows;
	for (auto item : selectedItems) {
		rows.append(ui->todoListWidget->row(item));
	}
	std::sort(rows.begin(), rows.end(), std::greater<int>());

	for (int row : rows) {
		if (row >= 0 && row < m_todoItems.size()) {
			m_todoItems.removeAt(row);
		}
	}

	refreshListWidget();
	saveData();
}

void MainWindow::onItemChanged(QListWidgetItem* item) {
	int row = ui->todoListWidget->row(item);
	if (row >= 0 && row < m_todoItems.size()) {
		bool completed = (item->checkState() == Qt::Checked);
		m_todoItems[row].completed = completed;
		saveData();
	}
}

void MainWindow::loadData() {
	QFile file(m_filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return;	 // 文件不存在或无法打开，可能是第一次运行
	}

	QByteArray data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonArray array = doc.array();

	m_todoItems.clear();
	for (const auto& value : array) {
		m_todoItems.append(TodoItem::fromJson(value.toObject()));
	}

	refreshListWidget();
}

void MainWindow::saveData() {
	QJsonArray array;
	for (const auto& item : m_todoItems) {
		array.append(item.toJson());
	}

	QJsonDocument doc(array);
	QFile file(m_filename);
	if (file.open(QIODevice::WriteOnly)) {
		file.write(doc.toJson());
	}
}

void MainWindow::refreshListWidget() {
	// 暂时断开信号，防止在添加 item 时触发 itemChanged
	ui->todoListWidget->blockSignals(true);

	ui->todoListWidget->clear();
	for (const auto& item : m_todoItems) {
		QString label = item.title;
		if (!item.description.isEmpty()) {
			label += " - " + item.description;
		}

		QListWidgetItem* widgetItem = new QListWidgetItem(label);
		widgetItem->setFlags(widgetItem->flags() | Qt::ItemIsUserCheckable);
		widgetItem->setCheckState(item.completed ? Qt::Checked : Qt::Unchecked);

		ui->todoListWidget->addItem(widgetItem);
	}

	ui->todoListWidget->blockSignals(false);
}
