#include "tododetaildialog.h"

#include <QMessageBox>

#include "ui_tododetaildialog.h"

TodoDetailDialog::TodoDetailDialog(const TodoItem& item, QWidget* parent)
	: QDialog(parent), ui(new Ui::TodoDetailDialog), m_todoItem(item) {
	ui->setupUi(this);

	ui->titleInput->setText(item.title);
	ui->categoryInput->setText(item.category);
	ui->priorityInput->setCurrentIndex(item.priority);

	if (item.deadline.isValid()) {
		ui->deadlineInput->setDateTime(item.deadline);
	} else {
		ui->deadlineInput->setDateTime(QDateTime::currentDateTime());
	}

	ui->statusInput->setChecked(item.completed);
	ui->descriptionInput->setText(item.description);

	connect(ui->updateButton, &QPushButton::clicked, this,
			&TodoDetailDialog::onUpdateClicked);
	connect(ui->cancelButton, &QPushButton::clicked, this,
			&TodoDetailDialog::onCancelClicked);
}

TodoDetailDialog::~TodoDetailDialog() { delete ui; }

TodoItem TodoDetailDialog::getTodoItem() const { return m_todoItem; }

void TodoDetailDialog::onUpdateClicked() {
	QString title = ui->titleInput->text().trimmed();
	if (title.isEmpty()) {
		QMessageBox::warning(this, "警告", "标题不能为空！");
		return;
	}

	m_todoItem.title = title;
	m_todoItem.category = ui->categoryInput->text().trimmed();
	m_todoItem.priority = ui->priorityInput->currentIndex();
	m_todoItem.deadline = ui->deadlineInput->dateTime();
	m_todoItem.completed = ui->statusInput->isChecked();
	m_todoItem.description = ui->descriptionInput->toPlainText().trimmed();

	accept();
}

void TodoDetailDialog::onCancelClicked() { reject(); }
