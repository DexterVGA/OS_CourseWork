#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include <QWidget>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <QMessageBox>
#include <QLabel>

namespace Ui {
class KeyboardHook;
}

class KeyboardHook : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardHook(QWidget *parent = nullptr);
    ~KeyboardHook();
    static LRESULT CALLBACK keybProc(int code, WPARAM wParam, LPARAM lParam);

private:
    Ui::KeyboardHook *ui;
    HHOOK   keybHook;

private slots:
    void close_slot();
};

#endif // KEYBOARDHOOK_H
