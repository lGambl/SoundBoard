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
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QUrl>
#include <QFileInfo>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QSettings>

#include "model/itemdelegate.h"
#include "model/sounditemwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // load saved audio output device
    {
        QSettings settings("MyCompany", "ToDo");
        QString savedId = settings.value("audioOutput").toString();
        auto devices = QMediaDevices::audioOutputs();
        for (const auto &dev : devices) {
            if (dev.id() == savedId) {
                SoundItemWidget::setAudioDevice(dev);
                break;
            }
        }
    }

    // connect output action
    connect(ui->actionDevices, &QAction::triggered, this, &MainWindow::chooseAudioOutput);

    QString appDir = QCoreApplication::applicationDirPath();
    QDir projectRootDir(appDir);
    projectRootDir.cdUp();

    QString filePath = projectRootDir.filePath("items.txt");

    qDebug() << "Looking for items.txt at:" << filePath;

    itemVM.setFilePath(filePath.toStdString());
    if (!itemVM.loadFromFile(filePath.toStdString())) {
        QMessageBox::warning(this, "Error", "Failed to load items from file.");
    } else {
        refreshList();
    }

    connect(ui->listView, &QListView::clicked, this, [this](const QModelIndex &index) {
        int itemIndex = index.row();
        QString itemName = listModel->item(itemIndex)->text();
        int status = listModel->item(itemIndex)->data(Qt::UserRole).toInt();
        qDebug() << "Item clicked:" << itemName << "Status:" << status;

        handleItemEdit(itemIndex);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshList()
{
    if (!listModel) {
        listModel = new QStandardItemModel(this);
        ui->listView->setModel(listModel);
    }
    listModel->clear();

    for (size_t i = 0; i < itemVM.getItemCount(); ++i) {
        const auto &it = itemVM.getItem(i);
        QStandardItem *row = new QStandardItem;
        listModel->appendRow(row);
        QModelIndex idx = listModel->index(int(i), 0);

        QString fullPath = QString::fromStdString(it.getItemName());
        QString displayName = QFileInfo(fullPath).fileName();
        auto *w = new SoundItemWidget(fullPath, ui->listView);

        // playback is handled internally by SoundItemWidget
        connect(w, &SoundItemWidget::deleteRequested, [this, i]() {
            handleItemDelete(int(i));
        });

        ui->listView->setIndexWidget(idx, w);
    }
}

void MainWindow::handleItemDelete(int index)
{
    if (itemVM.removeItem(index)) {
        refreshList();
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete item.");
    }
}

void MainWindow::handleItemEdit(int index)
{
    QString currentName = QString::fromStdString(itemVM.getItem(index).getItemName());
    QString newName = QInputDialog::getText(this, "Edit Item", "Enter new name:", QLineEdit::Normal, currentName);

    if (!newName.isEmpty()) {
        if (itemVM.updateItem(index, newName.toStdString())) {
            refreshList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to update item.");
        }
    }
}

void MainWindow::handleItemUpdate(int index, int status)
{
    if (itemVM.updateItemStatus(index, status)) {
        refreshList();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update item status.");
    }
}

void MainWindow::on_addButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Add Sound File",
        QString(),
        "Audio Files (*.wav *.mp3 *.ogg)"
    );
    if (!fileName.isEmpty()) {
        item newItem(fileName.toStdString(), 0);
        itemVM.addItem(newItem);
        refreshList();
    }
}

void MainWindow::chooseAudioOutput()
{
    auto devices = QMediaDevices::audioOutputs();
    QStringList items;
    for (auto &d : devices)
        items << d.description();

    bool ok = false;
    int currentIndex = items.indexOf(SoundItemWidget::audioDevice().description());
    if (currentIndex < 0) currentIndex = 0;

    QString sel = QInputDialog::getItem(
        this,
        tr("Select Audio Output"),
        tr("Output Device:"),
        items,
        currentIndex,
        false,
        &ok
    );

    if (!ok || sel.isEmpty()) return;

    // set the QAudioOutput to your chosen device (e.g. VB-Cable)
    for (auto &d : devices) {
        if (d.description() == sel) {
            SoundItemWidget::setAudioDevice(d);
            QSettings("MyCompany","ToDo").setValue("audioOutput", d.id());
            break;
        }
    }
    refreshList();
}

void MainWindow::on_stopButton_clicked()
{
    // stop all players, don't delete
    for (int i = 0; i < listModel->rowCount(); ++i) {
        QModelIndex idx = listModel->index(i, 0);
        if (auto *w = qobject_cast<SoundItemWidget*>(ui->listView->indexWidget(idx))) {
            w->stop();
        }
    }
}