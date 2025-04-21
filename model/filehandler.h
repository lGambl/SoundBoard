#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>
using namespace std;

#include "item.h"

class FileHandler {
public:
    static vector<item> loadItemsFromFile(const string& filename);
    static bool saveItemsToFile(const string& filename, const vector<item>& items);
};

#endif // FILEHANDLER_H