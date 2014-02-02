#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QWidget;
class QBoxLayout;
class QLineEdit;
class QPushButton;
class QStringList;
class KCPlainTextBrowser;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void onActionGenerate();
    void onActionBrowseFolder();

private:
    bool iterateFolder(const QString &path);

    struct fileDataItem
    {
        QString fileName;
        QString filePath;
        QString modifiedDate;
    };
    QList<fileDataItem> trick;  //I don't know how to name this?!

    QWidget *contents;
    QLabel *folderSelectHint, *versionNumberHint;
    QLineEdit *folderPath, *versionNumber;
    QPushButton *makeList, *cancel;
    KCPlainTextBrowser *outputDetails;

    QBoxLayout *browseLayout, *buttonLayout, *versionLayout;
};

#endif // MAINWINDOW_H
