#ifndef TODODETAILDIALOG_H
#define TODODETAILDIALOG_H

#include <QDialog>

#include "../model/taskitem.h"

namespace Ui {
class TaskDetailDialog;
}

class TaskDetailDialog : public QDialog {
	Q_OBJECT

   public:
	explicit TaskDetailDialog(const TaskItem& item, QWidget* parent = nullptr);
	~TaskDetailDialog();

	TaskItem getTaskItem() const;

   private slots:
	void onUpdateClicked();
	void onCancelClicked();

   private:
	Ui::TaskDetailDialog* ui;
	TaskItem m_todoItem;
};

#endif	// TODODETAILDIALOG_H
