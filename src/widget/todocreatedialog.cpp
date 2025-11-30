#include "todocreatedialog.h"

#include <QMessageBox>

#include "ui_todocreatedialog.h"

TodoCreateDialog::TodoCreateDialog(QWidget* parent)
	: QDialog(parent), ui(new Ui::TodoCreateDialog) {
	ui->setupUi(this);
	ui->deadlineInput->setDateTime(QDateTime::currentDateTime());

	connect(ui->okButton, &QPushButton::clicked, this,
			&TodoCreateDialog::onOkClicked);
	connect(ui->cancelButton, &QPushButton::clicked, this,
			&TodoCreateDialog::onCancelClicked);
	connect(ui->clearButton, &QPushButton::clicked, this,
			&TodoCreateDialog::onClearClicked);
}

TodoCreateDialog::~TodoCreateDialog() { delete ui; }

TodoItem TodoCreateDialog::getTodoItem() const { return m_todoItem; }

void TodoCreateDialog::onOkClicked() {
	QString title = ui->titleInput->text().trimmed();
	QString description = ui->descriptionInput->toPlainText().trimmed();
	QString category = ui->categoryInput->text().trimmed();
	int priority = ui->priorityInput->currentIndex();
	QDateTime deadline = ui->deadlineInput->dateTime();

	if (title.isEmpty()) {
		QMessageBox::warning(this, "警告", "标题不能为空！");
		return;
	}

	m_todoItem =
		TodoItem(-1, title, description, false, category, priority, deadline);
	accept();
}

void TodoCreateDialog::onCancelClicked() { reject(); }

void TodoCreateDialog::onClearClicked() {
	ui->titleInput->clear();
	ui->descriptionInput->clear();
	ui->categoryInput->clear();
	ui->priorityInput->setCurrentIndex(0);
	ui->deadlineInput->setDateTime(QDateTime::currentDateTime());
}
