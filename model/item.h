#ifndef ITEM_H
#define ITEM_H

#include <string>
using namespace std;

class item {
    string filePath;
    string displayName;
    int stat;
    int keyBinding = 0;

public:
    item(const string& filePath, int status, const string& displayName, int keyBinding = 0);

    bool updateItem(const string& newDisplayName);

    bool setItemStatus(int status);

    string getItemName() const;

    string getDisplayName() const;

    int getItemStatus() const;

    void setKeyBinding(int key) { keyBinding = key; }
    int getKeyBinding() const { return keyBinding; }
};

#endif // ITEM_H
