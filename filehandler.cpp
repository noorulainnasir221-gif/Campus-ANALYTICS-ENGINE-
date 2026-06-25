#include "filehandler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


std::string getDataPath(const std::string& filename) {
    return "data/" + filename;
}

std::vector<std::vector<std::string>> readTXT(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::string fullPath = getDataPath(filename);
    std::ifstream file(fullPath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Error] Could not open file for reading: " << fullPath << std::endl;
        return data;
    }

    std::vector<std::string> currentRow;
    std::string currentCell = "";
    char c;
    while (file.get(c)) {
        if (c == '\r') {
            // Skip carriage return to normalize newlines
            continue;
        } else if (c == '\n') {
            // End of cell and end of row
            currentRow.push_back(currentCell);
            currentCell = "";
            // Avoid pushing empty lines
            if (!currentRow.empty() && !(currentRow.size() == 1 && currentRow[0].empty())) {
                data.push_back(currentRow);
            }
            currentRow.clear();
        } else if (c == ',') {
            // End of cell
            currentRow.push_back(currentCell);
            currentCell = "";
        } else {
            currentCell += c;
        }
    }
    
    // Catch final line without a trailing newline
    if (!currentCell.empty() || !currentRow.empty()) {
        currentRow.push_back(currentCell);
        if (!currentRow.empty() && !(currentRow.size() == 1 && currentRow[0].empty())) {
            data.push_back(currentRow);
        }
    }

    file.close();
    return data;
}

bool writeTXT(const std::string& filename, const std::vector<std::vector<std::string>>& data) {
    std::string fullPath = getDataPath(filename);
    std::ofstream file(fullPath, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Error] Could not open file for writing: " << fullPath << std::endl;
        return false;
    }

    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
    return true;
}

bool appendTXT(const std::string& filename, const std::vector<std::string>& row) {
    std::string fullPath = getDataPath(filename);
    std::ofstream file(fullPath, std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Error] Could not open file for appending: " << fullPath << std::endl;
        return false;
    }

    for (size_t j = 0; j < row.size(); ++j) {
        file << row[j];
        if (j < row.size() - 1) {
            file << ",";
        }
    }
    file << "\n";
    file.close();
    return true;
}

int findRow(const std::vector<std::vector<std::string>>& data, const std::string& key, int colIndex) {
    for (size_t i = 0; i < data.size(); ++i) {
        if (colIndex >= 0 && colIndex < static_cast<int>(data[i].size())) {
            if (data[i][colIndex] == key) {
                return static_cast<int>(i);
            }
        }
    }
    return -1;
}

bool rowExists(const std::string& filename, const std::string& key, int colIndex) {
    std::vector<std::vector<std::string>> data = readTXT(filename);
    return findRow(data, key, colIndex) != -1;
}

bool parseDate(const std::string& dateStr, int& day, int& month, int& year) {
    // Expected format: DD-MM-YYYY (exactly 10 chars)
    if (dateStr.length() != 10 || dateStr[2] != '-' || dateStr[5] != '-') {
        day = month = year = 0;
        return false;
    }
    try {
        day = std::stoi(dateStr.substr(0, 2));
        month = std::stoi(dateStr.substr(3, 2));
        year = std::stoi(dateStr.substr(6, 4));
        return true;
    } catch (...) {
        day = month = year = 0;
        return false;
    }
}

bool isDateAfter(const std::string& date1, const std::string& date2) {
    int d1 = 0, m1 = 0, y1 = 0;
    int d2 = 0, m2 = 0, y2 = 0;
    
    bool parsed1 = parseDate(date1, d1, m1, y1);
    bool parsed2 = parseDate(date2, d2, m2, y2);
    
    // Treat invalid/empty/unset dates (like 00-00-0000) as chronologically early (epoch)
    if (!parsed1) return false;
    if (!parsed2) return true; // valid date1 is always after invalid date2
    
    if (y1 != y2) {
        return y1 > y2;
    }
    if (m1 != m2) {
        return m1 > m2;
    }
    return d1 > d2;
}
