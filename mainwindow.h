#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStandardItemModel>
#include "model/viewmodel.h"

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

private:
    Ui::MainWindow *ui;
    ItemViewModel itemVM;               // ViewModel instance
    QStandardItemModel *listModel = nullptr; // Model for QListView

    // Refresh the list view with the current items from the view model
    void refreshList();

private slots:

    void on_addButton_clicked();
    void chooseAudioOutput();

    void handleItemDelete(int index);

    void handleItemEdit(int index);

    void handleItemUpdate(int index, int status);
};

#endif // MAINWINDOW_H
