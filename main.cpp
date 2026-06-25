#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include "reports.h"
using namespace std;


// Clear standard input streams to handle invalid characters
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void studentMenu() {
    while (true) {
        std::cout << "\n============================================\n";
        std::cout << "         Student Records Management         \n";
        std::cout << "============================================\n";
        std::cout << " 1. Add New Student Record\n";
        std::cout << " 2. View Student Details\n";
        std::cout << " 3. Update Student Record\n";
        std::cout << " 4. Delete/Deactivate Student Record\n";
        std::cout << " 5. List Student Directory\n";
        std::cout << " 0. Return to Main Menu\n";
        std::cout << "============================================\n";
        std::cout << "Enter your choice: ";
        
        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }
        std::cin.ignore(10000, '\n');

        switch (choice) {
            case 1: addStudentInteractive(); break;
            case 2: viewStudentInteractive(); break;
            case 3: updateStudentInteractive(); break;
            case 4: deleteStudentInteractive(); break;
            case 5: listStudentsInteractive(); break;
            case 0: return;
            default: std::cout << "Invalid choice. Select from 0-5.\n";
        }
    }
}

void courseMenu() {
    while (true) {
        std::cout << "\n============================================\n";
        std::cout << "        Course & Enrollment Management      \n";
        std::cout << "============================================\n";
        std::cout << " 1. Create New Course\n";
        std::cout << " 2. Enroll Student in Course\n";
        std::cout << " 3. Drop/Withdraw Course Enrollment\n";
        std::cout << " 4. List Course Catalog\n";
        std::cout << " 5. List Student Enrollment Logs\n";
        std::cout << " 0. Return to Main Menu\n";
        std::cout << "============================================\n";
        std::cout << "Enter your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }
        std::cin.ignore(10000, '\n');

        switch (choice) {
            case 1: addCourseInteractive(); break;
            case 2: enrollStudentInteractive(); break;
            case 3: dropCourseInteractive(); break;
            case 4: listCoursesInteractive(); break;
            case 5: listEnrollmentsInteractive(); break;
            case 0: return;
            default: std::cout << "Invalid choice. Select from 0-5.\n";
        }
    }
}

void attendanceMenu() {
    while (true) {
        std::cout << "\n============================================\n";
        std::cout << "         Attendance Tracking System         \n";
        std::cout << "============================================\n";
        std::cout << " 1. Mark Session Attendance\n";
        std::cout << " 2. View Student Attendance Report\n";
        std::cout << " 0. Return to Main Menu\n";
        std::cout << "============================================\n";
        std::cout << "Enter your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }
        std::cin.ignore(10000, '\n');

        switch (choice) {
            case 1: markAttendanceInteractive(); break;
            case 2: viewAttendanceReportInteractive(); break;
            case 0: return;
            default: std::cout << "Invalid choice. Select from 0-2.\n";
        }
    }
}

void gradeMenu() {
    while (true) {
        std::cout << "\n============================================\n";
        std::cout << "       Academic Grading & GPA Processing    \n";
        std::cout << "============================================\n";
        std::cout << " 1. Enter/Update Marks & Grades\n";
        std::cout << " 2. View Student Transcript\n";
        std::cout << " 0. Return to Main Menu\n";
        std::cout << "============================================\n";
        std::cout << "Enter your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }
        std::cin.ignore(10000, '\n');

        switch (choice) {
            case 1: addOrUpdateGradeInteractive(); break;
            case 2: viewStudentGradesInteractive(); break;
            case 0: return;
            default: std::cout << "Invalid choice. Select from 0-2.\n";
        }
    }
}

void feeMenu() {
    while (true) {
        std::cout << "\n============================================\n";
        std::cout << "         Financial Ledger & Fees            \n";
        std::cout << "============================================\n";
        std::cout << " 1. Generate Semester Fee Challan\n";
        std::cout << " 2. Record Payment Receipt\n";
        std::cout << " 3. View Student Ledger Details\n";
        std::cout << " 4. View Outstanding Defaulters List\n";
        std::cout << " 0. Return to Main Menu\n";
        std::cout << "============================================\n";
        std::cout << "Enter your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }
        std::cin.ignore(10000, '\n');

        switch (choice) {
            case 1: generateFeeChallanInteractive(); break;
            case 2: recordPaymentInteractive(); break;
            case 3: viewStudentFeeStatusInteractive(); break;
            case 4: listDefaultersInteractive(); break;
            case 0: return;
            default: std::cout << "Invalid choice. Select from 0-4.\n";
        }
    }
}

void reportsMenu() {
    while (true) {
        std::cout << "\n============================================\n";
        std::cout << "       Aggregated Reports & Analytics       \n";
        std::cout << "============================================\n";
        std::cout << " 1. Department Performance Report\n";
        std::cout << " 2. Course-wise Academic Statistics\n";
        std::cout << " 3. Low Attendance Warning Flag List\n";
        std::cout << " 4. Financial Health & Revenue Report\n";
        std::cout << " 5. Top 5 Campus Performers List\n";
        std::cout << " 0. Return to Main Menu\n";
        std::cout << "============================================\n";
        std::cout << "Enter your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }
        std::cin.ignore(10000, '\n');

        switch (choice) {
            case 1: generateAcademicPerformanceReport(); break;
            case 2: generateCoursePerformanceReport(); break;
            case 3: generateAttendancePerformanceReport(); break;
            case 4: generateFinancialHealthReport(); break;
            case 5: generateTopPerformersReport(); break;
            case 0: return;
            default: std::cout << "Invalid choice. Select from 0-5.\n";
        }
    }
}

int main() {
    while (true) {
        std::cout << "\n============================================\n";
        std::cout << "        CAMPUS ANALYTICS ENGINE v1.0        \n";
        std::cout << "      Procedural Data Analytics System      \n";
        std::cout << "============================================\n";
        std::cout << " 1. Student Directory Management\n";
        std::cout << " 2. Course & Enrollment Management\n";
        std::cout << " 3. Attendance Tracking System\n";
        std::cout << " 4. Academic Grading & GPA Processing\n";
        std::cout << " 5. Financial Ledger & Fee Tracking\n";
        std::cout << " 6. Aggregated Reports & Analytics Dashboard\n";
        std::cout << " 0. Exit System\n";
        std::cout << "============================================\n";
        std::cout << "Enter your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (choice == 0) {
            std::cout << "\nThank you for using Campus Analytics Engine. Exiting...\n";
            break;
        }

        switch (choice) {
            case 1: studentMenu(); break;
            case 2: courseMenu(); break;
            case 3: attendanceMenu(); break;
            case 4: gradeMenu(); break;
            case 5: feeMenu(); break;
            case 6: reportsMenu(); break;
            default: std::cout << "Invalid option. Select from 0-6.\n";
        }
    }
    return 0;
}
