#include "fee_tracker.h"
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
using namespace std;


std::vector<Fee> loadFees() {
    std::vector<Fee> fees;
    std::vector<std::vector<std::string>> data = readTXT("fees.txt");
    if (data.empty()) return fees;

    // Line 0 is header: fee_id,roll_no,semester,total_fee,amount_paid,due_date,payment_date,payment_method,status
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i].size() < 9) continue;
        Fee f;
        std::snprintf(f.fee_id, sizeof(f.fee_id), "%s", data[i][0].c_str());
        std::snprintf(f.roll_no, sizeof(f.roll_no), "%s", data[i][1].c_str());
        try {
            f.semester = std::stoi(data[i][2]);
        } catch (...) {
            f.semester = 1;
        }
        try {
            f.total_fee = std::stod(data[i][3]);
        } catch (...) {
            f.total_fee = 0.0;
        }
        try {
            f.amount_paid = std::stod(data[i][4]);
        } catch (...) {
            f.amount_paid = 0.0;
        }
        std::snprintf(f.due_date, sizeof(f.due_date), "%s", data[i][5].c_str());
        std::snprintf(f.payment_date, sizeof(f.payment_date), "%s", data[i][6].c_str());
        std::snprintf(f.payment_method, sizeof(f.payment_method), "%s", data[i][7].c_str());
        std::snprintf(f.status, sizeof(f.status), "%s", data[i][8].c_str());
        fees.push_back(f);
    }
    return fees;
}

bool saveFees(const std::vector<Fee>& fees) {
    std::vector<std::vector<std::string>> data;
    std::vector<std::string> header = {"fee_id", "roll_no", "semester", "total_fee", "amount_paid", "due_date", "payment_date", "payment_method", "status"};
    data.push_back(header);

    for (size_t i = 0; i < fees.size(); ++i) {
        std::vector<std::string> row;
        row.push_back(fees[i].fee_id);
        row.push_back(fees[i].roll_no);
        row.push_back(std::to_string(fees[i].semester));
        
        char tf[20], ap[20];
        std::snprintf(tf, sizeof(tf), "%.0f", fees[i].total_fee);
        std::snprintf(ap, sizeof(ap), "%.0f", fees[i].amount_paid);
        row.push_back(tf);
        row.push_back(ap);

        row.push_back(fees[i].due_date);
        row.push_back(fees[i].payment_date);
        row.push_back(fees[i].payment_method);
        row.push_back(fees[i].status);
        data.push_back(row);
    }
    return writeTXT("fees.txt", data);
}

std::string generateFeeId(const std::vector<Fee>& fees) {
    int maxId = 0;
    for (size_t i = 0; i < fees.size(); ++i) {
        std::string idStr = fees[i].fee_id;
        if (idStr.length() > 1 && idStr[0] == 'F') {
            try {
                int val = std::stoi(idStr.substr(1));
                if (val > maxId) {
                    maxId = val;
                }
            } catch (...) {}
        }
    }
    char buf[20];
    std::snprintf(buf, sizeof(buf), "F%03d", maxId + 1);
    return std::string(buf);
}

int findFeeIndex(const std::vector<Fee>& fees, const std::string& fee_id) {
    for (size_t i = 0; i < fees.size(); ++i) {
        if (std::string(fees[i].fee_id) == fee_id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void generateFeeChallanInteractive() {
    std::cout << "\n--- Generate Semester Fee Challan ---\n";
    std::string roll_no, due_date;
    int semester;
    double total_fee;

    std::cout << "Enter Student Roll Number: ";
    std::cin >> roll_no;

    // Validate student exists
    std::vector<Student> students = loadStudents();
    if (findStudentIndex(students, roll_no) == -1) {
        std::cout << "[Error] Student not found.\n";
        return;
    }

    std::cout << "Enter Semester: ";
    while (!(std::cin >> semester) || semester < 1) {
        std::cout << "Invalid semester. Enter positive integer: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }

    std::cout << "Enter Tuition Fee Amount: ";
    while (!(std::cin >> total_fee) || total_fee <= 0.0) {
        std::cout << "Invalid amount. Enter positive value: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    std::cin.ignore(10000, '\n');

    std::cout << "Enter Due Date (DD-MM-YYYY): ";
    std::cin >> due_date;
    std::cin.ignore(10000, '\n');

    int day, month, year;
    if (!parseDate(due_date, day, month, year)) {
        std::cout << "[Error] Invalid date format. Must be DD-MM-YYYY.\n";
        return;
    }

    std::vector<Fee> fees = loadFees();
    std::string fee_id = generateFeeId(fees);

    Fee f;
    std::snprintf(f.fee_id, sizeof(f.fee_id), "%s", fee_id.c_str());
    std::snprintf(f.roll_no, sizeof(f.roll_no), "%s", roll_no.c_str());
    f.semester = semester;
    f.total_fee = total_fee;
    f.amount_paid = 0.0;
    std::snprintf(f.due_date, sizeof(f.due_date), "%s", due_date.c_str());
    std::snprintf(f.payment_date, sizeof(f.payment_date), "00-00-0000");
    std::snprintf(f.payment_method, sizeof(f.payment_method), "NONE");
    std::snprintf(f.status, sizeof(f.status), "unpaid");

    fees.push_back(f);
    if (saveFees(fees)) {
        std::cout << "[Success] Challan generated successfully! Fee ID: " << fee_id << "\n";
    } else {
        std::cout << "[Error] Failed to save challan record.\n";
    }
}

void recordPaymentInteractive() {
    std::cout << "\n--- Record Fee Payment ---\n";
    std::string fee_id;
    std::cout << "Enter Fee ID (e.g. F001): ";
    std::cin >> fee_id;
    std::cin.ignore(10000, '\n');

    std::vector<Fee> fees = loadFees();
    int fIdx = findFeeIndex(fees, fee_id);
    if (fIdx == -1) {
        std::cout << "[Error] Fee record not found.\n";
        return;
    }

    Fee& f = fees[fIdx];
    if (std::string(f.status) == "paid" || std::string(f.status) == "paid_late") {
        std::cout << "[Info] This fee challan has already been fully paid.\n";
        return;
    }

    double amount;
    std::cout << "Challan amount: " << f.total_fee << ", Already Paid: " << f.amount_paid 
              << "\nEnter Payment Amount: ";
    while (!(std::cin >> amount) || amount <= 0.0 || amount > (f.total_fee - f.amount_paid)) {
        std::cout << "Invalid amount. Remaining balance is " << (f.total_fee - f.amount_paid) << ": ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }

    std::string payment_method;
    std::cout << "Enter Payment Method (Cash/Bank/Online): ";
    std::cin >> payment_method;
    
    std::string payment_date;
    std::cout << "Enter Payment Date (DD-MM-YYYY): ";
    std::cin >> payment_date;
    std::cin.ignore(10000, '\n');

    int day, month, year;
    if (!parseDate(payment_date, day, month, year)) {
        std::cout << "[Error] Invalid date format. Must be DD-MM-YYYY.\n";
        return;
    }

    // Accumulate payment
    f.amount_paid += amount;
    std::snprintf(f.payment_method, sizeof(f.payment_method), "%s", payment_method.c_str());
    std::snprintf(f.payment_date, sizeof(f.payment_date), "%s", payment_date.c_str());

    // Update status (Compare payment date with due date via isDateAfter)
    if (f.amount_paid >= f.total_fee) {
        if (isDateAfter(payment_date, f.due_date)) {
            std::snprintf(f.status, sizeof(f.status), "paid_late");
            std::cout << "[Info] Late payment detected! Status set to paid_late.\n";
        } else {
            std::snprintf(f.status, sizeof(f.status), "paid");
        }
    } else {
        std::snprintf(f.status, sizeof(f.status), "partial");
    }

    if (saveFees(fees)) {
        std::cout << "[Success] Payment recorded successfully! Challan updated.\n";
    } else {
        std::cout << "[Error] Failed to save payment record.\n";
    }
}

void viewStudentFeeStatusInteractive() {
    std::cout << "\n--- View Student Fee Status ---\n";
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

    std::vector<Fee> fees = loadFees();
    std::cout << "\nFee Directory for " << students[sIdx].name << " (" << roll_no << "):\n";
    std::cout << std::left << std::setw(10) << "Fee ID"
              << std::setw(10) << "Semester"
              << std::setw(12) << "Total Fee"
              << std::setw(12) << "Paid Amount"
              << std::setw(12) << "Due Date"
              << std::setw(12) << "Pay Date"
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(80, '-') << "\n";

    bool foundAny = false;
    for (size_t i = 0; i < fees.size(); ++i) {
        if (std::string(fees[i].roll_no) == roll_no) {
            std::cout << std::left << std::setw(10) << fees[i].fee_id
                      << std::setw(10) << fees[i].semester
                      << std::setw(12) << std::fixed << std::setprecision(0) << fees[i].total_fee
                      << std::setw(12) << fees[i].amount_paid
                      << std::setw(12) << fees[i].due_date
                      << std::setw(12) << fees[i].payment_date
                      << std::setw(10) << fees[i].status << "\n";
            foundAny = true;
        }
    }
    std::cout << std::string(80, '-') << "\n";
    if (!foundAny) {
        std::cout << "No fee challans generated yet for this student.\n";
    }
}

void listDefaultersInteractive() {
    std::cout << "\n--- Defaulter's List (Unpaid & Partial Challans) ---\n";
    std::vector<Fee> fees = loadFees();
    std::vector<Student> students = loadStudents();

    std::cout << std::left << std::setw(10) << "Fee ID"
              << std::setw(15) << "Roll No"
              << std::setw(25) << "Student Name"
              << std::setw(10) << "Semester"
              << std::setw(12) << "Arrears"
              << std::setw(12) << "Due Date"
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(94, '-') << "\n";

    bool foundAny = false;
    for (size_t i = 0; i < fees.size(); ++i) {
        std::string st = fees[i].status;
        if (st == "unpaid" || st == "partial") {
            double arrears = fees[i].total_fee - fees[i].amount_paid;
            std::string name = "Unknown Student";
            int sIdx = findStudentIndex(students, fees[i].roll_no);
            if (sIdx != -1) {
                name = students[sIdx].name;
            }

            std::cout << std::left << std::setw(10) << fees[i].fee_id
                      << std::setw(15) << fees[i].roll_no
                      << std::setw(25) << name
                      << std::setw(10) << fees[i].semester
                      << std::setw(12) << std::fixed << std::setprecision(0) << arrears
                      << std::setw(12) << fees[i].due_date
                      << std::setw(10) << st << "\n";
            foundAny = true;
        }
    }
    std::cout << std::string(94, '-') << "\n";
    if (!foundAny) {
        std::cout << "All clear! No students have outstanding balances.\n";
    }
}
