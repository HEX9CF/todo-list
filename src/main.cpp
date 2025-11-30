#include <QApplication>

#include "controller/mainwindow.h"

// 应用程序入口点
int main(int argc, char* argv[]) {
	QApplication a(argc, argv);	 // 创建应用程序对象
	a.setOrganizationName("HEX9CF");
	a.setApplicationName("TodoList");
	MainWindow w;	  // 创建主窗口
	w.show();		  // 显示主窗口
	return a.exec();  // 进入事件循环
}
