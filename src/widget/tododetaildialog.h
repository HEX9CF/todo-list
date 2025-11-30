#ifndef TODODETAILDIALOG_H
#define TODODETAILDIALOG_H

#include <QDialog>

#include "../model/todoitem.h"

namespace Ui {
class TodoDetailDialog;
}

class TodoDetailDialog : public QDialog {
	Q_OBJECT

   public:
	explicit TodoDetailDialog(const TodoItem& item, QWidget* parent = nullptr);
	~TodoDetailDialog();

	TodoItem getTodoItem() const;

   private slots:
	void onUpdateClicked();
	void onCancelClicked();

   private:
	Ui::TodoDetailDialog* ui;
	TodoItem m_todoItem;
};

#endif	// TODODETAILDIALOG_H
