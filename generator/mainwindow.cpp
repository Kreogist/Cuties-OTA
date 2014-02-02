#include <QDebug>

#include <QMessageBox>
#include <QDateTime>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#include <QSettings>
#include <QStringList>

#include "kcplaintextbrowser.h"

#include <QKeySequence>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    contents=new QWidget(this);
    setCentralWidget(contents);

    QBoxLayout *mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, contents);
    contents->setLayout(mainLayout);

    versionLayout=new QBoxLayout(QBoxLayout::LeftToRight);

    versionNumberHint=new QLabel("Version: ",this);
    versionLayout->addWidget(versionNumberHint);

    versionNumber=new QLineEdit(this);
    versionLayout->addWidget(versionNumber);

    mainLayout->addLayout(versionLayout);

    folderSelectHint=new QLabel("Please select application folder:", this);
    mainLayout->addWidget(folderSelectHint);

    browseLayout=new QBoxLayout(QBoxLayout::LeftToRight);

    folderPath=new QLineEdit(this);
    browseLayout->addWidget(folderPath);

    QPushButton *browseFolder=new QPushButton(this);
    browseFolder->setText("&Browse");
    connect(browseFolder, SIGNAL(clicked()),
            this, SLOT(onActionBrowseFolder()));
    browseLayout->addWidget(browseFolder);

    mainLayout->addLayout(browseLayout);

    buttonLayout=new QBoxLayout(QBoxLayout::LeftToRight);

    makeList=new QPushButton("&Generate" ,this);
    makeList->setDefault(true);
    connect(makeList, SIGNAL(clicked()),
            this, SLOT(onActionGenerate()));
    buttonLayout->addWidget(makeList);

    cancel=new QPushButton("&Cancel" ,this);
    cancel->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(cancel, SIGNAL(clicked()),
            this, SLOT(close()));
    buttonLayout->addWidget(cancel);

    mainLayout->addLayout(buttonLayout);

    outputDetails=new KCPlainTextBrowser(this);
    mainLayout->addWidget(outputDetails, 1);
}

MainWindow::~MainWindow()
{
    browseLayout->deleteLater();
    buttonLayout->deleteLater();
    versionLayout->deleteLater();
}

void MainWindow::onActionGenerate()
{
    outputDetails->appendPlainText("Creating Version Information File...");
    QSettings settings(qApp->applicationDirPath()+"/versionInfo",
                       QSettings::IniFormat);
    settings.clear();
    outputDetails->appendPlainText("Writing Basic Information...");
    settings.beginGroup("Basic");
    settings.setValue("version", versionNumber->text());
    outputDetails->appendPlainText("Creating Application File Information...");
    QString folderPathText=folderPath->text();
    trick.clear();
    iterateFolder(folderPathText);
    int trickCount=trick.count();
    outputDetails->appendPlainText("Writing Application File Information...");
    settings.setValue("FileCount", trickCount);
    settings.endGroup();
    settings.beginGroup("FileAbstract");
    settings.beginWriteArray("Files");
    for(int i=0;i<trickCount;i++)
    {
        fileDataItem currentFileInfo=trick.at(i);
        outputDetails->appendPlainText("Writing #" +
                                       QString::number(i) +
                                       " File Information: " +
                                       currentFileInfo.fileName);
        settings.setArrayIndex(i);
        settings.setValue("FileName", currentFileInfo.fileName);
        settings.setValue("FilePath",
                         currentFileInfo.filePath.mid(folderPathText.length()));
        settings.setValue("ModifyDate", currentFileInfo.modifiedDate);
    }
    settings.endArray();
    settings.endGroup();
    QMessageBox::information(this,
                             "List Create Success",
                             QString("File has been written at \n" +
                                     qApp->applicationDirPath() +
                                     "/versionInfo"),
                             QMessageBox::Ok);
}

void MainWindow::onActionBrowseFolder()
{
    QFileDialog *selectFolder=new QFileDialog(this, "Application Path");
    selectFolder->setFileMode(QFileDialog::DirectoryOnly);
    selectFolder->exec();
    if(selectFolder->selectedFiles().count()!=0)
    {
        folderPath->setText(selectFolder->selectedFiles().at(0));
    }
}

bool MainWindow::iterateFolder(const QString &path)
{
    QDir currentDirectory(path);
    if(!currentDirectory.exists())
    {
        return false;
    }
    currentDirectory.setFilter(QDir::Dirs|QDir::Files);
    currentDirectory.setSorting(QDir::DirsFirst);
    QFileInfoList contentsList=currentDirectory.entryInfoList();
    int i=0;
    do
    {
        QFileInfo currentFileInfo=contentsList.at(i);
        if(currentFileInfo.fileName()=="." || currentFileInfo.fileName()=="..")
        {
            i++;
            continue;
        }
        if(currentFileInfo.isDir())
        {
            iterateFolder(currentFileInfo.filePath());
        }
        else
        {
            fileDataItem currentFileDataItem;
            currentFileDataItem.fileName=currentFileInfo.fileName();
            currentFileDataItem.filePath=currentFileInfo.path();
            currentFileDataItem.modifiedDate=
                    currentFileInfo.lastModified().toString("yyyyMMdd");
            outputDetails->appendPlainText("Add File " +
                                           currentFileDataItem.fileName +
                                           " to the list.");
            trick.append(currentFileDataItem);
        }
        i++;
    }
    while(i<contentsList.size());
    return true;
}
