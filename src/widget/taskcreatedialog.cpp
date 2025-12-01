#include <QMessageBox>

#include "taskcreatedialog.h"
#include "ui_taskcreatedialog.h"

TaskCreateDialog::TaskCreateDialog(QWidget* parent)
	: QDialog(parent), ui(new Ui::TaskCreateDialog) {
	ui->setupUi(this);
	ui->deadlineInput->setDateTime(QDateTime::currentDateTime());

	connect(ui->okButton, &QPushButton::clicked, this,
			&TaskCreateDialog::onOkClicked);
	connect(ui->cancelButton, &QPushButton::clicked, this,
			&TaskCreateDialog::onCancelClicked);
	connect(ui->clearButton, &QPushButton::clicked, this,
			&TaskCreateDialog::onClearClicked);
}

TaskCreateDialog::~TaskCreateDialog() { delete ui; }

TaskItem TaskCreateDialog::getTaskItem() const { return m_todoItem; }

void TaskCreateDialog::onOkClicked() {
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
		TaskItem(-1, title, description, false, category, priority, deadline);
	accept();
}

void TaskCreateDialog::onCancelClicked() { reject(); }

void TaskCreateDialog::onClearClicked() {
	ui->titleInput->clear();
	ui->descriptionInput->clear();
	ui->categoryInput->clear();
	ui->priorityInput->setCurrentIndex(0);
	ui->deadlineInput->setDateTime(QDateTime::currentDateTime());
}
