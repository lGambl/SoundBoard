#ifndef ITEM_H
#define ITEM_H

#include <string>
using namespace std;

class item {
    string filePath;
    string displayName;
    int stat;

public:
    item(const string& filePath, int status, const string& displayName);

    bool updateItem(const string& newDisplayName);

    bool setItemStatus(int status);

    string getItemName() const;

    string getDisplayName() const;

    int getItemStatus() const;
};

#endif // ITEM_H
