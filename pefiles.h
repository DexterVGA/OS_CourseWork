#ifndef PEFILES_H
#define PEFILES_H

#include <QWidget>
#include <QMessageBox>
#include <windows.h>
#include <stdio.h>
#include <imagehlp.h>
#include <process.h>
#include <QFileDialog>

namespace Ui {
class PEFiles;
}

class PEFiles : public QWidget
{
    Q_OBJECT

public:
    explicit PEFiles(QWidget *parent = nullptr);
    ~PEFiles();

private:
    Ui::PEFiles *ui;
    HANDLE hFileMapOut;
    LPVOID MapViewOut;
    QString str1 = "";
    WCHAR Buffer2[256];
    PIMAGE_SECTION_HEADER  GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader);
    void showExport();
    void showImport();

private slots:
    void load_slot();
    void viewButton_clicked();
    void close_slot();
};

#endif // PEFILES_H
