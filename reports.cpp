#include "reports.h"
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "fee_tracker.h"
#include "filehandler.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
using namespace std;


void generateAcademicPerformanceReport() {
    std::cout << "\n--- Department-Wise Performance Summary ---\n";
    std::vector<Student> students = loadStudents();
    if (students.empty()) {
        std::cout << "No student data available.\n";
        return;
    }
    
    char depts[50][100];
    double cgpaSum[50];
    int count[50];
    int numDepts = 0;
    
    // Custom loop aggregation (no std::map)
    for (size_t i = 0; i < students.size(); ++i) {
        int foundIdx = -1;
        for (int j = 0; j < numDepts; ++j) {
            if (std::strcmp(depts[j], students[i].department) == 0) {
                foundIdx = j;
                break;
            }
        }
        
        if (foundIdx != -1) {
            cgpaSum[foundIdx] += students[i].cgpa;
            count[foundIdx]++;
        } else {
            if (numDepts < 50) {
                std::snprintf(depts[numDepts], sizeof(depts[numDepts]), "%s", students[i].department);
                cgpaSum[numDepts] = students[i].cgpa;
                count[numDepts] = 1;
                numDepts++;
            }
        }
    }
    
    std::cout << std::left << std::setw(30) << "Department"
              << std::setw(15) << "Student Count"
              << std::setw(15) << "Average CGPA" << "\n";
    std::cout << std::string(60, '-') << "\n";
    for (int i = 0; i < numDepts; ++i) {
        double avg = cgpaSum[i] / count[i];
        std::cout << std::left << std::setw(30) << depts[i]
                  << std::setw(15) << count[i]
                  << std::setw(15) << std::fixed << std::setprecision(2) << avg << "\n";
    }
    std::cout << std::string(60, '-') << "\n";
}

void generateCoursePerformanceReport() {
    std::cout << "\n--- Course-Wise Academic Statistics ---\n";
    std::vector<Enrollment> enrollments = loadEnrollments();
    std::vector<Course> courses = loadCourses();
    
    if (courses.empty()) {
        std::cout << "No courses available.\n";
        return;
    }
    
    std::cout << std::left << std::setw(12) << "Course Code"
              << std::setw(25) << "Instructor"
              << std::setw(12) << "Enrolled"
              << std::setw(12) << "Graded"
              << std::setw(15) << "Average Marks"
              << std::setw(12) << "Fail Count" << "\n";
    std::cout << std::string(88, '-') << "\n";
    
    for (size_t c = 0; c < courses.size(); ++c) {
        std::string code = courses[c].course_code;
        double marksSum = 0.0;
        int gradedCount = 0;
        int enrolledCount = 0;
        int failCount = 0;
        
        for (size_t e = 0; e < enrollments.size(); ++e) {
            if (std::string(enrollments[e].course_code) == code) {
                if (std::string(enrollments[e].status) == "active") {
                    enrolledCount++;
                }
                if (std::string(enrollments[e].grade) != "N/A" && std::string(enrollments[e].status) != "dropped") {
                    gradedCount++;
                    marksSum += enrollments[e].total_marks;
                    if (std::string(enrollments[e].grade) == "F") {
                        failCount++;
                    }
                }
            }
        }
        
        double avgMarks = 0.0;
        if (gradedCount > 0) {
            avgMarks = marksSum / gradedCount;
        }
        
        std::cout << std::left << std::setw(12) << courses[c].course_code
                  << std::setw(25) << courses[c].instructor
                  << std::setw(12) << enrolledCount
                  << std::setw(12) << gradedCount;
        if (gradedCount > 0) {
            std::cout << std::setw(15) << std::fixed << std::setprecision(2) << avgMarks;
        } else {
            std::cout << std::setw(15) << "N/A";
        }
        std::cout << std::setw(12) << failCount << "\n";
    }
    std::cout << std::string(88, '-') << "\n";
}

void generateAttendancePerformanceReport() {
    std::cout << "\n--- Attendance Low Threshold Alert (Below 75%) ---\n";
    std::vector<Enrollment> enrollments = loadEnrollments();
    std::vector<Student> students = loadStudents();
    
    std::cout << std::left << std::setw(15) << "Roll Number"
              << std::setw(25) << "Student Name"
              << std::setw(12) << "Course Code"
              << std::setw(15) << "Attendance %"
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(77, '-') << "\n";
    
    bool foundAny = false;
    for (size_t i = 0; i < enrollments.size(); ++i) {
        if (std::string(enrollments[i].status) == "active") {
            std::string roll = enrollments[i].roll_no;
            std::string ccode = enrollments[i].course_code;
            double attendanceRate = getAttendancePercentage(roll, ccode);
            if (attendanceRate < 75.0) {
                std::string name = "Unknown Student";
                int sIdx = findStudentIndex(students, roll);
                if (sIdx != -1) {
                    name = students[sIdx].name;
                }
                std::cout << std::left << std::setw(15) << roll
                          << std::setw(25) << name
                          << std::setw(12) << ccode
                          << std::setw(15) << std::fixed << std::setprecision(2) << attendanceRate
                          << std::setw(10) << "Warning" << "\n";
                foundAny = true;
            }
        }
    }
    std::cout << std::string(77, '-') << "\n";
    if (!foundAny) {
        std::cout << "All active enrollments meet the 75% attendance threshold.\n";
    }
}

void generateFinancialHealthReport() {
    std::cout << "\n--- Campus Financial Aggregates ---\n";
    std::vector<Fee> fees = loadFees();
    if (fees.empty()) {
        std::cout << "No fee ledger history found.\n";
        return;
    }
    
    double totalExpected = 0.0;
    double totalCollected = 0.0;
    int totalChallans = 0;
    int paidFull = 0;
    int paidLate = 0;
    int partial = 0;
    int unpaid = 0;
    
    for (size_t i = 0; i < fees.size(); ++i) {
        totalExpected += fees[i].total_fee;
        totalCollected += fees[i].amount_paid;
        totalChallans++;
        
        std::string st = fees[i].status;
        if (st == "paid") paidFull++;
        else if (st == "paid_late") {
            paidFull++;
            paidLate++;
        }
        else if (st == "partial") partial++;
        else if (st == "unpaid") unpaid++;
    }
    
    double outstanding = totalExpected - totalCollected;
    double efficiency = (totalExpected > 0) ? (totalCollected / totalExpected) * 100.0 : 0.0;
    
    std::cout << "-------------------------------------------\n";
    std::cout << " Total Ledger Accounts : " << totalChallans << "\n";
    std::cout << " Total Expected Revenue: " << std::fixed << std::setprecision(2) << totalExpected << " PKR\n";
    std::cout << " Total Collected Fees  : " << totalCollected << " PKR\n";
    std::cout << " Outstanding Arrears   : " << outstanding << " PKR\n";
    std::cout << " Collection Efficiency : " << efficiency << "%\n";
    std::cout << "-------------------------------------------\n";
    std::cout << " Fully Settled Accounts: " << paidFull << "\n";
    std::cout << "   - Paid on Time      : " << (paidFull - paidLate) << "\n";
    std::cout << "   - Paid Late Penalty : " << paidLate << "\n";
    std::cout << " Partially Paid Accounts: " << partial << "\n";
    std::cout << " Unpaid Accounts       : " << unpaid << "\n";
    std::cout << "-------------------------------------------\n";
}

void generateTopPerformersReport() {
    std::cout << "\n--- Top Academic Performers (Top 5 Students) ---\n";
    std::vector<Student> students = loadStudents();
    if (students.empty()) {
        std::cout << "No student records found.\n";
        return;
    }
    
    // Sort array using manual selection sort (no std::sort)
    for (size_t i = 0; i < students.size() - 1; ++i) {
        size_t maxIdx = i;
        for (size_t j = i + 1; j < students.size(); ++j) {
            if (students[j].cgpa > students[maxIdx].cgpa) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            Student temp = students[i];
            students[i] = students[maxIdx];
            students[maxIdx] = temp;
        }
    }
    
    std::cout << std::left << std::setw(8) << "Rank"
              << std::setw(15) << "Roll No"
              << std::setw(25) << "Student Name"
              << std::setw(25) << "Department"
              << std::setw(10) << "CGPA" << "\n";
    std::cout << std::string(83, '-') << "\n";
    
    int rank = 1;
    for (size_t i = 0; i < students.size() && i < 5; ++i) {
        std::cout << std::left << std::setw(8) << rank
                  << std::setw(15) << students[i].roll_no
                  << std::setw(25) << students[i].name
                  << std::setw(25) << students[i].department
                  << std::setw(10) << std::fixed << std::setprecision(2) << students[i].cgpa << "\n";
        rank++;
    }
    std::cout << std::string(83, '-') << "\n";
}
