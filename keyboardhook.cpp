#include "keyboardhook.h"
#include "ui_keyboardhook.h"

enum Keys
{
    ShiftKey = 16,
    Capital = 20,
};

int shift() {
    return GetKeyState(VK_LSHIFT) < 0 || GetKeyState(VK_RSHIFT) < 0;
}

int caps() {
    return (GetKeyState(VK_CAPITAL) & 1) == 1;
}

QLabel *lb;

KeyboardHook::KeyboardHook(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyboardHook)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon2.png"));

    connect(ui->close_button, SIGNAL(clicked()), this, SLOT(close_slot()));

    lb = ui->label;
    HINSTANCE hInstance = GetModuleHandle(NULL);
    keybHook = SetWindowsHookEx(WH_KEYBOARD_LL, keybProc, hInstance, 0);
    if (keybHook == NULL) {
        QMessageBox::critical(this,"ERROR","Keyboard Hook failed\n");
    }
}

KeyboardHook::~KeyboardHook()
{
    delete ui;
}

LRESULT CALLBACK KeyboardHook::keybProc(int code, WPARAM wParam, LPARAM lParam) {
    if (wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
        DWORD wVirtKey = kbdStruct->vkCode;
        DWORD wScanCode = kbdStruct->scanCode;

        BYTE lpKeyState[256];
        GetKeyboardState(lpKeyState);
        lpKeyState[Keys::ShiftKey] = 0;
        lpKeyState[Keys::Capital] = 0;
        if (shift()) {
            lpKeyState[Keys::ShiftKey] = 0x80;
        }
        if (caps()) {
            lpKeyState[Keys::Capital] = 0x01;
        }

        char result;
        ToAscii(wVirtKey, wScanCode, lpKeyState, (LPWORD)&result, 0);
        //std::cout << result << std::endl;
        char res[2]={result,0};
        if(result>32)
            lb->setText("Последняя нажатая клавиша - " + QString::fromLatin1(res));
        else
            lb->setText("Последняя нажатая клавиша - ");
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}

void KeyboardHook::close_slot()
{
    lb->setText("нажмите кнопку");
    this->hide();
    this->setEnabled(false);
}
