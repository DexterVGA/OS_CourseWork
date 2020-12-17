#ifndef TASKVIEWER_H
#define TASKVIEWER_H

#include <QWidget>
#include <QMessageBox>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

namespace Ui {
class TaskViewer;
}

class TaskViewer : public QWidget
{
    Q_OBJECT

public:
    explicit TaskViewer(QWidget *parent = nullptr);
    ~TaskViewer();

private:
    Ui::TaskViewer *ui;
    BOOL ListProcessModules(DWORD dwPID);
    QString str = "";
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;
    void updateViewer();

private slots:
    void close_slot();
    void update_slot();
};

#endif // TASKVIEWER_H
