#include "pefiles.h"
#include "ui_pefiles.h"

PEFiles::PEFiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PEFiles)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon2.png"));
    connect(ui->view_button, SIGNAL(clicked()), this, SLOT(viewButton_clicked()));
    connect(ui->load_button, SIGNAL(clicked()), this, SLOT(load_slot()));
    connect(ui->close_button, SIGNAL(clicked()), this, SLOT(close_slot()));
}

PEFiles::~PEFiles()
{
    delete ui;
}

void PEFiles::load_slot()
{
    QString filename = QFileDialog::getOpenFileName(0, "Выберите файл", QDir::currentPath(), "*.exe *.dll *.ocx *.sys *.scr *.drv *.cpl *.efi");
    ui->lineEdit->setText(filename);
}

void PEFiles::close_slot()
{
    ui->textEdit->clear();
    ui->textEdit_2->clear();
    ui->lineEdit->clear();
    this->hide();
}

void PEFiles::viewButton_clicked()
{
    showExport();
    showImport();
}

void PEFiles::showExport()
{
    str1 = "";
    ui->textEdit->setText(str1);
    ui->textEdit_2->setText(str1);

    LOADED_IMAGE LoadedImage;
    PUCHAR BaseAddress;
    DWORD RVAExpDir, VAExpAddress;
    IMAGE_EXPORT_DIRECTORY* ExpTable;
    char* sName;
    DWORD nNames;
    char* pName;
    char** pNames;
    QString filename = ui->lineEdit->text();
    memset(Buffer2, 0, sizeof(Buffer2));
    filename.toWCharArray(Buffer2);
    QByteArray ba = filename.toLocal8Bit();
    const char *c_str2 = ba.data();
    str1 = "";
    str1 += filename;
    str1 +="\n";
    ui->textEdit->setText(str1);

    if (!MapAndLoad(c_str2, NULL, &LoadedImage, TRUE, TRUE) && !MapAndLoad(c_str2, NULL, &LoadedImage, FALSE, TRUE)) {
        QMessageBox::critical(this, "Error", "Can't map and load file");
        exit(1);
    }

    BaseAddress = LoadedImage.MappedAddress;// возвращает базовый адрес
    if(LoadedImage.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size) {
        RVAExpDir = LoadedImage.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        VAExpAddress = (DWORD)ImageRvaToVa(LoadedImage.FileHeader, BaseAddress, RVAExpDir, NULL);
        ExpTable = (IMAGE_EXPORT_DIRECTORY*)VAExpAddress;
        sName = (char*)ImageRvaToVa(LoadedImage.FileHeader, BaseAddress, ExpTable->Name, NULL);
        pNames = (char**)ImageRvaToVa(LoadedImage.FileHeader, BaseAddress, ExpTable->AddressOfNames, NULL);
        nNames = ExpTable->NumberOfNames;
        for (unsigned i = 0; i < nNames; i++) {
            pName = (char*)ImageRvaToVa(LoadedImage.FileHeader, BaseAddress, (DWORD)*pNames, NULL);
            str1 += "\n";
            str1 += pName;
            *pNames++;
        }
    }
    else {
        QMessageBox::critical(this, "Error", "Export lib is empty");
    }
    UnMapAndLoad(&LoadedImage);

    ui->textEdit->setText(str1);
    str1 = "";
}

void PEFiles::showImport()
{
    HANDLE hFile;
    HANDLE hFileMapping;
    LPVOID lpFileBase;
    PIMAGE_THUNK_DATA thunk;
    PIMAGE_IMPORT_BY_NAME pOrdinalName;
    PIMAGE_DOS_HEADER pDOSHeader;
    PIMAGE_NT_HEADERS pNTHeader;
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    PIMAGE_SECTION_HEADER pSection;
    hFile = CreateFile(Buffer2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE) return;
    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL); //создание мап файла
    if (!hFileMapping) {
        CloseHandle(hFile);
        return;
    }
    lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0); //отражение мапфайла в память
    if (!lpFileBase) {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        return;
    }

    pDOSHeader = (PIMAGE_DOS_HEADER)lpFileBase; //адрес таблицы
    if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE) return;

    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDOSHeader + pDOSHeader->e_lfanew); //факт. начало самого файла(смещение)
    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) return;

    int importsStartRVA = pNTHeader-> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress; //адрес таблицы
    if (!importsStartRVA) return;

    pSection = GetEnclosingSectionHeader(importsStartRVA, pNTHeader); //начало функций
    if (!pSection) return;

    int delta = pSection->VirtualAddress - pSection->PointerToRawData; //размер секций, выровненный до размера файла
    importDesc = (PIMAGE_IMPORT_DESCRIPTOR) (importsStartRVA - delta + (DWORD)lpFileBase);
    while (importDesc->TimeDateStamp || importDesc->Name) { //дата создание или имя
        QString strtemp = QString(QByteArray((const char*)(PBYTE)(importDesc->Name) - delta + (DWORD)lpFileBase)); //название dll со смещением
        str1 += "\n\n";
        str1 += strtemp;
        str1 += "\n\n";
        thunk = (PIMAGE_THUNK_DATA)importDesc->Characteristics; //название dll
        if (!thunk) thunk = (PIMAGE_THUNK_DATA)importDesc->FirstThunk;
        thunk = (PIMAGE_THUNK_DATA)( (PBYTE)thunk - delta + (DWORD)lpFileBase);
        while (thunk->u1.AddressOfData) {
            if (!(thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)thunk->u1.AddressOfData;
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)pOrdinalName - delta + (DWORD)lpFileBase);
                str1 += pOrdinalName->Name;
                str1 += "\n";
            }
            thunk++;
        }
        importDesc++;

    }
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
    ui->textEdit_2->setText(str1);
}

PIMAGE_SECTION_HEADER PEFiles:: GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader) { //получает дескриптор секции
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    for (unsigned i=0; i<pNTHeader->FileHeader.NumberOfSections; i++, section++) {
         if ((rva >= section->VirtualAddress) && (rva < (section->VirtualAddress + section->Misc.VirtualSize)))
         return section;
    }
    return 0;
}
