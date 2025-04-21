#include "viewmodel.h"
#include "filehandler.h"
#include <qlogging.h>
#include <QDebug>
#include <QFile>

ItemViewModel::ItemViewModel() {}

bool ItemViewModel::loadFromFile(const string& filename) {
    filePath = filename;
    qDebug() << QString::fromStdString(filename);
    items = FileHandler::loadItemsFromFile(filename);

    // Check if the file was successfully loaded
    if (items.empty()) {
        // If the file is empty but exists, return true
        QFile file(QString::fromStdString(filename));
        if (file.exists() && file.size() == 0) {
            qInfo("File is empty but loaded successfully.");
            return true;
        }

        // If the file doesn't exist or couldn't be opened, return false
        qWarning("Failed to load items from file.");
        return false;
    }

    return true;
}

void ItemViewModel::setFilePath(const string& filename) {
    filePath = filename;
}

void ItemViewModel::addItem(const item& newItem) {
    items.push_back(newItem);
    saveToFile();
}

bool ItemViewModel::removeItem(size_t index) {
    if (index >= items.size()) return false;
    items.erase(items.begin() + index);
    saveToFile();
    return true;
}

bool ItemViewModel::updateItem(size_t index, const string& newName) {
    if (index >= items.size()) return false;
    if (items[index].updateItem(newName)) {
        saveToFile();
        return true;
    }
    return false;
}

bool ItemViewModel::updateItemStatus(size_t index, int newStatus) {
    if (index >= items.size()) return false;
    if (items[index].setItemStatus(newStatus)) {
        saveToFile();
        return true;
    }
    return false;
}

size_t ItemViewModel::getItemCount() const {
    return items.size();
}

const item& ItemViewModel::getItem(size_t index) const {
    return items.at(index);
}

vector<item> ItemViewModel::getAllItems() const {
    return items;
}

void ItemViewModel::saveToFile() {
    if (!filePath.empty()) {
        FileHandler::saveItemsToFile(filePath, items);
    } else {
        qWarning("File path is not set. Cannot save items.");
    }
}
