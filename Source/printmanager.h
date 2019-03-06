#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <QMainWindow>
#include "ui_printmanager.h"
#include "Manager.h"
#include <QMessageBox>
#include <QProcess>
#include <QFileDialog>
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QDir>

//For debugging output
#include <QTextStream>
#include <sstream>

namespace Ui {
class PrintManager;
}

class PrintManager : public QMainWindow
{
    Q_OBJECT

private:
    Ui::PrintManager *ui;
    Manager M;
    std::string path;
    std::vector<std::string> links;
    std::vector<std::vector<std::string>> cache;
    std::string q_Link;
    std::string q_Name;
    std::string q_Subs;
    std::string q_Date;
    std::string q_Type;
    std::string q_Note;
    bool edit;
    bool name;

public:
    explicit PrintManager(QWidget *parent = 0);
    ~PrintManager();

    void search();

    void clearSearch();

    void loadTable();

private slots:

    void on_lineEdit_returnPressed();
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void launch(std::string file);
    void openSelected();
    void cacheTable();
    void updateTable();
    void updateItem(int row, int column);
    void on_pushButton_Search_clicked();
    void on_pushButton_Reset_clicked();
    void on_pushButton_Open_clicked();
    void on_pushButton_Edit_clicked();
    void on_pushButton_Delete_clicked();

    void on_menuChangeDirectory_triggered();
    void on_actionImportNames_triggered();
    void on_menuUpdateNames_triggered();
    void on_actionExport_to_Excel_triggered();
    void on_actionCreate_Backup_triggered();
    void on_actionHelp_triggered();
};

#endif // PRINTMANAGER_H
