#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Attendance {
    char log_id[50];
    char roll_no[50];
    char course_code[50];
    char session_date[50]; // DD-MM-YYYY
    char status[10]; // P, A, L
};

// Load attendance records from attendance_log.txt into a vector
vector<Attendance> loadAttendance();

// Save attendance records to attendance_log.txt (RMW pattern)
bool saveAttendance(const vector<Attendance>& logs);

// Calculate student attendance percentage in a course
double getAttendancePercentage(const string& roll_no, const string& course_code);

// Interactive menu options
void markAttendanceInteractive();
void viewAttendanceReportInteractive();

#endif
