#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

public:
    void setStopAllKey(int key) { stopAllKey = key; }
    int getStopAllKey() const { return stopAllKey; }
};

#endif // MAINWINDOW_H
