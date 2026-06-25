# Campus-ANALYTICS-ENGINE-
 Campus Analytics Engine (Modular Data Analytics Framework)

Campus Analytics Engine is a high-performance, modular command-line academic administration and reports aggregate framework. Developed from scratch in C++, it adheres to strict **procedural programming constraints** (Programming Fundamentals paradigm). It operates without Object-Oriented Programming (OOP) classes, STL container algorithms, or library date modules, relying instead on raw data structures, nested loops, manual string parsing, and an atomic file input/output design pattern.

---

## 🌟 Key Features

1. **Student Directory CRUD**:
   * Create, Read, Update, and Delete student records with validation of roll numbers and automatic GPA syncing.
2. **Course Registration & Relational Enrollment**:
   * Full course catalog and enrollment tracker.
   * Auto-validation of seat capacities, active student status, and prerequisite course completion.
3. **Attendance Logging**:
   * Real-time session attendance logging (Present, Absent, Late).
   * Interactive single student queries and bulk session attendance sheets for active class enrollments.
4. **Weighted Grade & GPA Evaluation**:
   * Evaluates academic performance using 5 distinct assessment components:
     * Assignments (15%)
     * Quizzes (10%)
     * Midterm Exam (25%)
     * Project (15%)
     * Final Exam (35%)
   * Calculates Letter Grades and Course GPAs based on total marks.
   * **Attendance Penalty Enforcement**: Automatically drops student component marks by 5.0 (for attendance $< 75\%$) or 10.0 (for attendance $< 65\%$).
5. **tuition Ledgers & Late Fee Processing**:
   * Fee generation and transaction entries.
   * Checks due dates using manual date comparisons (determining `paid_late` statuses without using library helpers).
6. **Aggregate Campus Reports Dashboard**:
   * Prints statistics on department-wise average CGPA, course fail rates, outstanding defaulters list, financial collection efficiency, and lists the top 5 campus performers.

---

## ⚙️ Compilation & Build Instructions

Ensure you have a modern C++ compiler installed (supporting C++11 or later, such as GCC/G++).

To compile all project modules from the root directory and output the executable to the `/bin` directory, execute the following command:

```bash
gcc src/*.cpp -I./src -lstdc++ -o bin/campus_analytics.exe
```

*Note: The `-lstdc++` flag is passed to the GCC driver to properly link standard C++ libraries, resolving symbols for containers and stream streams while executing C++ files procedural-style.*

### Running the System
Once compiled, you can launch the engine:
```bash
./bin/campus_analytics.exe
```

---

## 🏛️ System Architecture

### 1. Directory Structure
```
/campus analysis/
├── data/                       # CSV Database Files (Persistent Storage)
│   ├── students.txt            # Students records
│   ├── courses.txt             # Academic courses catalog
│   ├── enrollments.txt         # Student registrations, marks, and final grades
│   ├── attendance_log.txt      # Session-by-session student attendance logs
│   └── fees.txt                # Financial ledgers and balances
├── src/                        # C++ Source Code Modules (Clear separation of concerns)
│   ├── main.cpp                # Console CLI loop and navigation menus
│   ├── filehandler.h/.cpp      # Char-by-char CSV parser and date comparator
│   ├── student_ops.h/.cpp      # Student records management logic
│   ├── course_ops.h/.cpp       # Courses and enrollment operations
│   ├── attendance.h/.cpp       # Attendance sheet logging and rating calculator
│   ├── grades.h/.cpp           # Grade evaluations, penalties, and CGPA syncing
│   ├── fee_tracker.h/.cpp      # Tuitionchallan generator and payments logger
│   └── reports.h/.cpp          # Campus aggregate reports generator
└── bin/                        # Build output directory
    └── campus_analytics.exe    # Compiled binary
```

---

## 🛠️ Programming Constraints & Implementation Design

To comply with Programming Fundamentals restrictions, this framework implements the following designs:

### A. Non-OOP Procedural Paradigm (Structs & Functions)
No classes are defined in the engine. All data representations are flat C-style structs:
* `Student`, `Course`, `Enrollment`, `Attendance`, and `Fee`.
All behaviors are structured as static, standalone functions passing these structs by reference to optimize performance.

### B. No STL Algorithms or Associative Containers
The use of `#include <algorithm>`, `<map>`, `<unordered_map>`, or `<set>` is strictly forbidden.
* **Sorting**: The Top 5 Performers report uses a manual **Selection Sort** algorithm.
* **Grouping**: Department averages and course averages are compiled using parallel arrays (e.g. `char depts[50][100]`) and matching index counters populated by nested lookup loops.

### C. Manual Date Arithmetic
The `#include <ctime>` header is forbidden. All date checks use manual parsing of `DD-MM-YYYY` string structures:
* A custom date helper parses date sub-strings into integer representation (day, month, year).
* Chronological comparisons (such as checking if a payment was submitted late relative to a due date) are evaluated sequentially:
  ```cpp
  if (year1 != year2) return year1 > year2;
  if (month1 != month2) return month1 > month2;
  return day1 > day2;
  ```

### D. Character-by-Character CSV Parser
No standard line-splitting algorithms (`std::getline(stream, str, delimiter)`) are utilized. The `filehandler` reads stream files character-by-character:
```cpp
while (file.get(c)) {
    if (c == '\n') {
        // finalize cell and row
    } else if (c == ',') {
        // finalize cell
    } else {
        // accumulate character
    }
}
```

### E. Atomic Read-Modify-Write (RMW) Pattern
For all data modifications (e.g., updating student CGPA, recording a payment, or dropping a course), the system:
1. Loads the entire database file into a memory vector of structs.
2. Identifies and updates the target struct matching the key index.
3. Completely rewrites the updated array structure back to the target CSV file, preventing partial file corruption.

### F. Cross-Module Interactions
To ensure relational integrity:
* `grades.cpp` calls `getAttendancePercentage(...)` from `attendance.h` to check the student's attendance rate before final grading.
* Upon successfully entering grades, `grades.cpp` queries course credits from `course_ops.h` and triggers `syncStudentCGPA(...)`, which updates the student's CGPA in `students.txt`.
