#include "grades.h"
#include "attendance.h"
#include "course_ops.h"
#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
using namespace std;


double calculateGPAFromMarks(double marks) {
    if (marks >= 90.0) return 4.0;
    if (marks >= 85.0) return 3.7;
    if (marks >= 80.0) return 3.3;
    if (marks >= 75.0) return 3.0;
    if (marks >= 70.0) return 2.7;
    if (marks >= 65.0) return 2.3;
    if (marks >= 60.0) return 2.0;
    if (marks >= 55.0) return 1.7;
    if (marks >= 50.0) return 1.0;
    return 0.0;
}

std::string calculateGradeFromMarks(double marks) {
    if (marks >= 90.0) return "A";
    if (marks >= 85.0) return "A-";
    if (marks >= 80.0) return "B+";
    if (marks >= 75.0) return "B";
    if (marks >= 70.0) return "B-";
    if (marks >= 65.0) return "C+";
    if (marks >= 60.0) return "C";
    if (marks >= 55.0) return "C-";
    if (marks >= 50.0) return "D";
    return "F";
}

bool syncStudentCGPA(const std::string& roll_no) {
    std::vector<Enrollment> enrollments = loadEnrollments();
    std::vector<Course> courses = loadCourses();

    double totalWeightedPoints = 0.0;
    int totalCredits = 0;

    for (size_t i = 0; i < enrollments.size(); ++i) {
        if (std::string(enrollments[i].roll_no) == roll_no && 
            std::string(enrollments[i].status) != "dropped" &&
            std::string(enrollments[i].grade) != "N/A") {
            
            // Look up course credits
            int cIdx = findCourseIndex(courses, enrollments[i].course_code);
            if (cIdx != -1) {
                int credits = courses[cIdx].credit_hours;
                double gpa = calculateGPAFromMarks(enrollments[i].total_marks);
                totalWeightedPoints += (gpa * credits);
                totalCredits += credits;
            }
        }
    }

    double newCGPA = 0.0;
    if (totalCredits > 0) {
        newCGPA = totalWeightedPoints / totalCredits;
    }

    // Atomic RMW update to students.txt
    std::vector<Student> students = loadStudents();
    int sIdx = findStudentIndex(students, roll_no);
    if (sIdx != -1) {
        students[sIdx].cgpa = newCGPA;
        return saveStudents(students);
    }
    return false;
}

void addOrUpdateGradeInteractive() {
    std::cout << "\n--- Grade & Marks Entry ---\n";
    std::string roll_no, course_code;
    std::cout << "Enter Student Roll Number: ";
    std::cin >> roll_no;
    std::cout << "Enter Course Code: ";
    std::cin >> course_code;
    std::cin.ignore(10000, '\n');

    // 1. Verify active enrollment exists
    std::vector<Enrollment> enrollments = loadEnrollments();
    int eIdx = findEnrollmentIndex(enrollments, roll_no, course_code);
    if (eIdx == -1) {
        std::cout << "[Error] Enrollment record not found for student in this course.\n";
        return;
    }
    if (std::string(enrollments[eIdx].status) != "active") {
        std::cout << "[Error] Enrollment is inactive (" << enrollments[eIdx].status << "). Cannot grade.\n";
        return;
    }

    // 2. Input 5 component marks with range validation
    double assignment, quiz, mid, project, final_exam;
    
    std::cout << "Enter Assignment Marks (0-15): ";
    while (!(std::cin >> assignment) || assignment < 0.0 || assignment > 15.0) {
        std::cout << "Invalid marks. Range 0-15: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cout << "Enter Quiz Marks (0-10): ";
    while (!(std::cin >> quiz) || quiz < 0.0 || quiz > 10.0) {
        std::cout << "Invalid marks. Range 0-10: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cout << "Enter Midterm Exam Marks (0-25): ";
    while (!(std::cin >> mid) || mid < 0.0 || mid > 25.0) {
        std::cout << "Invalid marks. Range 0-25: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cout << "Enter Project Marks (0-15): ";
    while (!(std::cin >> project) || project < 0.0 || project > 15.0) {
        std::cout << "Invalid marks. Range 0-15: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cout << "Enter Final Exam Marks (0-35): ";
    while (!(std::cin >> final_exam) || final_exam < 0.0 || final_exam > 35.0) {
        std::cout << "Invalid marks. Range 0-35: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cin.ignore(10000, '\n');

    // 3. Apply attendance penalties (Cross-Module call to attendance.h)
    double attendanceRate = getAttendancePercentage(roll_no, course_code);
    double penalty = 0.0;
    if (attendanceRate < 65.0) {
        penalty = 10.0;
    } else if (attendanceRate < 75.0) {
        penalty = 5.0;
    }

    double baseTotal = assignment + quiz + mid + project + final_exam;
    double finalTotal = baseTotal - penalty;
    if (finalTotal < 0.0) finalTotal = 0.0;
    if (finalTotal > 100.0) finalTotal = 100.0;

    std::string grade = calculateGradeFromMarks(finalTotal);
    double gpa = calculateGPAFromMarks(finalTotal);

    // 4. Update enrollment structures (RMW)
    enrollments[eIdx].assignment = assignment;
    enrollments[eIdx].quiz = quiz;
    enrollments[eIdx].mid = mid;
    enrollments[eIdx].project = project;
    enrollments[eIdx].final_exam = final_exam;
    enrollments[eIdx].total_marks = finalTotal;
    std::snprintf(enrollments[eIdx].grade, sizeof(enrollments[eIdx].grade), "%s", grade.c_str());

    if (saveEnrollments(enrollments)) {
        std::cout << "\n========================================\n";
        std::cout << " [Success] Marks updated successfully!\n";
        std::cout << " Base Marks       : " << baseTotal << " / 100.0\n";
        std::cout << " Attendance Rate  : " << std::fixed << std::setprecision(2) << attendanceRate << "%\n";
        std::cout << " Attendance Penalty: -" << penalty << " marks\n";
        std::cout << " Final Total Marks: " << finalTotal << "\n";
        std::cout << " Assigned Grade   : " << grade << " (GPA: " << gpa << ")\n";
        std::cout << "========================================\n";
        
        // Sync CGPA in students.txt
        if (syncStudentCGPA(roll_no)) {
            std::cout << "[Info] Student CGPA synchronized in directory database.\n";
        } else {
            std::cout << "[Warning] Grade saved, but failed to synchronize CGPA.\n";
        }
    } else {
        std::cout << "[Error] Failed to write updates to enrollments.\n";
    }
}

void viewStudentGradesInteractive() {
    std::cout << "\n--- View Student Transcript ---\n";
    std::string roll_no;
    std::cout << "Enter Student Roll Number: ";
    std::cin >> roll_no;
    std::cin.ignore(10000, '\n');

    std::vector<Student> students = loadStudents();
    int sIdx = findStudentIndex(students, roll_no);
    if (sIdx == -1) {
        std::cout << "[Error] Student not found.\n";
        return;
    }

    std::vector<Enrollment> enrollments = loadEnrollments();
    std::vector<Course> courses = loadCourses();

    std::cout << "\n========================================================================\n";
    std::cout << " Academic Transcript: " << students[sIdx].name << " (" << roll_no << ")\n";
    std::cout << " Department         : " << students[sIdx].department << "\n";
    std::cout << " Current Semester   : " << students[sIdx].semester << "\n";
    std::cout << " CGPA               : " << std::fixed << std::setprecision(2) << students[sIdx].cgpa << "\n";
    std::cout << "========================================================================\n";
    std::cout << std::left << std::setw(12) << "Course Code"
              << std::setw(25) << "Course Title"
              << std::setw(8) << "Credits"
              << std::setw(8) << "Marks"
              << std::setw(8) << "Grade"
              << std::setw(8) << "GPA"
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(80, '-') << "\n";

    bool gradedAny = false;
    for (size_t i = 0; i < enrollments.size(); ++i) {
        if (std::string(enrollments[i].roll_no) == roll_no) {
            std::string title = "Unknown Course";
            int credits = 0;
            int cIdx = findCourseIndex(courses, enrollments[i].course_code);
            if (cIdx != -1) {
                title = courses[cIdx].course_name;
                credits = courses[cIdx].credit_hours;
            }

            double gpa = calculateGPAFromMarks(enrollments[i].total_marks);
            
            std::cout << std::left << std::setw(12) << enrollments[i].course_code
                      << std::setw(25) << title
                      << std::setw(8) << credits;
            
            if (std::string(enrollments[i].grade) == "N/A") {
                std::cout << std::setw(8) << "N/A"
                          << std::setw(8) << "N/A"
                          << std::setw(8) << "N/A";
            } else {
                std::cout << std::setw(8) << std::fixed << std::setprecision(2) << enrollments[i].total_marks
                          << std::setw(8) << enrollments[i].grade
                          << std::setw(8) << std::fixed << std::setprecision(2) << gpa;
                gradedAny = true;
            }
            std::cout << std::setw(10) << enrollments[i].status << "\n";
        }
    }
    std::cout << "========================================================================\n";
    if (!gradedAny) {
        std::cout << "[Info] No courses have been graded yet for this student.\n";
    }
}
