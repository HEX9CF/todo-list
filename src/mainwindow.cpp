#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);	// 设置 UI 界面
}

MainWindow::~MainWindow() {
	delete ui;	// 释放 UI 内存
}
