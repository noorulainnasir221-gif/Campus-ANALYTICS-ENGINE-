#include "attendance.h"
#include "filehandler.h"
#include "course_ops.h"
#include "student_ops.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
using namespace std;


std::vector<Attendance> loadAttendance() {
    std::vector<Attendance> logs;
    std::vector<std::vector<std::string>> data = readTXT("attendance_log.txt");
    if (data.empty()) return logs;

    // Line 0 is header: log_id,roll_no,course_code,session_date,status
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i].size() < 5) continue;
        Attendance a;
        std::snprintf(a.log_id, sizeof(a.log_id), "%s", data[i][0].c_str());
        std::snprintf(a.roll_no, sizeof(a.roll_no), "%s", data[i][1].c_str());
        std::snprintf(a.course_code, sizeof(a.course_code), "%s", data[i][2].c_str());
        std::snprintf(a.session_date, sizeof(a.session_date), "%s", data[i][3].c_str());
        std::snprintf(a.status, sizeof(a.status), "%s", data[i][4].c_str());
        logs.push_back(a);
    }
    return logs;
}

bool saveAttendance(const std::vector<Attendance>& logs) {
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> header = {"log_id", "roll_no", "course_code", "session_date", "status"};
    data.push_back(header);

    for (size_t i = 0; i < logs.size(); ++i) {
        std::vector<std::string> row;
        row.push_back(logs[i].log_id);
        row.push_back(logs[i].roll_no);
        row.push_back(logs[i].course_code);
        row.push_back(logs[i].session_date);
        row.push_back(logs[i].status);
        data.push_back(row);
    }
    return writeTXT("attendance_log.txt", data);
}

double getAttendancePercentage(const std::string& roll_no, const std::string& course_code) {
    std::vector<Attendance> logs = loadAttendance();
    int total = 0;
    double score = 0.0;
    for (size_t i = 0; i < logs.size(); ++i) {
        if (std::string(logs[i].roll_no) == roll_no && std::string(logs[i].course_code) == course_code) {
            total++;
            std::string st = logs[i].status;
            if (st == "P") {
                score += 1.0;
            } else if (st == "L") {
                score += 0.5;
            }
        }
    }
    if (total == 0) return 100.0; // No logs means 100% (or ungraded/new)
    return (score / total) * 100.0;
}

std::string generateLogId(const std::vector<Attendance>& logs) {
    int maxId = 0;
    for (size_t i = 0; i < logs.size(); ++i) {
        std::string idStr = logs[i].log_id;
        if (idStr.length() > 1 && idStr[0] == 'L') {
            try {
                int val = std::stoi(idStr.substr(1));
                if (val > maxId) {
                    maxId = val;
                }
            } catch (...) {}
        }
    }
    char buf[20];
    std::snprintf(buf, sizeof(buf), "L%05d", maxId + 1);
    return std::string(buf);
}

void markAttendanceInteractive() {
    std::cout << "\n--- Mark Attendance Session ---\n";
    std::string course_code, session_date;
    std::cout << "Enter Course Code: ";
    std::cin >> course_code;
    
    // Validate Course exists
    std::vector<Course> courses = loadCourses();
    if (findCourseIndex(courses, course_code) == -1) {
        std::cout << "[Error] Course " << course_code << " does not exist.\n";
        return;
    }

    std::cout << "Enter Session Date (DD-MM-YYYY, e.g. 26-06-2026): ";
    std::cin >> session_date;
    std::cin.ignore(10000, '\n');

    int day, month, year;
    if (!parseDate(session_date, day, month, year)) {
        std::cout << "[Error] Invalid date format. Must be DD-MM-YYYY.\n";
        return;
    }

    // Load active enrollments for this course
    std::vector<Enrollment> enrollments = loadEnrollments();
    std::vector<Enrollment> enrolledInCourse;
    for (size_t i = 0; i < enrollments.size(); ++i) {
        if (std::string(enrollments[i].course_code) == course_code && 
            std::string(enrollments[i].status) == "active") {
            enrolledInCourse.push_back(enrollments[i]);
        }
    }

    if (enrolledInCourse.empty()) {
        std::cout << "[Info] No active students are enrolled in this course.\n";
        return;
    }

    std::vector<Student> students = loadStudents();
    std::vector<Attendance> logs = loadAttendance();

    std::cout << "\nMarking attendance for " << enrolledInCourse.size() << " student(s).\n";
    std::cout << "Enter P for Present, A for Absent, L for Late:\n\n";

    for (size_t i = 0; i < enrolledInCourse.size(); ++i) {
        std::string roll_no = enrolledInCourse[i].roll_no;
        std::string name = "Unknown Student";
        int sIdx = findStudentIndex(students, roll_no);
        if (sIdx != -1) {
            name = students[sIdx].name;
        }

        std::string status = "";
        while (true) {
            std::cout << name << " (" << roll_no << "): ";
            std::cin >> status;
            if (status == "P" || status == "A" || status == "L") {
                break;
            }
            std::cout << "Invalid status! Enter P, A, or L.\n";
        }

        Attendance log;
        std::string logId = generateLogId(logs);
        std::snprintf(log.log_id, sizeof(log.log_id), "%s", logId.c_str());
        std::snprintf(log.roll_no, sizeof(log.roll_no), "%s", roll_no.c_str());
        std::snprintf(log.course_code, sizeof(log.course_code), "%s", course_code.c_str());
        std::snprintf(log.session_date, sizeof(log.session_date), "%s", session_date.c_str());
        std::snprintf(log.status, sizeof(log.status), "%s", status.c_str());

        logs.push_back(log);
    }
    std::cin.ignore(10000, '\n');

    if (saveAttendance(logs)) {
        std::cout << "\n[Success] Attendance session successfully logged!\n";
    } else {
        std::cout << "\n[Error] Failed to save attendance log.\n";
    }
}

void viewAttendanceReportInteractive() {
    std::cout << "\n--- Student Attendance Report ---\n";
    std::string roll_no, course_code;
    std::cout << "Enter Student Roll Number: ";
    std::cin >> roll_no;
    std::cout << "Enter Course Code: ";
    std::cin >> course_code;
    std::cin.ignore(10000, '\n');

    std::vector<Student> students = loadStudents();
    int sIdx = findStudentIndex(students, roll_no);
    if (sIdx == -1) {
        std::cout << "[Error] Student not found.\n";
        return;
    }

    std::vector<Course> courses = loadCourses();
    int cIdx = findCourseIndex(courses, course_code);
    if (cIdx == -1) {
        std::cout << "[Error] Course not found.\n";
        return;
    }

    std::vector<Attendance> logs = loadAttendance();
    int total = 0, present = 0, absent = 0, late = 0;
    
    std::cout << "\nAttendance Log Details for " << students[sIdx].name << " in " << course_code << ":\n";
    std::cout << std::left << std::setw(15) << "Date" << std::setw(10) << "Status" << "\n";
    std::cout << std::string(25, '-') << "\n";

    for (size_t i = 0; i < logs.size(); ++i) {
        if (std::string(logs[i].roll_no) == roll_no && std::string(logs[i].course_code) == course_code) {
            total++;
            std::string st = logs[i].status;
            std::cout << std::left << std::setw(15) << logs[i].session_date << std::setw(10) << st << "\n";
            if (st == "P") present++;
            else if (st == "A") absent++;
            else if (st == "L") late++;
        }
    }
    std::cout << std::string(25, '-') << "\n";

    if (total == 0) {
        std::cout << "No attendance records found for this student in this course.\n";
        return;
    }

    double rate = ((present + 0.5 * late) / total) * 100.0;
    std::cout << "Total Sessions : " << total << "\n";
    std::cout << "Present Count  : " << present << "\n";
    std::cout << "Absent Count   : " << absent << "\n";
    std::cout << "Late Count     : " << late << "\n";
    std::cout << "Attendance Rate: " << std::fixed << std::setprecision(2) << rate << "%\n";
    if (rate < 75.0) {
        std::cout << "[Warning] Attendance is below 75%! Grade penalties will be applied.\n";
    }
}
