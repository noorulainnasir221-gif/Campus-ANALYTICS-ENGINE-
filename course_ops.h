#ifndef COURSE_OPS_H
#define COURSE_OPS_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Course {
    char course_code[50];
    char course_name[100];
    int credit_hours;
    char instructor[100];
    int capacity;
    int enrolled;
    char prerequisite[50]; // NONE or another course code
};

struct Enrollment {
    char enrollment_id[50];
    char roll_no[50];
    char course_code[50];
    int semester;
    char enrollment_date[50];
    char status[50]; // active, dropped, completed
    
    // Evaluation components & final grades
    double assignment;
    double quiz;
    double mid;
    double project;
    double final_exam;
    double total_marks;
    char grade[10]; // e.g., A, B+, F, N/A
};

// Load course records from courses.txt into a vector
vector<Course> loadCourses();

// Save course records to courses.txt (RMW pattern)
bool saveCourses(const vector<Course>& courses);

// Load enrollment records from enrollments.txt into a vector
vector<Enrollment> loadEnrollments();

// Save enrollment records to enrollments.txt (RMW pattern)
bool saveEnrollments(const vector<Enrollment>& enrollments);

// Search helpers
int findCourseIndex(const vector<Course>& courses, const string& course_code);
int findEnrollmentIndex(const vector<Enrollment>& enrollments, const string& roll_no, const string& course_code);

// Interactive operations called from the menu
void addCourseInteractive();
void listCoursesInteractive();
void enrollStudentInteractive();
void dropCourseInteractive();
void listEnrollmentsInteractive();

#endif
