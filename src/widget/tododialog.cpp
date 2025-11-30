#include "tododialog.h"

#include <QMessageBox>

#include "ui_tododialog.h"

TodoDialog::TodoDialog(QWidget* parent)
	: QDialog(parent), ui(new Ui::TodoDialog) {
	ui->setupUi(this);
	ui->deadlineInput->setDateTime(QDateTime::currentDateTime());

	connect(ui->okButton, &QPushButton::clicked, this,
			&TodoDialog::onOkClicked);
	connect(ui->cancelButton, &QPushButton::clicked, this,
			&TodoDialog::onCancelClicked);
	connect(ui->clearButton, &QPushButton::clicked, this,
			&TodoDialog::onClearClicked);
}

TodoDialog::~TodoDialog() { delete ui; }

TodoItem TodoDialog::getTodoItem() const { return m_todoItem; }

void TodoDialog::onOkClicked() {
	QString title = ui->titleInput->text().trimmed();
	QString description = ui->descriptionInput->text().trimmed();
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

void TodoDialog::onCancelClicked() { reject(); }

void TodoDialog::onClearClicked() {
	ui->titleInput->clear();
	ui->descriptionInput->clear();
	ui->categoryInput->clear();
	ui->priorityInput->setCurrentIndex(0);
	ui->deadlineInput->setDateTime(QDateTime::currentDateTime());
}
