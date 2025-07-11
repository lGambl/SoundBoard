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
#include <QSize>
#include <QLineEdit>

#include "model/itemdelegate.h"
#include "model/sounditemwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowOpacity(0.9);
    ui->listWidget->setSpacing(10);

    // Load saved audio output device
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

    connect(ui->listWidget, &QListWidget::clicked, this, [this](const QModelIndex &index) {
        int itemIndex = index.row();
        QString itemName = QString::fromStdString(itemVM.getItem(itemIndex).getItemName());
        int status = itemVM.getItem(itemIndex).getItemStatus();
        qDebug() << "Item clicked:" << itemName << "Status:" << status;
        handleItemEdit(itemIndex);
    });

    ui->overallVolume->setRange(0, 100);
    ui->overallVolume->setValue(itemVM.getVolume());

    connect(ui->overallVolume, &QSlider::valueChanged, this, [this](int value) {
        itemVM.setVolume(value);
        float vol = value / 100.0f;
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (auto *w = qobject_cast<SoundItemWidget*>(ui->listWidget->itemWidget(item))) {
                w->setVolume(vol);
            }
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshList()
{
    qDebug() << "Refreshing list. Total items:" << itemVM.getItemCount();

    ui->listWidget->clear();

    for (size_t i = 0; i < itemVM.getItemCount(); ++i) {
        const auto &it = itemVM.getItem(i);

        QString filePath = QString::fromStdString(it.getItemName());
        QString displayName = QString::fromStdString(it.getDisplayName());

        auto *w = new SoundItemWidget(filePath, displayName, ui->listWidget);
        w->setVolume(ui->overallVolume->value() / 100.0f);

        QListWidgetItem *item = new QListWidgetItem;
        item->setSizeHint(w->sizeHint());
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, w);

        int itemIndex = static_cast<int>(i);

        connect(w, &SoundItemWidget::deleteRequested, this, [this, itemIndex]() {
            handleItemDelete(itemIndex);
        });

        connect(w, &SoundItemWidget::editRequested, this, [this, itemIndex]() {
            handleItemEdit(itemIndex);
        });
    }

    ui->listWidget->scrollToBottom();
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
    qDebug() << "AddButton clicked";
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Add Sound File",
        QString(),
        "Audio Files (*.wav *.mp3 *.ogg)"
    );

    if (!fileName.isEmpty()) {
        qDebug() << "Selected file:" << fileName;
        QString defaultName = QFileInfo(fileName).fileName();
        QString customName = QInputDialog::getText(this, "Enter Name", "Display Name:", QLineEdit::Normal, defaultName);
        if (customName.isEmpty()) customName = defaultName;

        item newItem(fileName.toStdString(), 0, customName.toStdString());
        itemVM.addItem(newItem);
        refreshList();
        ui->statusbar->showMessage(tr("Added item. Total now: %1").arg(itemVM.getItemCount()), 2000);
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

    for (auto &d : devices) {
        if (d.description() == sel) {
            SoundItemWidget::setAudioDevice(d);
            QSettings("MyCompany", "ToDo").setValue("audioOutput", d.id());
            break;
        }
    }
    refreshList();
}

void MainWindow::on_stopButton_clicked()
{
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        if (auto *w = qobject_cast<SoundItemWidget*>(ui->listWidget->itemWidget(item))) {
            w->stop();
        }
    }
}
