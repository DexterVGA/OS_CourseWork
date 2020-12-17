#include "mainwindow.h"
#include "ui_mainwindow.h"
#pragma comment(lib,"user32.lib")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon2.png"));
    taskView = new TaskViewer();
    taskView->hide();
    peView = new PEFiles();
    peView->hide();
    keyView = new KeyboardHook();
    keyView->setEnabled(false);
    keyView->hide();

    connect(ui->taskViewer_button, SIGNAL(clicked()), this, SLOT(taskViewer_show()));
    connect(ui->peFiles_button, SIGNAL(clicked()), this, SLOT(peFiles_show()));
    connect(ui->keyScaner_button, SIGNAL(clicked()), this, SLOT(keyScan_show()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::taskViewer_show()
{
    taskView->show();
}

void MainWindow::peFiles_show()
{
    peView->show();
}

void MainWindow::keyScan_show()
{
    keyView->setEnabled(true);
    keyView->show();
}
