#include "taskviewer.h"
#include "ui_taskviewer.h"

TaskViewer::TaskViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskViewer)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon2.png"));
    connect(ui->close_button, SIGNAL(clicked()), this, SLOT(close_slot()));
    connect(ui->update_button, SIGNAL(clicked()), this, SLOT(update_slot()));

    updateViewer();
}

TaskViewer::~TaskViewer()
{
    delete ui;
}

void TaskViewer::updateViewer()
{
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // снимок всех процессов
    if(hProcessSnap == INVALID_HANDLE_VALUE) {
        QMessageBox::critical(this, "Error", "Invalid handle value");
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(!Process32First( hProcessSnap, &pe32)) {
        QMessageBox::critical(this, "Error", "Process32First");
        CloseHandle( hProcessSnap );
        return;
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do {
        str += "\n\n==========================";
        str += "\nPROCESS NAME:  ";
        str += QString::fromWCharArray(pe32.szExeFile);
        str += "\n-------------------------------------------------------";

        // Retrieve the priority class.
        dwPriorityClass = 0;
        hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
        if(hProcess == NULL){
        } else {
            dwPriorityClass = GetPriorityClass(hProcess);
            if(!dwPriorityClass)
                CloseHandle(hProcess);
        }
        str += "\nProcess ID ";

        str += QString::number(pe32.th32ProcessID);
        str += "\nThread count ";
        str +=  QString::number(pe32.cntThreads);
        str += "\nParent process ID ";
        str +=  QString::number(pe32.th32ParentProcessID);
        str += "\nPriority base ";
        str +=  QString::number(pe32.pcPriClassBase);
        if(dwPriorityClass) {
            str += "\nPriority class   ";
            str +=  QString::number(dwPriorityClass);
        }


        // List the modules and threads associated with this process
        ListProcessModules(pe32.th32ProcessID);

    } while(Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    ui->textEdit->setText(str);
}

BOOL TaskViewer::ListProcessModules(DWORD dwPID) //получение модулей и потоков, связанных с процессом
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if(hModuleSnap == INVALID_HANDLE_VALUE) {
    //printError( TEXT("CreateToolhelp32Snapshot (of modules)") );
        return(FALSE);
    }

    // Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    // Retrieve information about the first module,
    // and exit if unsuccessful
    if(!Module32First(hModuleSnap, &me32)) {
        // printError( TEXT("Module32First") );  // show cause of failure
        CloseHandle(hModuleSnap);           // clean the snapshot object
        return(FALSE);
    }

    // Now walk the module list of the process,
    // and display information about each module
    do {
        str += "\n\n     MODULE NAME: ";
        str += QString::fromWCharArray(me32.szModule);
        str += "\n     Executable     =";
        str += QString::fromWCharArray(me32.szExePath );
        str += "\n     Process ID     =";
        str += QString::number(me32.th32ProcessID);
        str += "\n     Ref count (g)  =";
        str += QString::number(me32.GlblcntUsage);
        str += "\n     Ref count (p)  =";
        str += QString::number(me32.ProccntUsage);
        str += "\n     Base address   =";
        str += QString::number((DWORD) me32.modBaseAddr);
        str += "\n     Base size      = ";
        str += QString::number( me32.modBaseSize );
    } while(Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
    return(TRUE);
}

void TaskViewer::close_slot()
{
    ui->textEdit->clear();
    this->hide();
}

void TaskViewer::update_slot()
{
    ui->textEdit->clear();
    updateViewer();
}
