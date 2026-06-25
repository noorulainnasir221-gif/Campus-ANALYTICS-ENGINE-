#include "course_ops.h"
#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
using namespace std;


std::vector<Course> loadCourses() {
    std::vector<Course> courses;
    std::vector<std::vector<std::string>> data = readTXT("courses.txt");
    if (data.empty()) return courses;

    // Line 0 is header: course_code,course_name,credit_hours,instructor,capacity,enrolled,prerequisite
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i].size() < 7) continue;
        Course c;
        std::snprintf(c.course_code, sizeof(c.course_code), "%s", data[i][0].c_str());
        std::snprintf(c.course_name, sizeof(c.course_name), "%s", data[i][1].c_str());
        try {
            c.credit_hours = std::stoi(data[i][2]);
        } catch (...) {
            c.credit_hours = 3;
        }
        std::snprintf(c.instructor, sizeof(c.instructor), "%s", data[i][3].c_str());
        try {
            c.capacity = std::stoi(data[i][4]);
        } catch (...) {
            c.capacity = 40;
        }
        try {
            c.enrolled = std::stoi(data[i][5]);
        } catch (...) {
            c.enrolled = 0;
        }
        std::snprintf(c.prerequisite, sizeof(c.prerequisite), "%s", data[i][6].c_str());
        courses.push_back(c);
    }
    return courses;
}

bool saveCourses(const std::vector<Course>& courses) {
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> header = {"course_code", "course_name", "credit_hours", "instructor", "capacity", "enrolled", "prerequisite"};
    data.push_back(header);

    for (size_t i = 0; i < courses.size(); ++i) {
        std::vector<std::string> row;
        row.push_back(courses[i].course_code);
        row.push_back(courses[i].course_name);
        row.push_back(std::to_string(courses[i].credit_hours));
        row.push_back(courses[i].instructor);
        row.push_back(std::to_string(courses[i].capacity));
        row.push_back(std::to_string(courses[i].enrolled));
        row.push_back(courses[i].prerequisite);
        data.push_back(row);
    }
    return writeTXT("courses.txt", data);
}

std::vector<Enrollment> loadEnrollments() {
    std::vector<Enrollment> enrollments;
    std::vector<std::vector<std::string>> data = readTXT("enrollments.txt");
    if (data.empty()) return enrollments;

    // Line 0 is header: enrollment_id,roll_no,course_code,semester,enrollment_date,status,...
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i].size() < 6) continue;
        Enrollment e;
        std::snprintf(e.enrollment_id, sizeof(e.enrollment_id), "%s", data[i][0].c_str());
        std::snprintf(e.roll_no, sizeof(e.roll_no), "%s", data[i][1].c_str());
        std::snprintf(e.course_code, sizeof(e.course_code), "%s", data[i][2].c_str());
        try {
            e.semester = std::stoi(data[i][3]);
        } catch (...) {
            e.semester = 1;
        }
        std::snprintf(e.enrollment_date, sizeof(e.enrollment_date), "%s", data[i][4].c_str());
        std::snprintf(e.status, sizeof(e.status), "%s", data[i][5].c_str());

        // Parse grades if present (column index 6 to 12)
        if (data[i].size() >= 13) {
            try {
                e.assignment = std::stod(data[i][6]);
                e.quiz = std::stod(data[i][7]);
                e.mid = std::stod(data[i][8]);
                e.project = std::stod(data[i][9]);
                e.final_exam = std::stod(data[i][10]);
                e.total_marks = std::stod(data[i][11]);
            } catch (...) {
                e.assignment = e.quiz = e.mid = e.project = e.final_exam = e.total_marks = -1.0;
            }
            std::snprintf(e.grade, sizeof(e.grade), "%s", data[i][12].c_str());
        } else {
            e.assignment = e.quiz = e.mid = e.project = e.final_exam = e.total_marks = -1.0;
            std::snprintf(e.grade, sizeof(e.grade), "N/A");
        }
        enrollments.push_back(e);
    }
    return enrollments;
}

bool saveEnrollments(const std::vector<Enrollment>& enrollments) {
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> header = {
        "enrollment_id", "roll_no", "course_code", "semester", "enrollment_date", "status",
        "assignment", "quiz", "mid", "project", "final", "total", "grade"
    };
    data.push_back(header);

    for (size_t i = 0; i < enrollments.size(); ++i) {
        std::vector<std::string> row;
        row.push_back(enrollments[i].enrollment_id);
        row.push_back(enrollments[i].roll_no);
        row.push_back(enrollments[i].course_code);
        row.push_back(std::to_string(enrollments[i].semester));
        row.push_back(enrollments[i].enrollment_date);
        row.push_back(enrollments[i].status);

        char a[20], q[20], m[20], p[20], f[20], t[20];
        std::snprintf(a, sizeof(a), "%.2f", enrollments[i].assignment);
        std::snprintf(q, sizeof(q), "%.2f", enrollments[i].quiz);
        std::snprintf(m, sizeof(m), "%.2f", enrollments[i].mid);
        std::snprintf(p, sizeof(p), "%.2f", enrollments[i].project);
        std::snprintf(f, sizeof(f), "%.2f", enrollments[i].final_exam);
        std::snprintf(t, sizeof(t), "%.2f", enrollments[i].total_marks);

        row.push_back(a);
        row.push_back(q);
        row.push_back(m);
        row.push_back(p);
        row.push_back(f);
        row.push_back(t);
        row.push_back(enrollments[i].grade);

        data.push_back(row);
    }
    return writeTXT("enrollments.txt", data);
}

int findCourseIndex(const std::vector<Course>& courses, const std::string& course_code) {
    for (size_t i = 0; i < courses.size(); ++i) {
        if (std::string(courses[i].course_code) == course_code) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int findEnrollmentIndex(const std::vector<Enrollment>& enrollments, const std::string& roll_no, const std::string& course_code) {
    for (size_t i = 0; i < enrollments.size(); ++i) {
        if (std::string(enrollments[i].roll_no) == roll_no && 
            std::string(enrollments[i].course_code) == course_code) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Generate new enrollment id like E0126
std::string generateEnrollmentId(const std::vector<Enrollment>& enrollments) {
    int maxId = 0;
    for (size_t i = 0; i < enrollments.size(); ++i) {
        std::string idStr = enrollments[i].enrollment_id;
        if (idStr.length() > 1 && idStr[0] == 'E') {
            try {
                int val = std::stoi(idStr.substr(1));
                if (val > maxId) {
                    maxId = val;
                }
            } catch (...) {}
        }
    }
    char buf[20];
    std::snprintf(buf, sizeof(buf), "E%04d", maxId + 1);
    return std::string(buf);
}

// Custom prerequisite validation
bool isPrereqSatisfied(const std::string& roll_no, const std::string& prereq, const std::vector<Enrollment>& enrollments) {
    if (prereq == "NONE" || prereq.empty()) return true;

    for (size_t i = 0; i < enrollments.size(); ++i) {
        if (std::string(enrollments[i].roll_no) == roll_no && 
            std::string(enrollments[i].course_code) == prereq) {
            
            // Student must be active/completed in the prereq course and not failed (grade is not F)
            if (std::string(enrollments[i].status) != "dropped") {
                if (std::string(enrollments[i].grade) != "F") {
                    return true;
                }
            }
        }
    }
    return false;
}

void addCourseInteractive() {
    std::cout << "\n--- Add New Course ---\n";
    std::vector<Course> courses = loadCourses();

    std::string code, name, instructor, prereq;
    int credits, capacity;

    std::cout << "Enter Course Code (e.g. CS201): ";
    std::cin >> code;
    std::cin.ignore(10000, '\n');

    if (findCourseIndex(courses, code) != -1) {
        std::cout << "[Error] Course " << code << " already exists!\n";
        return;
    }

    std::cout << "Enter Course Name: ";
    std::getline(std::cin, name);

    std::cout << "Enter Credit Hours: ";
    while (!(std::cin >> credits) || credits < 1 || credits > 6) {
        std::cout << "Invalid. Enter credit hours (1-6): ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cin.ignore(10000, '\n');

    std::cout << "Enter Instructor Name: ";
    std::getline(std::cin, instructor);

    std::cout << "Enter Classroom Capacity: ";
    while (!(std::cin >> capacity) || capacity < 1) {
        std::cout << "Invalid capacity. Enter positive integer: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cin.ignore(10000, '\n');

    std::cout << "Enter Prerequisite Course Code (or NONE): ";
    std::cin >> prereq;
    std::cin.ignore(10000, '\n');

    Course c;
    std::snprintf(c.course_code, sizeof(c.course_code), "%s", code.c_str());
    std::snprintf(c.course_name, sizeof(c.course_name), "%s", name.c_str());
    c.credit_hours = credits;
    std::snprintf(c.instructor, sizeof(c.instructor), "%s", instructor.c_str());
    c.capacity = capacity;
    c.enrolled = 0;
    std::snprintf(c.prerequisite, sizeof(c.prerequisite), "%s", prereq.c_str());

    courses.push_back(c);
    if (saveCourses(courses)) {
        std::cout << "[Success] Course " << code << " added successfully!\n";
    } else {
        std::cout << "[Error] Failed to write course data.\n";
    }
}

void listCoursesInteractive() {
    std::cout << "\n--- Course Catalog ---\n";
    std::vector<Course> courses = loadCourses();

    if (courses.empty()) {
        std::cout << "No courses available.\n";
        return;
    }

    std::cout << std::left << std::setw(12) << "Code"
              << std::setw(30) << "Course Name"
              << std::setw(10) << "Credits"
              << std::setw(20) << "Instructor"
              << std::setw(12) << "Capacity"
              << std::setw(10) << "Enrolled"
              << std::setw(15) << "Prerequisite" << "\n";
    std::cout << std::string(109, '-') << "\n";

    for (size_t i = 0; i < courses.size(); ++i) {
        std::cout << std::left << std::setw(12) << courses[i].course_code
                  << std::setw(30) << courses[i].course_name
                  << std::setw(10) << courses[i].credit_hours
                  << std::setw(20) << courses[i].instructor
                  << std::setw(12) << courses[i].capacity
                  << std::setw(10) << courses[i].enrolled
                  << std::setw(15) << courses[i].prerequisite << "\n";
    }
    std::cout << std::string(109, '-') << "\n";
}

void enrollStudentInteractive() {
    std::cout << "\n--- Course Enrollment ---\n";
    
    std::string roll_no, course_code;
    std::cout << "Enter Student Roll Number: ";
    std::cin >> roll_no;
    std::cout << "Enter Course Code: ";
    std::cin >> course_code;
    std::cin.ignore(10000, '\n');

    // 1. Verify student exists and is active
    std::vector<Student> students = loadStudents();
    int sIdx = findStudentIndex(students, roll_no);
    if (sIdx == -1) {
        std::cout << "[Error] Student not found.\n";
        return;
    }
    if (std::string(students[sIdx].status) != "active") {
        std::cout << "[Error] Student is currently inactive. Cannot enroll.\n";
        return;
    }

    // 2. Verify course exists and has slots
    std::vector<Course> courses = loadCourses();
    int cIdx = findCourseIndex(courses, course_code);
    if (cIdx == -1) {
        std::cout << "[Error] Course " << course_code << " does not exist.\n";
        return;
    }
    if (courses[cIdx].enrolled >= courses[cIdx].capacity) {
        std::cout << "[Error] Course capacity reached! (" << courses[cIdx].enrolled << "/" << courses[cIdx].capacity << ")\n";
        return;
    }

    // 3. Verify student is not already active in this course
    std::vector<Enrollment> enrollments = loadEnrollments();
    int eIdx = findEnrollmentIndex(enrollments, roll_no, course_code);
    if (eIdx != -1 && std::string(enrollments[eIdx].status) == "active") {
        std::cout << "[Error] Student is already enrolled in " << course_code << ".\n";
        return;
    }

    // 4. Verify prerequisite course
    std::string prereq = courses[cIdx].prerequisite;
    if (!isPrereqSatisfied(roll_no, prereq, enrollments)) {
        std::cout << "[Error] Prerequisite not satisfied! Student must pass " << prereq << " first.\n";
        return;
    }

    // Generate enrollment ID and setup record
    std::string eId = generateEnrollmentId(enrollments);
    std::string today = "26-06-2026"; // Current system-wide date

    Enrollment newEnroll;
    std::snprintf(newEnroll.enrollment_id, sizeof(newEnroll.enrollment_id), "%s", eId.c_str());
    std::snprintf(newEnroll.roll_no, sizeof(newEnroll.roll_no), "%s", roll_no.c_str());
    std::snprintf(newEnroll.course_code, sizeof(newEnroll.course_code), "%s", course_code.c_str());
    newEnroll.semester = students[sIdx].semester;
    std::snprintf(newEnroll.enrollment_date, sizeof(newEnroll.enrollment_date), "%s", today.c_str());
    std::snprintf(newEnroll.status, sizeof(newEnroll.status), "active");
    
    // Default values for grading components
    newEnroll.assignment = -1.0;
    newEnroll.quiz = -1.0;
    newEnroll.mid = -1.0;
    newEnroll.project = -1.0;
    newEnroll.final_exam = -1.0;
    newEnroll.total_marks = -1.0;
    std::snprintf(newEnroll.grade, sizeof(newEnroll.grade), "N/A");

    // Update course count (RMW)
    courses[cIdx].enrolled += 1;
    saveCourses(courses);

    // Save enrollment record
    enrollments.push_back(newEnroll);
    if (saveEnrollments(enrollments)) {
        std::cout << "[Success] Student " << roll_no << " enrolled in " << course_code << " (ID: " << eId << ")\n";
    } else {
        std::cout << "[Error] Failed to log enrollment.\n";
    }
}

void dropCourseInteractive() {
    std::cout << "\n--- Drop/Withdraw Course ---\n";
    
    std::string roll_no, course_code;
    std::cout << "Enter Student Roll Number: ";
    std::cin >> roll_no;
    std::cout << "Enter Course Code to Drop: ";
    std::cin >> course_code;
    std::cin.ignore(10000, '\n');

    std::vector<Enrollment> enrollments = loadEnrollments();
    int eIdx = findEnrollmentIndex(enrollments, roll_no, course_code);
    if (eIdx == -1 || std::string(enrollments[eIdx].status) != "active") {
        std::cout << "[Error] Active enrollment record not found.\n";
        return;
    }

    // Atomic RMW updates
    std::snprintf(enrollments[eIdx].status, sizeof(enrollments[eIdx].status), "dropped");
    saveEnrollments(enrollments);

    std::vector<Course> courses = loadCourses();
    int cIdx = findCourseIndex(courses, course_code);
    if (cIdx != -1) {
        courses[cIdx].enrolled = (courses[cIdx].enrolled > 0) ? courses[cIdx].enrolled - 1 : 0;
        saveCourses(courses);
    }

    std::cout << "[Success] Student " << roll_no << " dropped course " << course_code << ".\n";
}

void listEnrollmentsInteractive() {
    std::cout << "\n--- Student Enrollment Logs ---\n";
    std::vector<Enrollment> enrollments = loadEnrollments();

    if (enrollments.empty()) {
        std::cout << "No enrollments found.\n";
        return;
    }

    std::cout << std::left << std::setw(15) << "Enroll ID"
              << std::setw(15) << "Roll Number"
              << std::setw(12) << "Course Code"
              << std::setw(10) << "Semester"
              << std::setw(15) << "Date"
              << std::setw(10) << "Status" 
              << std::setw(10) << "Grade" << "\n";
    std::cout << std::string(87, '-') << "\n";

    for (size_t i = 0; i < enrollments.size(); ++i) {
        std::cout << std::left << std::setw(15) << enrollments[i].enrollment_id
                  << std::setw(15) << enrollments[i].roll_no
                  << std::setw(12) << enrollments[i].course_code
                  << std::setw(10) << enrollments[i].semester
                  << std::setw(15) << enrollments[i].enrollment_date
                  << std::setw(10) << enrollments[i].status
                  << std::setw(10) << enrollments[i].grade << "\n";
    }
    std::cout << std::string(87, '-') << "\n";
}
