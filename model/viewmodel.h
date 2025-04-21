#ifndef ITEM_VIEW_MODEL_H
#define ITEM_VIEW_MODEL_H

#include <vector>
#include <string>
#include "item.h"
using namespace std;

class ItemViewModel {
    vector<item> items;
    string filePath;

public:
    ItemViewModel();

    bool loadFromFile(const string& filename);
    void setFilePath(const string& filename);
    void addItem(const item& newItem);
    bool removeItem(size_t index);
    bool updateItem(size_t index, const string& newName);
    bool updateItemStatus(size_t index, int newStatus);

    size_t getItemCount() const;
    const item& getItem(size_t index) const;
    vector<item> getAllItems() const;

private:
    void saveToFile();
};

#endif // ITEM_VIEW_MODEL_H
