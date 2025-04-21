#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ItemViewModel itemVM;

    // Refresh the table with the current items from the view model
    void refreshTable();

private slots:
    // Handle deleting an item from the table
    void handleItemDelete(int row);

    // Handle editing an item in the table
    void handleItemEdit(int row);

    void handleItemUpdate(int row, int column);
};

#endif // MAINWINDOW_H
