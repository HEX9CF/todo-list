#ifndef TODOCREATEDIALOG_H
#define TODOCREATEDIALOG_H

#include <QDialog>

#include "../model/taskitem.h"

namespace Ui {
class TaskCreateDialog;
}

class TaskCreateDialog : public QDialog {
	Q_OBJECT

   public:
	explicit TaskCreateDialog(QWidget* parent = nullptr);
	~TaskCreateDialog();

	TaskItem getTaskItem() const;

   private slots:
	void onOkClicked();
	void onCancelClicked();
	void onClearClicked();

   private:
	Ui::TaskCreateDialog* ui;
	TaskItem m_todoItem;
};

#endif	// TODOCREATEDIALOG_H
