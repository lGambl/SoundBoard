#include "item.h"

item::item(const string& filePath, int status, const string& displayName, int keyBinding)
    : filePath(filePath), stat(status), displayName(displayName), keyBinding(keyBinding) {}

bool item::updateItem(const string& newDisplayName) {
    if (!newDisplayName.empty()) {
        displayName = newDisplayName;
        return true;
    }
    return false;
}

bool item::setItemStatus(int status) {
    stat = status;
    return true;
}

string item::getItemName() const {
    return filePath;
}

string item::getDisplayName() const {
    return displayName;
}

int item::getItemStatus() const {
    return stat;
}
