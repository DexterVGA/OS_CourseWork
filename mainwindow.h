#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <taskviewer.h>
#include <pefiles.h>
#include <keyboardhook.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static LRESULT CALLBACK keybProc(int code, WPARAM wParam, LPARAM lParam);

private slots:
    void taskViewer_show();
    void peFiles_show();
    void keyScan_show();

private:
    Ui::MainWindow *ui;
    HHOOK   keybHook;
    TaskViewer *taskView;
    PEFiles *peView;
    KeyboardHook *keyView;
};
#endif // MAINWINDOW_H
