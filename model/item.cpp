#include "item.h"

item::item(const string& itemName, int status)
    : what(itemName), stat(status) {}

bool item::updateItem(const string& newItem) {
    if (!newItem.empty()) {
        what = newItem;
        return true;
    }
    return false;
}

bool item::setItemStatus(int status) {
    stat = status;
    return true;
}

string item::getItemName() const {
    return what;
}

int item::getItemStatus() const {
    return stat;
}
