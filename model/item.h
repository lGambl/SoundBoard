#ifndef ITEM_H
#define ITEM_H

#include <string>
using namespace std;

class item {
    string what;
    int stat;

public:
    item(const string& itemName, int status);
    bool updateItem(const string& newItem);
    bool setItemStatus(int status);
    string getItemName() const;
    int getItemStatus() const;
};

#endif // ITEM_H
