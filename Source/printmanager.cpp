#include "printmanager.h"
#include <windows.h>
#include <atlstr.h>
#include <stdio.h>
#include <stdlib.h>

PrintManager::PrintManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PrintManager){

    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    edit = false;
    name = false;

    ui->comboBox->setCurrentIndex(2);
    path = M.getPath();
    clearSearch();
    loadTable();

}

PrintManager::~PrintManager(){
    delete ui;
}

// Manager Interface /////////////////////////////////

//Use Current Query Items to Search
void PrintManager::search(){
    M.search(q_Link, q_Name, q_Subs, q_Date, q_Type, q_Note);
}

//Clear All Query Items
void PrintManager::clearSearch(){
    q_Link = "";
    q_Name = "";
    q_Subs = "";
    q_Date = "";
    q_Type = "";
    q_Note = "";
    search();
}

//Load Current Search onto Table
void PrintManager::loadTable(){
    std::vector<Print*> Q = M.getQuery();
    std::string printData = "";
    std::ifstream checkFile;
    /*add rows and columns*/
    ui->tableWidget->setRowCount(Q.size()); //Temporary q.size
    ui->tableWidget->setColumnCount(6);

    for(int i=0; i<ui->tableWidget->rowCount(); i++){
        for(int j=0; j<ui->tableWidget->columnCount(); j++){
            QTableWidgetItem *pCell = ui->tableWidget->item(i, j);
            if(!pCell){
                pCell = new QTableWidgetItem;
                ui->tableWidget->setItem(i, j, pCell);
            }

            switch(j){
                case 0:
                    printData = Q.at(i)->getSubs();
                    break;
                case 1:
                    printData = Q.at(i)->getLink();
                    checkFile.open(path + "Prints/" + printData);
                    if(!checkFile.good()){
                       pCell->setBackground(QColor::QColor(255, 153, 153));
                    }
                    else{
                       pCell->setBackground(QColor::QColor(255, 255, 255));
                    }
                    checkFile.close();
                    break;
                case 2:
                    printData = Q.at(i)->getName();
                    break;
                case 3:
                    printData = Q.at(i)->getType();
                    break;
                case 4:
                    printData = Q.at(i)->getDate();
                    break;
                case 5:
                    printData = Q.at(i)->getNote();
                    break;
            }

            pCell->setText(QString::fromStdString(printData));

        }
    }
}

//Launch Selectected File
void PrintManager::launch(std::string file){
    QString qpath = QString::fromStdString(path + "Prints/") +
                    QString::fromStdString(file);

    QDesktopServices::openUrl(QUrl(("file:///" + qpath), QUrl::TolerantMode));
}

//Get Selected Files to Launch
void PrintManager::openSelected(){
    int sz = ceil(ui->tableWidget->selectedItems().size());
    QMessageBox Msgbox;
    std::ifstream checkFile;
    std::vector<std::string> failed;
    bool goodFileFound = false;

    QMessageBox::StandardButton reply; // = QMessageBox::Cancel;
    if( sz / 6 > 5){
        reply = QMessageBox::question(this, "Warning",
            "Opening too many files may cause programs to crash.\n\nDo you wish to continue?",
            QMessageBox::Ok | QMessageBox::Cancel);
    }
    if(reply != QMessageBox::Cancel){
        int j = 0;
        for(int i = 0; i < sz; i=i+6){
            j = ui->tableWidget->selectedItems().at(i)->row();

            std::string file = ui->tableWidget->item(j,1)->text().toUtf8().constData();
            checkFile.open(path + "Prints/" + file);
            if(!checkFile.good()){
                failed.push_back(file);
            }
            else{
                goodFileFound = true;
                M.access(file);
                launch(file);
            }
            checkFile.close();
        }
        for(int k = 0; k < failed.size(); k++){
            Msgbox.setText(("Error: File \"" + failed.at(k) + "\" does not exist.").c_str());
            Msgbox.exec();
        }
        if(goodFileFound){
            search();
            loadTable();
        }
    }
    ui->tableWidget->clearSelection();
}

// Slots //////////////////////////////////////////////

void PrintManager::on_pushButton_Search_clicked(){
    std::string query = ui->lineEdit->text().toUtf8().constData();
    int selection = ui->comboBox->currentIndex();
    switch(selection){
        case 0:
            q_Subs = query;
            break;
        case 1:
            q_Link = query;
            break;
        case 2:
            q_Name = query;
            break;
        case 3:
            q_Type = query;
            break;
        case 4:
            q_Date = query;
            break;
        case 5:
            q_Note = query;
            break;
   }
   if(query != ""){
       search();
       loadTable();
   }
}

//Enter in Search Window
void PrintManager::on_lineEdit_returnPressed(){
    on_pushButton_Search_clicked();
}



//Reset Button Released
void PrintManager::on_pushButton_Reset_clicked(){
    ui->lineEdit->clear();
    q_Name = "";
    q_Subs = "";
    q_Type = "";
    q_Note = "";
    q_Link = "";
    q_Date = "";
    search();
    loadTable();
}

//Double click cell (only use row)
void PrintManager::on_tableWidget_cellDoubleClicked(int row, int column){
    openSelected();
}

//Open Button Released
void PrintManager::on_pushButton_Open_clicked(){
    openSelected();
}

void PrintManager::on_pushButton_Delete_clicked(){
    int sz = ceil(ui->tableWidget->selectedItems().size());
    int j = 0;
    for(int i = 0; i < sz; i=i+6){
        j = ui->tableWidget->selectedItems().at(i)->row();
        std::string file = ui->tableWidget->item(j,1)->text().toUtf8().constData();
        M.remove(file);
    }
    search();
    loadTable();
}

void PrintManager::on_menuChangeDirectory_triggered(){
    path = QFileDialog::getExistingDirectory().toUtf8().constData();
    path = path + "/";
    M.setPath(path);
    search();
    loadTable();
}

void PrintManager::on_menuUpdateNames_triggered(){

    CString str= ("\"" + path + "Source/ExtractNames.bat\"").c_str();
    CString action= "open";
    ShellExecute(NULL,action,str,NULL,NULL,SW_SHOW);

    //system(("\"" + path + "Source/ExtractNames.bat\"").c_str());
    name = true;
}

void PrintManager::on_actionImportNames_triggered(){
    QMessageBox Msgbox;
    std::ifstream importFile;
    std::string file;

    if(name){
        importFile.open(path + "Source/" + "Names.cvs");
        if(importFile.good()){
            while(std::getline(importFile, file)){
                if(M.getPrint(file) == nullptr)
                    M.import(file);
            }
            importFile.close();
        }
        search();
        loadTable();
        name = false;
    }
    else{
        Msgbox.setText("First select \"Update Names\" to get generate the list of filenames.");
        Msgbox.exec();
    }
}

void PrintManager::on_actionExport_to_Excel_triggered(){
    std::ofstream exportFile;
    std::string fileName = (QInputDialog::getText(0, "Text input", "File Name:")).toUtf8().constData();
    std::vector<Print*> allPrints = M.getAll();

    exportFile.open(path + "Output/" + fileName + ".cvs");
    exportFile << "File Name\tPrint Name\tSubstation\tDate\tType\tComments\tLast Access\tTimes Accessed\n";
    for(int i = 0; i < allPrints.size(); i++){
        exportFile << *allPrints.at(i) << "\n";
    }
    exportFile.close();


    CString str= ("\"" + path + "Output/" + fileName + ".cvs\"").c_str();
    CString action= "open";
    ShellExecute(NULL,action,str,NULL,NULL,SW_SHOW);

    //system(("\"" + path + "Output/" + fileName + ".cvs\"").c_str());
}

void PrintManager::on_actionCreate_Backup_triggered(){
    std::ofstream exportFile;
    std::string fileName = "backup";
    std::vector<Print*> allPrints = M.getAll();

    exportFile.open(path + "Source/" + fileName + ".dat");
    for(int i = 0; i < allPrints.size(); i++){
        exportFile << *allPrints.at(i) << "\n";
    }
    exportFile.close();
}

void PrintManager::on_actionHelp_triggered(){
    qWarning("hmm");
    CString str= ("\"" + path + "Source/Help.pdf\"").c_str();
    CString action= "open";
    ShellExecute(NULL,action,str,NULL,NULL,SW_SHOW);
}

void PrintManager::on_pushButton_Edit_clicked(){
  if(edit){
      edit = false;
      updateTable();
      ui->tableWidget->blockSignals(false);
      ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
      ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
      ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
      ui->tableWidget->setLineWidth(1);
      ui->tableWidget->setFrameShape(QFrame::StyledPanel);
      ui->tableWidget->setGridStyle(Qt::SolidLine);
      //Buttons
      ui->pushButton_Delete->setDisabled(false);
      ui->pushButton_Open->setDisabled(false);
      ui->pushButton_Search->setDisabled(false);
      ui->pushButton_Reset->setDisabled(false);
      //Line Edit
      ui->lineEdit->setDisabled(false);
      //Drop Down Menu
      ui->comboBox->setDisabled(false);
      //File Menu
      ui->menuBar->setDisabled(false);

      //Set color
      int sz = ui->tableWidget->rowCount();
      for(int i = 0; i < sz; i++){
          for(int j = 0; j < 6; j++){
              ui->tableWidget->item(i,j)->setBackground(QColor::QColor(255, 255, 255));
          }
      }
      loadTable();
  }
  else{
    edit = true;
    cacheTable();
    //Table
    ui->tableWidget->blockSignals(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidget->setLineWidth(2);
    ui->tableWidget->setFrameShape(QFrame::Box);
    ui->tableWidget->setGridStyle(Qt::DashLine);
    ui->tableWidget->clearSelection();
    //Buttons
    ui->pushButton_Delete->setDisabled(true);
    ui->pushButton_Open->setDisabled(true);
    ui->pushButton_Search->setDisabled(true);
    ui->pushButton_Reset->setDisabled(true);
    //Line Edit
    ui->lineEdit->setDisabled(true);
    //Drop Down Menu
    ui->comboBox->setDisabled(true);
    //File Menu
    ui->menuBar->setDisabled(true);

    //ui->label_EditMode->setVisible(true);

    //Set Color
    int sz = ui->tableWidget->rowCount();
    for(int i = 0; i < sz; i++){
        for(int j = 0; j < 6; j++){
            ui->tableWidget->item(i,j)->setBackground(QColor::QColor(242, 242, 242));
        }
    }
  }
}

void PrintManager::cacheTable(){
    int row = ui->tableWidget->rowCount();
    int column = ui->tableWidget->columnCount();
    std::vector<std::string> temp;

    cache.clear();

    for(int i = 0; i < row; i++){
        temp.clear();
        for(int j = 0; j < column; j++){
           temp.push_back(ui->tableWidget->item(i, j)->text().toUtf8().constData());
            //qWarning(ui->tableWidget->item(i, j)->text().toUtf8().constData());
        }
        cache.push_back(temp);
    }
}

void PrintManager::updateTable(){
    int row = ui->tableWidget->rowCount();
    int column = ui->tableWidget->columnCount();

    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j++){
            //qWarning(ui->tableWidget->item(i, j)->text().toUtf8().constData());
            if(cache.at(i).at(j) != ui->tableWidget->item(i, j)->text().toUtf8().constData()){
                updateItem(i, j);
                ui->tableWidget->item(i,j)->setSelected(false);
            }
        }
    }
}

void PrintManager::updateItem(int row, int column){
    Print* P = M.getPrint(cache.at(row).at(1));
    switch(column){
        case 0:
            P->setSubs(ui->tableWidget->item(row, column)->text().toUtf8().constData());
            break;
        case 1:
            P->setLink(ui->tableWidget->item(row, column)->text().toUtf8().constData());
            break;
        case 2:
            P->setName(ui->tableWidget->item(row, column)->text().toUtf8().constData());
            break;
        case 3:
            P->setType(ui->tableWidget->item(row, column)->text().toUtf8().constData());
            break;
        case 4:
            P->setDate(ui->tableWidget->item(row, column)->text().toUtf8().constData());
            break;
        case 5:
            P->setNote(ui->tableWidget->item(row, column)->text().toUtf8().constData());
            break;

    }
}






