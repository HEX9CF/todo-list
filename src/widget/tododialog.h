#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QDialog>

#include "../model/todoitem.h"

namespace Ui {
class TodoDialog;
}

class TodoDialog : public QDialog {
	Q_OBJECT

   public:
	explicit TodoDialog(QWidget* parent = nullptr);
	~TodoDialog();

	TodoItem getTodoItem() const;

   private slots:
	void onOkClicked();
	void onCancelClicked();
	void onClearClicked();

   private:
	Ui::TodoDialog* ui;
	TodoItem m_todoItem;
};

#endif	// TODODIALOG_H
