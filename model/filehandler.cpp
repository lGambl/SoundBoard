#include "filehandler.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

vector<item> FileHandler::loadItemsFromFile(const string& filename) {
    vector<item> items;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return items; // Return empty vector if file cannot be opened
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string path;
        int status;

        if (getline(iss, path, '"') && getline(iss, path, '"') && iss >> status) {
            string displayName;
            int keyBinding = 0;

            if (iss >> ws && iss.peek() == '"') {
                iss.get();
                getline(iss, displayName, '"');

                if (!(iss >> keyBinding)) {
                    keyBinding = 0;
                }
            } else {
                size_t pos = path.find_last_of("/\\");
                displayName = (pos != string::npos) ? path.substr(pos + 1) : path;
            }
            // Check for empty path
            if (path.empty()) {
                cerr << "Skipping item with empty path. Line: " << line << endl;
                continue;
            }
            items.emplace_back(path, status, displayName, keyBinding);
        } else {
            cerr << "Invalid line format in file: " << line << endl;
        }
    }

    file.close();
    return items;
}

bool FileHandler::saveItemsToFile(const string& filename, const vector<item>& items) {
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Failed to open file for writing: " << filename << endl;
        return false;
    }

    for (const auto& item : items) {
        file << "\"" << item.getItemName() << "\" "
             << item.getItemStatus() << " \""
             << item.getDisplayName() << "\" "
             << item.getKeyBinding() << endl;
    }
    file.close();
    return true;
}