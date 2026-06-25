#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Build relative path pointing to the /data directory
string getDataPath(const string& filename);

// Character-by-character CSV reading loop (no getline split)
vector<vector<string>> readTXT(const string& filename);

// Overwrite file with 2D string data
bool writeTXT(const string& filename, const vector<vector<string>>& data);

// Append a single row of CSV data
bool appendTXT(const string& filename, const vector<string>& row);

// Find row index (0-indexed, including header) based on key matching at column index
int findRow(const vector<vector<string>>& data, const string& key, int colIndex);

// Check if a row with the given key at colIndex exists in the file
bool rowExists(const string& filename, const string& key, int colIndex);

// Custom helper: parse date "DD-MM-YYYY" into integers
bool parseDate(const string& dateStr, int& day, int& month, int& year);

// Custom date checker: returns true if date1 is chronologically strictly after date2
bool isDateAfter(const string& date1, const string& date2);

#endif
