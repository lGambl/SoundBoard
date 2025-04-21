#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFile>
#include <QPushButton>
#include <QLabel>


#include "model/itemdelegate.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Determine the path to items.txt
    QString appDir = QCoreApplication::applicationDirPath();
    QDir projectRootDir(appDir);
    projectRootDir.cdUp();

    QString filePath = projectRootDir.filePath("items.txt");

    qDebug() << "Looking for items.txt at:" << filePath;

    // Load items into the view model
    itemVM.setFilePath(filePath.toStdString());
    if (!itemVM.loadFromFile(filePath.toStdString())) {
        QMessageBox::warning(this, "Error", "Failed to load items from file.");
    } else {
        refreshTable();
    }

    // Connect cellChanged signal for updates
    connect(ui->tableWidget, &QTableWidget::cellChanged, this, &MainWindow::handleItemUpdate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshTable()
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(itemVM.getItemCount());
    ui->tableWidget->setColumnCount(3); // Name, Status, Actions
    ui->tableWidget->setHorizontalHeaderLabels({"Name", "Status", "Actions"});

    for (size_t i = 0; i < itemVM.getItemCount(); ++i) {
        const auto& item = itemVM.getItem(i);

        // Name column
        QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(item.getItemName()));
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable); // Make it non-editable
        ui->tableWidget->setItem(i, 0, nameItem);

        // Status column
        QTableWidgetItem *statusItem = new QTableWidgetItem(QString::number(item.getItemStatus()));
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable); // Make it non-editable
        ui->tableWidget->setItem(i, 1, statusItem);

        // Actions column
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);

        // Edit button
        QPushButton *editButton = new QPushButton("Edit");
        actionLayout->addWidget(editButton);

        // Delete button
        QPushButton *deleteButton = new QPushButton("Delete");
        actionLayout->addWidget(deleteButton);

        actionWidget->setLayout(actionLayout);
        ui->tableWidget->setCellWidget(i, 2, actionWidget);

        // Connect Edit button to handleItemEdit
        connect(editButton, &QPushButton::clicked, this, [this, i]() {
            handleItemEdit(i);
        });

        // Connect Delete button to handleItemDelete
        connect(deleteButton, &QPushButton::clicked, this, [this, i]() {
            handleItemDelete(i);
        });
    }
}

void MainWindow::handleItemDelete(int row)
{
    if (itemVM.removeItem(row)) {
        refreshTable();
    } else {
        QMessageBox::warning(this, "Error", "Could not delete item.");
    }
}

void MainWindow::handleItemEdit(int row)
{
    const auto& item = itemVM.getItem(row);

    // Create a dialog for editing
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Item");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // Name input
    QLabel *nameLabel = new QLabel("Name:");
    QLineEdit *nameEdit = new QLineEdit(QString::fromStdString(item.getItemName()));
    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);

    // Status input
    QLabel *statusLabel = new QLabel("Status:");
    QSpinBox *statusSpinBox = new QSpinBox();
    statusSpinBox->setRange(0, 1); // Assuming status is binary (e.g., 0 or 1)
    statusSpinBox->setValue(item.getItemStatus());
    layout->addWidget(statusLabel);
    layout->addWidget(statusSpinBox);

    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Show the dialog
    if (dialog.exec() == QDialog::Accepted) {
        QString newName = nameEdit->text();
        int newStatus = statusSpinBox->value();

        // Update the item in the view model
        if (!itemVM.updateItem(row, newName.toStdString()) ||
            !itemVM.updateItemStatus(row, newStatus)) {
            QMessageBox::warning(this, "Error", "Failed to update item.");
        }

        // Refresh the table
        refreshTable();
    }
}

void MainWindow::handleItemUpdate(int row, int column)
{
    if (column == 0) { // Name column
        QTableWidgetItem *nameItem = ui->tableWidget->item(row, column);
        if (nameItem) {
            QString newName = nameItem->text();
            if (!itemVM.updateItem(row, newName.toStdString())) {
                QMessageBox::warning(this, "Error", "Failed to update item name.");
                refreshTable(); // Revert changes
            }
        }
    } else if (column == 1) { // Status column
        QTableWidgetItem *statusItem = ui->tableWidget->item(row, column);
        if (statusItem) {
            bool ok;
            int newStatus = statusItem->text().toInt(&ok);
            if (ok) {
                if (!itemVM.updateItemStatus(row, newStatus)) {
                    QMessageBox::warning(this, "Error", "Failed to update item status.");
                    refreshTable(); // Revert changes
                }
            } else {
                QMessageBox::warning(this, "Error", "Invalid status value.");
                refreshTable(); // Revert changes
            }
        }
    }
}
