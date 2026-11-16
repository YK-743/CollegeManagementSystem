# College Management System (CMS)

A robust, modular, console-based College Management System built in **C++17**, developed as a showcase of Object-Oriented Programming (OOP) principles, clean architecture, and persistent data storage.

---

## 1. Project Overview

The College Management System is an end-to-end management portal for higher education institutions. It provides comprehensive administrative, academic, and financial capabilities across eight interconnected modules:

1. **Student Management** — Admission, profile updates, soft deactivation, search, and polymorphic profile inspection.
2. **Faculty Management** — Faculty hiring, detail modification, deactivation, and departmental search.
3. **Course Management** — Course catalog creation, credit/capacity limits, and faculty course assignment.
4. **Student Enrollment** — Course registrations, capacity enforcement, course dropping, and re-enrollment tracking.
5. **Attendance Management** — Class-by-class attendance recording (Present/Absent), record correction, attendance percentage calculation, and low-attendance alerts.
6. **Fee Management** — Fee tracking, payment collection with receipt generation, overpayment prevention, and financial ledger history.
7. **Reports & Analytics** — Institutional dashboard summaries, fee balances, low-attendance threshold warnings, and course enrollment ratios.
8. **Data Management** — File persistence (pipe-delimited text storage with escaping), manual backup saves, and reloading.

---

## 2. Architecture Overview

The system strictly adheres to a **Four-Layer Modular Architecture**:

```
+-----------------------------------------------------------------------+
|                             ConsoleUI                                 |
|      (Presentation Layer: Menus, Input Validation, Formatted Tables)   |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                           CollegeSystem                               |
|                (Central Application Coordinator / Facade)             |
+-----------------------------------------------------------------------+
            |                                               |
            v                                               v
+-----------------------+                       +-----------------------+
|       Managers        |                       |      DataManager      |
|   (Business Logic)    |<----------------------|  (Persistence Layer)  |
| - StudentManager      |   (friend access      | - Pipe serialization  |
| - FacultyManager      |    to private         | - Field escaping      |
| - CourseManager       |    collections)       | - 6 text files        |
| - EnrollmentManager   |                       +-----------------------+
| - AttendanceManager   |
| - FeeManager          |
| - ReportManager       |
+-----------------------+
            |
            v
+-----------------------------------------------------------------------+
|                            Domain Models                              |
|           Person (Abstract) <--- Student, Faculty                     |
|           Course, Enrollment, AttendanceRecord, FeePayment            |
+-----------------------------------------------------------------------+
```

### Architectural Principles:
* **Separation of Concerns:** Business logic in Managers is completely isolated from I/O (`std::cin` / `std::cout` reside strictly within `ConsoleUI`).
* **Encapsulation:** All attributes are `private` across all layers.
* **Controlled Persistence:** Persistence logic lives exclusively in `DataManager`, which accesses raw collections via `friend class DataManager;` declarations without polluting public domain interfaces.
* **Soft Deletion & Audit Trail:** Deactivated students, faculty, courses, and dropped enrollments are never physically erased; their active flags are toggled to preserve historical academic and financial integrity.

---

## 3. Class Organization & Folder Structure

```
CollegeManagementSystem/
├── include/
│   └── college/
│       ├── Models.h            # Domain entities (Person, Student, Faculty, Course, etc.)
│       ├── Managers.h          # Business logic managers & report structures
│       ├── DataManager.h       # File storage & persistence engine
│       ├── CollegeSystem.h     # Central coordinator owning all managers
│       └── ConsoleUI.h         # Console menu system & input validation
├── src/
│   ├── main.cpp                # Application entry point with top-level try/catch
│   ├── Models.cpp              # Model member functions & polymorphic displayDetails()
│   ├── Managers.cpp            # Business rule validation & calculations
│   ├── DataManager.cpp         # Pipe-delimited file serialization & deserialization
│   ├── CollegeSystem.cpp       # Coordinator implementation & manager accessors
│   └── ConsoleUI.cpp           # Menu loops, input sanitization, table formatting
├── data/                       # Persistent text storage (created automatically)
│   ├── students.txt            # Student records
│   ├── faculty.txt             # Faculty records
│   ├── courses.txt             # Course records
│   ├── enrollments.txt         # Enrollment & drop records
│   ├── attendance.txt          # Class attendance logs
│   └── payments.txt            # Fee payment receipts
├── build/                      # Build outputs, test runners, and test suites
│   ├── test_models.cpp         # Model class smoke tests
│   ├── test_managers.cpp       # Core managers smoke tests
│   ├── test_academic_managers.cpp # Academic managers tests
│   ├── test_finance_reports.cpp# Finance & reporting tests
│   ├── test_persistence.cpp    # File persistence round-trip tests
│   ├── test_integration.cpp    # Full system integration test suite
│   └── test_e2e_edge_cases.cpp # Edge-case & boundary condition test suite
├── OOP_CONCEPT_MAP.md          # Comprehensive OOP syllabus mapping
├── CMakeLists.txt              # Standard CMake build configuration
└── README.md                   # This project guide
```

---

## 4. Compiler Requirements & Compilation

### Requirements:
* **Language Standard:** C++17
* **Compiler:** MinGW GCC (tested with GCC 6.3.0 and newer) or any standard-conforming C++17 compiler (Clang, MSVC)
* **Operating System:** Windows, Linux, or macOS (platform-agnostic standard C++)

### Compilation Command (Direct MinGW GCC):
```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Models.cpp src/Managers.cpp src/DataManager.cpp src/CollegeSystem.cpp src/ConsoleUI.cpp -o build/CollegeManagementSystem.exe
```

### Building via CMake (Alternative):
```powershell
mkdir build
cd build
cmake ..
cmake --build .
```

---

## 5. Running the Application

From the project root directory:

```powershell
.\build\CollegeManagementSystem.exe
```

Upon launch:
1. `CollegeSystem` automatically loads existing records from the `data/` folder.
2. If files are missing (e.g. on first launch), empty collections are initialized gracefully without error.
3. Every modification (registration, course assignment, enrollment, attendance, fee payment) automatically triggers an auto-save to ensure zero data loss.
4. Exiting via menu option `9` performs a final synchronization save and clean shutdown.

---

## 6. How Data Persistence Works

* **Format:** Custom pipe-delimited text files (`field1|field2|field3...`).
* **Field Escaping:** Safe character translation prevents corrupted records:
  * Pipe characters are escaped as `\|`
  * Backslashes are escaped as `\\`
  * Newline characters are escaped as `\n` and `\r`
* **Precision:** Floating-point monetary values use `std::setprecision(17)` for exact round-trip preservation.
* **Fail-Safe Loading:** `loadAllData()` constructs temporary managers; existing in-memory state is only replaced if all files parse successfully without data corruption.

---

## 7. Module Guide & Menu Reference

### Main Menu Options:
```
========================================
   COLLEGE MANAGEMENT SYSTEM
========================================
  1. Student Management
  2. Faculty Management
  3. Course Management
  4. Student Enrollment
  5. Attendance Management
  6. Fee Management
  7. Reports & Analytics
  8. Data Management
  9. Exit
========================================
```

1. **Student Management**
   * Register students with roll number, full name, email, phone, department, semester, and total fee.
   * Update student details (preserves unedited fields if left blank).
   * Soft-deactivate students (preserves academic history while excluding them from active lists).
   * Search student by roll number.
   * View complete student profile with polymorphic Person display, course enrollments, and fee ledger.

2. **Faculty Management**
   * Register faculty members with ID, name, email, phone, department, and designation.
   * Update faculty contact details and designation.
   * Soft-deactivate faculty members.
   * Search and view active faculty listings.

3. **Course Management**
   * Add courses with code, name, credits, and student capacity limits.
   * Assign active faculty to active courses.
   * View course catalog showing real-time enrollment ratios (`Enrolled / Capacity`).
   * Soft-deactivate courses.

4. **Student Enrollment**
   * Enroll active students into active courses.
   * Enforces capacity limits — rejects enrollments if the course is full.
   * Reject duplicate active enrollments.
   * Drop courses — marks enrollment inactive, freeing up course capacity while retaining historical records.
   * Re-enrollment supported after dropping.

5. **Attendance Management**
   * Mark attendance by date (`1` for Present, `0` for Absent).
   * Validates active enrollment before marking attendance.
   * Rejects duplicate entries for the same student, course, and date.
   * Correct existing attendance entries.
   * Calculate live attendance percentage per course.
   * Generate attendance warning reports for students falling below a custom threshold percentage.

6. **Fee Management**
   * View student fee status (Total Fee, Total Paid, Outstanding Balance).
   * Record fee payments with date, mode (Cash, UPI, Card), and unique receipt number.
   * Rejects payments exceeding outstanding fees (overpayment protection).
   * Rejects zero, negative, or duplicate receipt numbers.
   * Generate printable, formatted payment receipts.
   * View student payment history and institutional total collections.

7. **Reports & Analytics**
   * **College Dashboard:** Instant KPI metrics (Total active students, faculty, courses, enrollments, total collected fees, total outstanding fees).
   * **Student Fee Report:** Tabular ledger of all active students with individual balances.
   * **Attendance Report:** Course-wise attendance percentages with `BELOW` / `OK` threshold flags.
   * **Course Enrollment Report:** Course capacity utilization and assigned faculty.

8. **Data Management**
   * Manual save of all records to disk.
   * Hot-reload data from disk (with confirmation prompt to prevent unintended in-memory overwrites).

9. **Exit**
   * Automatically executes `saveAllData()` and terminates cleanly.

---

## 8. Sample Demonstration Workflow

Follow these steps to demonstrate the full lifecycle of the system:

1. **Register a Faculty Member:**
   * Go to `2. Faculty Management` -> `1. Register faculty`
   * Enter: ID: `FAC001`, Name: `Dr. Alan Turing`, Dept: `Computer Science`, Designation: `Professor`
2. **Add a Course:**
   * Go to `3. Course Management` -> `1. Add course`
   * Enter: Code: `CS101`, Name: `Discrete Mathematics`, Credits: `4`, Capacity: `2`, Faculty ID: `FAC001`
3. **Register Students:**
   * Go to `1. Student Management` -> `1. Register new student`
   * Register Student 1: Roll: `2026CS01`, Name: `Ada Lovelace`, Dept: `Computer Science`, Semester: `1`, Total Fee: `50000`
   * Register Student 2: Roll: `2026CS02`, Name: `Grace Hopper`, Dept: `Computer Science`, Semester: `1`, Total Fee: `50000`
4. **Enroll in Course (Capacity Enforcement):**
   * Go to `4. Student Enrollment` -> `1. Enroll student`
   * Enroll `2026CS01` in `CS101` (Success: 1/2 filled)
   * Enroll `2026CS02` in `CS101` (Success: 2/2 filled)
5. **Mark Attendance:**
   * Go to `5. Attendance Management` -> `1. Mark attendance`
   * Mark `2026CS01` in `CS101` on `2026-09-01` as Present (`1`)
   * Mark `2026CS01` in `CS101` on `2026-09-02` as Absent (`0`)
   * Check Attendance Percentage: `50.00%`
6. **Record Fee Payment & Print Receipt:**
   * Go to `6. Fee Management` -> `2. Record fee payment`
   * Student: `2026CS01`, Receipt: `REC-001`, Amount: `25000`, Mode: `UPI`
   * View receipt via `4. Generate fee receipt` -> enter `REC-001`
7. **View Institutional Dashboard:**
   * Go to `7. Reports & Analytics` -> `1. College dashboard`
   * Observe live calculated totals: Students: `2`, Faculty: `1`, Courses: `1`, Active Enrollments: `2`, Collections: `25000.00`, Outstanding: `75000.00`.
8. **Exit and Verify Persistence:**
   * Choose `9. Exit`.
   * Restart the application: all records, fee payments, and attendance logs are automatically restored!

---

## 9. Test Suite & Verification

The project includes 7 specialized automated test drivers located in the `build/` directory:

| Test Driver | Focus Area | Checks | Result |
|---|---|---|---|
| `test_models.cpp` | Model constructors, getters, mutators, and polymorphism | 9 | **PASS** |
| `test_managers.cpp` | Student, Faculty, Course business rules and soft-delete | 56 | **PASS** |
| `test_academic_managers.cpp` | Enrollment, capacity checks, and attendance math | 35 | **PASS** |
| `test_finance_reports.cpp` | Fee payments, overpayment guards, and report logic | 57 | **PASS** |
| `test_persistence.cpp` | Data persistence, round-trip save/load, and error recovery | 89 | **PASS** |
| `test_integration.cpp` | Complete coordinator, polymorphism, and manager interaction | 69 | **PASS** |
| `test_e2e_edge_cases.cpp` | Boundary conditions, spaces, escaping, and invalid states | 81 | **PASS** |
| **Total** | **Comprehensive Regression Suite** | **396** | **ALL PASSED** |

To compile and run any test suite:
```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/Models.cpp src/Managers.cpp src/DataManager.cpp src/CollegeSystem.cpp build/test_e2e_edge_cases.cpp -o build/test_e2e_edge_cases.exe
.\build\test_e2e_edge_cases.exe
```

---

## 10. License & Academic Submission
Developed for academic coursework in Object-Oriented Programming (OOPS). Demonstrates full compliance with standard C++17 and GCC 6.3.0.
