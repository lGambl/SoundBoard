#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowOpacity(0.9);
    ui->listWidget->setSpacing(10);

    // Load saved audio output device
    {
        QSettings settings("Gamb", "SoundBoard");
        QByteArray savedId = settings.value("audioOutput").toByteArray();
        auto devices = QMediaDevices::audioOutputs();
        for (const auto &dev : devices) {
            if (dev.id() == savedId) {
                SoundItemWidget::setAudioDevice(dev);
                break;
            }
        }
    }

    connect(ui->actionDevices, &QAction::triggered, this, &MainWindow::chooseAudioOutput);
    connect(ui->actionSetStopAllKey, &QAction::triggered, this, &MainWindow::on_setStopAllKeyButton_clicked);

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
        float vol = static_cast<float>(value);
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (auto *w = qobject_cast<SoundItemWidget*>(ui->listWidget->itemWidget(item))) {
                w->setVolume(vol);
            }
        }
    });

    setFocusPolicy(Qt::StrongFocus);

    hotkeyManager = new HotkeyManager(this);
    connect(hotkeyManager, &HotkeyManager::hotkeyPressed, this, [this](int index) {
        if (index >= 0 && index < static_cast<int>(itemVM.getItemCount())) {
            QListWidgetItem *item = ui->listWidget->item(index);
            if (auto *w = qobject_cast<SoundItemWidget*>(ui->listWidget->itemWidget(item))) {
                w->stop();
                w->setVolume(static_cast<float>(ui->overallVolume->value()));
                w->play();
            }
        }
    });
    connect(hotkeyManager, &HotkeyManager::stopAllPressed, this, &MainWindow::on_stopButton_clicked);

    QTimer::singleShot(0, this, &MainWindow::onWindowInitialized);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshList()
{
    qDebug() << "Refreshing list. Total items:" << itemVM.getItemCount();


    if (!ui->listWidget) {
        qCritical() << "[MainWindow] ui->listWidget is nullptr!";
        return;
    }
    ui->listWidget->clear();

    if (hotkeyManager) hotkeyManager->unregisterAll();

    QString appDir = QCoreApplication::applicationDirPath();
    QDir projectRootDir(appDir);
    projectRootDir.cdUp();
    QString itemsDir = projectRootDir.absolutePath();

    for (size_t i = 0; i < itemVM.getItemCount(); ++i) {
        try {
            const auto &it = itemVM.getItem(i);
            QString filePath = QString::fromStdString(it.getItemName());
            QString displayName = QString::fromStdString(it.getDisplayName());

            QFileInfo fi(filePath);
            if (fi.isRelative()) {
                filePath = QDir(itemsDir).filePath(filePath);
            }

            if (!QFile::exists(filePath)) {
                qWarning() << "[MainWindow] Skipping missing file (resolved):" << filePath;
                continue;
            }

            qDebug() << "[MainWindow] Creating SoundItemWidget with filePath:" << filePath << ", displayName:" << displayName;
            if (!ui->listWidget) {
                qCritical() << "[MainWindow] ui->listWidget is nullptr inside loop!";
                continue;
            }
            auto *w = new SoundItemWidget(filePath, displayName, ui->listWidget);
            if (!w) {
                qCritical() << "[MainWindow] Failed to create SoundItemWidget for" << filePath;
                continue;
            }
            w->setVolume(static_cast<float>(ui->overallVolume->value()));

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

            connect(w, &SoundItemWidget::keyBindRequested, this, [this, itemIndex]() {
                KeyBindDialog dlg(this);
                if (dlg.exec() == QDialog::Accepted) {
                    int key = dlg.getKey();
                    itemVM.updateItemKey(itemIndex, key);
                    refreshList();
                }
            });

            int itemKey = it.getKeyBinding();
            if (itemKey != 0) hotkeyManager->registerHotkey(itemKey, static_cast<int>(i));
        } catch (const std::exception& ex) {
            qCritical() << "[MainWindow] Exception in refreshList for item" << i << ":" << ex.what();
        } catch (...) {
            qCritical() << "[MainWindow] Unknown exception in refreshList for item" << i;
        }
    }

    hotkeyManager->registerStopAllHotkey(stopAllKey);

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

    int selectedIndex = items.indexOf(sel);
    if (selectedIndex >= 0 && selectedIndex < devices.size()) {
        const QAudioDevice &device = devices.at(selectedIndex);
        SoundItemWidget::setAudioDevice(device);
        QSettings("Gamb", "SoundBoard").setValue("audioOutput", device.id());
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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    qDebug() << "Key pressed:" << key;
    if (key == stopAllKey && stopAllKey != 0) {
        on_stopButton_clicked();
        return;
    }
    for (size_t i = 0; i < itemVM.getItemCount(); ++i) {
        int itemKey = itemVM.getItem(i).getKeyBinding();
        qDebug() << "Item" << i << "keybind:" << itemKey;
        if (itemKey != 0 && itemKey == key) {
            QListWidgetItem *item = ui->listWidget->item(static_cast<int>(i));
            if (auto *w = qobject_cast<SoundItemWidget*>(ui->listWidget->itemWidget(item))) {
                w->stop();
                w->setVolume(static_cast<float>(ui->overallVolume->value()));
                w->play();
            }
            break;
        }
    }
}

void MainWindow::on_setStopAllKeyButton_clicked()
{
KeyBindDialog dlg(this);
if (dlg.exec() == QDialog::Accepted) {
    int key = dlg.getKey();
    setStopAllKey(key);
    if (hotkeyManager) hotkeyManager->registerStopAllHotkey(key);
}
}

void MainWindow::onWindowInitialized() 
{
    if (hotkeyManager) hotkeyManager->registerStopAllHotkey(stopAllKey);
    refreshList();
}
