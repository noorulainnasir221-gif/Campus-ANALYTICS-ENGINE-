#ifndef GRADES_H
#define GRADES_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Calculate course GPA based on final total marks
double calculateGPAFromMarks(double marks);

// Calculate Letter Grade based on final total marks
string calculateGradeFromMarks(double marks);

// Recalculates and updates the student's CGPA in students.txt based on enrollment grades
bool syncStudentCGPA(const string& roll_no);

// Interactive menu options
void addOrUpdateGradeInteractive();
void viewStudentGradesInteractive();

#endif
