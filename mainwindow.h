#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFile>
#include <QPushButton>
#include <QLabel>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QUrl>
#include <QFileInfo>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QSettings>
#include <QSize>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>

#include "model/itemdelegate.h"
#include "model/sounditemwidget.h"
#include "model/keybinddialog.h"
#include "model/hotkeymanager.h"
#include "model/viewmodel.h"
#include "model/hotkeymanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setStopAllKey(int key) { stopAllKey = key; }
    int getStopAllKey() const { return stopAllKey; }


protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    ItemViewModel itemVM;
    int stopAllKey = 0;
    HotkeyManager *hotkeyManager = nullptr;

    void refreshList();

private slots:

    void on_addButton_clicked();

    void chooseAudioOutput();

    void handleItemDelete(int index);

    void handleItemEdit(int index);

    void handleItemUpdate(int index, int status);

    void on_stopButton_clicked();

    void on_setStopAllKeyButton_clicked();
    
    void onWindowInitialized(); 

};

#endif // MAINWINDOW_H
