#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
using namespace std;


std::vector<Student> loadStudents() {
    std::vector<Student> students;
    std::vector<std::vector<std::string>> data = readTXT("students.txt");
    if (data.empty()) return students;

    // Line 0 is header: roll_no,name,department,semester,cgpa,status
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i].size() < 6) continue;
        Student s;
        std::snprintf(s.roll_no, sizeof(s.roll_no), "%s", data[i][0].c_str());
        std::snprintf(s.name, sizeof(s.name), "%s", data[i][1].c_str());
        std::snprintf(s.department, sizeof(s.department), "%s", data[i][2].c_str());
        try {
            s.semester = std::stoi(data[i][3]);
        } catch (...) {
            s.semester = 1;
        }
        try {
            s.cgpa = std::stod(data[i][4]);
        } catch (...) {
            s.cgpa = 0.0;
        }
        std::snprintf(s.status, sizeof(s.status), "%s", data[i][5].c_str());
        students.push_back(s);
    }
    return students;
}

bool saveStudents(const std::vector<Student>& students) {
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> header = {"roll_no", "name", "department", "semester", "cgpa", "status"};
    data.push_back(header);

    for (size_t i = 0; i < students.size(); ++i) {
        std::vector<std::string> row;
        row.push_back(students[i].roll_no);
        row.push_back(students[i].name);
        row.push_back(students[i].department);
        row.push_back(std::to_string(students[i].semester));
        
        char cgpaStr[20];
        std::snprintf(cgpaStr, sizeof(cgpaStr), "%.2f", students[i].cgpa);
        row.push_back(cgpaStr);
        
        row.push_back(students[i].status);
        data.push_back(row);
    }
    return writeTXT("students.txt", data);
}

int findStudentIndex(const std::vector<Student>& students, const std::string& roll_no) {
    for (size_t i = 0; i < students.size(); ++i) {
        if (std::string(students[i].roll_no) == roll_no) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void addStudentInteractive() {
    std::cout << "\n--- Add New Student Record ---\n";
    std::vector<Student> students = loadStudents();

    std::string roll_no, name, dept;
    int semester;
    double cgpa = 0.0;
    std::string status = "active";

    std::cout << "Enter Roll Number (e.g. BSAI-23-030): ";
    std::cin >> roll_no;
    std::cin.ignore(10000, '\n'); // clear buffer

    if (findStudentIndex(students, roll_no) != -1) {
        std::cout << "[Error] Student with Roll Number " << roll_no << " already exists!\n";
        return;
    }

    std::cout << "Enter Full Name: ";
    std::getline(std::cin, name);
    
    std::cout << "Enter Department Name: ";
    std::getline(std::cin, dept);

    std::cout << "Enter Current Semester: ";
    while (!(std::cin >> semester) || semester < 1) {
        std::cout << "Invalid input. Enter positive integer: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cin.ignore(10000, '\n');

    std::cout << "Enter initial CGPA (default 0.0): ";
    std::cin >> cgpa;
    if (std::cin.fail() || cgpa < 0.0 || cgpa > 4.0) {
        cgpa = 0.0;
    }
    std::cin.ignore(10000, '\n');

    Student newStudent;
    std::snprintf(newStudent.roll_no, sizeof(newStudent.roll_no), "%s", roll_no.c_str());
    std::snprintf(newStudent.name, sizeof(newStudent.name), "%s", name.c_str());
    std::snprintf(newStudent.department, sizeof(newStudent.department), "%s", dept.c_str());
    newStudent.semester = semester;
    newStudent.cgpa = cgpa;
    std::snprintf(newStudent.status, sizeof(newStudent.status), "%s", status.c_str());

    students.push_back(newStudent);
    if (saveStudents(students)) {
        std::cout << "[Success] Student added successfully!\n";
    } else {
        std::cout << "[Error] Failed to write changes to file.\n";
    }
}

void viewStudentInteractive() {
    std::cout << "\n--- View Student Details ---\n";
    std::vector<Student> students = loadStudents();

    std::string roll_no;
    std::cout << "Enter Student Roll Number: ";
    std::cin >> roll_no;
    std::cin.ignore(10000, '\n');

    int idx = findStudentIndex(students, roll_no);
    if (idx == -1) {
        std::cout << "[Error] Student not found.\n";
        return;
    }

    Student s = students[idx];
    std::cout << "\n========================================\n";
    std::cout << " Roll Number : " << s.roll_no << "\n";
    std::cout << " Name        : " << s.name << "\n";
    std::cout << " Department  : " << s.department << "\n";
    std::cout << " Semester    : " << s.semester << "\n";
    std::cout << " CGPA        : " << std::fixed << std::setprecision(2) << s.cgpa << "\n";
    std::cout << " Status      : " << s.status << "\n";
    std::cout << "========================================\n";
}

void updateStudentInteractive() {
    std::cout << "\n--- Update Student Record ---\n";
    std::vector<Student> students = loadStudents();

    std::string roll_no;
    std::cout << "Enter Roll Number of Student to Update: ";
    std::cin >> roll_no;
    std::cin.ignore(10000, '\n');

    int idx = findStudentIndex(students, roll_no);
    if (idx == -1) {
        std::cout << "[Error] Student not found.\n";
        return;
    }

    Student& s = students[idx];
    std::string temp;

    std::cout << "Current Name: " << s.name << "\nEnter New Name (leave blank to skip): ";
    std::getline(std::cin, temp);
    if (!temp.empty()) {
        std::snprintf(s.name, sizeof(s.name), "%s", temp.c_str());
    }

    std::cout << "Current Department: " << s.department << "\nEnter New Department (leave blank to skip): ";
    std::getline(std::cin, temp);
    if (!temp.empty()) {
        std::snprintf(s.department, sizeof(s.department), "%s", temp.c_str());
    }

    std::cout << "Current Semester: " << s.semester << "\nEnter New Semester (0 to skip): ";
    int sem;
    if (std::cin >> sem && sem > 0) {
        s.semester = sem;
    }
    std::cin.clear();
    std::cin.ignore(10000, '\n');

    std::cout << "Current Status: " << s.status << "\nEnter New Status (active/inactive, blank to skip): ";
    std::getline(std::cin, temp);
    if (!temp.empty()) {
        std::snprintf(s.status, sizeof(s.status), "%s", temp.c_str());
    }

    if (saveStudents(students)) {
        std::cout << "[Success] Student record updated successfully!\n";
    } else {
        std::cout << "[Error] Failed to write changes.\n";
    }
}

void deleteStudentInteractive() {
    std::cout << "\n--- Delete/Deactivate Student Record ---\n";
    std::vector<Student> students = loadStudents();

    std::string roll_no;
    std::cout << "Enter Roll Number of Student to Delete: ";
    std::cin >> roll_no;
    std::cin.ignore(10000, '\n');

    int idx = findStudentIndex(students, roll_no);
    if (idx == -1) {
        std::cout << "[Error] Student not found.\n";
        return;
    }

    std::cout << "Student found: " << students[idx].name << "\n";
    std::cout << "1. Mark as inactive (Recommended to preserve enrollment history)\n";
    std::cout << "2. Delete permanently from database\n";
    std::cout << "Choose option: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(10000, '\n');

    if (choice == 1) {
        std::snprintf(students[idx].status, sizeof(students[idx].status), "inactive");
        std::cout << "[Info] Status set to inactive.\n";
    } else if (choice == 2) {
        students.erase(students.begin() + idx);
        std::cout << "[Info] Record removed from collection.\n";
    } else {
        std::cout << "[Error] Invalid selection. Aborted deletion.\n";
        return;
    }

    if (saveStudents(students)) {
        std::cout << "[Success] Database updated successfully!\n";
    } else {
        std::cout << "[Error] Failed to write updates.\n";
    }
}

void listStudentsInteractive() {
    std::cout << "\n--- Student Directory ---\n";
    std::vector<Student> students = loadStudents();

    if (students.empty()) {
        std::cout << "No student records available.\n";
        return;
    }

    std::cout << std::left << std::setw(15) << "Roll No" 
              << std::setw(25) << "Name" 
              << std::setw(25) << "Department" 
              << std::setw(10) << "Semester" 
              << std::setw(8) << "CGPA" 
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(93, '-') << "\n";

    for (size_t i = 0; i < students.size(); ++i) {
        std::cout << std::left << std::setw(15) << students[i].roll_no
                  << std::setw(25) << students[i].name
                  << std::setw(25) << students[i].department
                  << std::setw(10) << students[i].semester
                  << std::setw(8) << std::fixed << std::setprecision(2) << students[i].cgpa
                  << std::setw(10) << students[i].status << "\n";
    }
    std::cout << std::string(93, '-') << "\n";
}
