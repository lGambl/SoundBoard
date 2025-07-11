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
        // Read filePath between quotes
        if (getline(iss, path, '"') && getline(iss, path, '"') && iss >> status) {
            string displayName;
            // Check if displayName is provided
            if (iss >> ws && iss.peek() == '"') {
                iss.get(); // consume '"'
                getline(iss, displayName, '"');
            } else {
                // Default displayName to filename
                size_t pos = path.find_last_of("/\\");
                displayName = (pos != string::npos) ? path.substr(pos + 1) : path;
            }
            items.emplace_back(path, status, displayName);
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
             << item.getDisplayName() << "\"" << endl;
    }
    file.close();
    return true;
}