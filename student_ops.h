#ifndef STUDENT_OPS_H
#define STUDENT_OPS_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Student {
    char roll_no[50];
    char name[100];
    char department[100];
    int semester;
    double cgpa;
    char status[50]; // active, inactive
};

// Load student records from students.txt into a vector of Student structs
vector<Student> loadStudents();

// Save student records from a vector of Student structs to students.txt (RMW pattern)
bool saveStudents(const vector<Student>& students);

// Find student index in a vector by roll number. Returns -1 if not found.
int findStudentIndex(const vector<Student>& students, const string& roll_no);

// Interactive operations called from the menu
void addStudentInteractive();
void viewStudentInteractive();
void updateStudentInteractive();
void deleteStudentInteractive();
void listStudentsInteractive();

#endif
