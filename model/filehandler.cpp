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
        string name;
        int status;

        if (getline(iss, name, '"') && getline(iss, name, '"') && iss >> status) {
            items.emplace_back(name, status);
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
        file << "\"" << item.getItemName() << "\" " << item.getItemStatus() << endl;
    }
    file.close();
    return true;
}