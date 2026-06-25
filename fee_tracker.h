#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Fee {
    char fee_id[50];
    char roll_no[50];
    int semester;
    double total_fee;
    double amount_paid;
    char due_date[50]; // DD-MM-YYYY
    char payment_date[50]; // DD-MM-YYYY or 00-00-0000
    char payment_method[50]; // Cash, Bank, Online, NONE
    char status[50]; // paid, paid_late, partial, unpaid
};

// Load fee records from fees.txt into a vector
vector<Fee> loadFees();

// Save fee records to fees.txt (RMW pattern)
bool saveFees(const vector<Fee>& fees);

// Interactive menu options
void generateFeeChallanInteractive();
void recordPaymentInteractive();
void viewStudentFeeStatusInteractive();
void listDefaultersInteractive();

#endif
