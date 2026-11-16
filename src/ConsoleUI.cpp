// College Management System - Console UI Implementation

#include "college/ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace college {

// ============================================================================
// Constructor
// ============================================================================

ConsoleUI::ConsoleUI(CollegeSystem& system) : system(system) {}

// ============================================================================
// Input Helpers
// ============================================================================

std::string ConsoleUI::readLine(const std::string& prompt) const {
    std::cout << prompt;
    std::string input;
    if (!std::getline(std::cin, input)) {
        // EOF reached — signal exit by returning empty string
        std::cin.clear();
        return "";
    }
    return input;
}

int ConsoleUI::readInt(const std::string& prompt) const {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            // EOF reached — return a sentinel that triggers exit
            std::cin.clear();
            return -1;
        }
        std::istringstream iss(line);
        int value;
        char leftover;
        if (iss >> value && !(iss >> leftover)) {
            return value;
        }
        std::cout << "  Invalid input. Please enter a whole number.\n";
    }
}

double ConsoleUI::readDouble(const std::string& prompt) const {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            // EOF reached — return 0 as fallback
            std::cin.clear();
            return 0.0;
        }
        std::istringstream iss(line);
        double value;
        char leftover;
        if (iss >> value && !(iss >> leftover)) {
            return value;
        }
        std::cout << "  Invalid input. Please enter a number.\n";
    }
}

// ============================================================================
// Display Helpers
// ============================================================================

void ConsoleUI::printHeading(const std::string& title) const {
    std::cout << "\n========================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================\n\n";
}

void ConsoleUI::pauseScreen() const {
    std::cout << "\nPress Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void ConsoleUI::autoSave() {
    if (!system.saveAllData()) {
        std::cout << "  WARNING: Failed to save data to files!\n";
    }
}

// ============================================================================
// Main Loop
// ============================================================================

void ConsoleUI::run() {
    int choice = 0;
    do {
        showMainMenu();
        choice = readInt("Enter your choice: ");

        // Handle EOF (e.g., piped input exhausted)
        if (std::cin.eof()) {
            choice = 9;
        }

        switch (choice) {
            case 1: studentMenu();    break;
            case 2: facultyMenu();    break;
            case 3: courseMenu();      break;
            case 4: enrollmentMenu(); break;
            case 5: attendanceMenu(); break;
            case 6: feeMenu();        break;
            case 7: reportMenu();     break;
            case 8: dataMenu();       break;
            case 9:
                autoSave();
                std::cout << "\nAll data saved. Thank you for using the College Management System!\n";
                break;
            default:
                std::cout << "  Invalid choice. Please select 1-9.\n";
                break;
        }
    } while (choice != 9);
}

void ConsoleUI::showMainMenu() {
    std::cout << "\n========================================\n";
    std::cout << "   COLLEGE MANAGEMENT SYSTEM\n";
    std::cout << "========================================\n";
    std::cout << "  1. Student Management\n";
    std::cout << "  2. Faculty Management\n";
    std::cout << "  3. Course Management\n";
    std::cout << "  4. Student Enrollment\n";
    std::cout << "  5. Attendance Management\n";
    std::cout << "  6. Fee Management\n";
    std::cout << "  7. Reports & Analytics\n";
    std::cout << "  8. Data Management\n";
    std::cout << "  9. Exit\n";
    std::cout << "========================================\n";
}

// ============================================================================
// 1. Student Management
// ============================================================================

void ConsoleUI::studentMenu() {
    int choice = 0;
    do {
        printHeading("STUDENT MANAGEMENT");
        std::cout << "  1. Register new student\n";
        std::cout << "  2. Update student details\n";
        std::cout << "  3. Deactivate student\n";
        std::cout << "  4. View all active students\n";
        std::cout << "  5. Search student by roll number\n";
        std::cout << "  6. View individual student profile\n";
        std::cout << "  7. Back\n\n";
        choice = readInt("Enter your choice: ");
        if (std::cin.eof()) break;

        switch (choice) {
        case 1: {
            printHeading("REGISTER NEW STUDENT");
            std::string rollNo = readLine("Roll Number : ");
            std::string name   = readLine("Name        : ");
            if (rollNo.empty() || name.empty()) {
                std::cout << "  Error: Roll number and name cannot be empty.\n";
                pauseScreen(); break;
            }
            std::string email  = readLine("Email       : ");
            std::string phone  = readLine("Phone       : ");
            std::string dept   = readLine("Department  : ");
            int semester = readInt("Semester    : ");
            if (semester <= 0) {
                std::cout << "  Error: Semester must be a positive number.\n";
                pauseScreen(); break;
            }
            double totalFee = readDouble("Total Fee   : ");
            if (totalFee <= 0.0) {
                std::cout << "  Error: Total fee must be a positive value.\n";
                pauseScreen(); break;
            }

            Student student(rollNo, name, email, phone, dept, semester, totalFee);
            if (system.getStudentManager().addStudent(student)) {
                std::cout << "  Student registered successfully!\n";
                autoSave();
            } else {
                std::cout << "  Error: Student with roll number '" << rollNo
                          << "' already exists.\n";
            }
            pauseScreen();
            break;
        }
        case 2: {
            printHeading("UPDATE STUDENT DETAILS");
            std::string rollNo = readLine("Enter roll number to update: ");
            Student* s = system.getStudentManager().findStudent(rollNo);
            if (s == nullptr || !s->isActive()) {
                std::cout << "  Error: Active student '" << rollNo << "' not found.\n";
                pauseScreen(); break;
            }
            std::cout << "  Current details:\n";
            s->displayDetails();

            std::cout << "\n  Enter new details (leave blank to keep current):\n";
            std::string name  = readLine("Name [" + s->getName() + "]       : ");
            std::string email = readLine("Email [" + s->getEmail() + "]     : ");
            std::string phone = readLine("Phone [" + s->getPhone() + "]     : ");
            std::string dept  = readLine("Department [" + s->getDepartment() + "]: ");

            std::string semStr = readLine("Semester [" + std::to_string(s->getSemester()) + "]   : ");
            int semester = 0;
            if (!semStr.empty()) {
                std::istringstream iss(semStr);
                iss >> semester;
            }

            // Use current values if blank/zero input
            if (name.empty())  name  = s->getName();
            if (email.empty()) email = s->getEmail();
            if (phone.empty()) phone = s->getPhone();
            if (dept.empty())  dept  = s->getDepartment();
            if (semester <= 0) semester = s->getSemester();

            Student updatedData(rollNo, name, email, phone, dept, semester, s->getTotalFee());
            if (system.getStudentManager().updateStudent(rollNo, updatedData)) {
                std::cout << "  Student updated successfully!\n";
                autoSave();
            } else {
                std::cout << "  Error: Update failed.\n";
            }
            pauseScreen();
            break;
        }
        case 3: {
            printHeading("DEACTIVATE STUDENT");
            std::string rollNo = readLine("Enter roll number to deactivate: ");
            if (system.getStudentManager().removeStudent(rollNo)) {
                std::cout << "  Student deactivated successfully.\n";
                autoSave();
            } else {
                std::cout << "  Error: Active student '" << rollNo << "' not found.\n";
            }
            pauseScreen();
            break;
        }
        case 4: {
            printHeading("ALL ACTIVE STUDENTS");
            const auto& allStudents = system.getStudentManager().getAllStudents();
            bool found = false;
            for (const auto& s : allStudents) {
                if (s.isActive()) {
                    s.displayDetails();
                    std::cout << "----------------------------------------\n";
                    found = true;
                }
            }
            if (!found) {
                std::cout << "  No active students found.\n";
            }
            std::cout << "  Total active students: "
                      << system.getStudentManager().getTotalStudents() << "\n";
            pauseScreen();
            break;
        }
        case 5: {
            printHeading("SEARCH STUDENT");
            std::string rollNo = readLine("Enter roll number: ");
            const Student* s = system.getStudentManager().findStudent(rollNo);
            if (s != nullptr) {
                s->displayDetails();
                std::cout << "  Status: " << (s->isActive() ? "Active" : "Inactive") << "\n";
            } else {
                std::cout << "  Student '" << rollNo << "' not found.\n";
            }
            pauseScreen();
            break;
        }
        case 6: {
            printHeading("STUDENT PROFILE");
            std::string rollNo = readLine("Enter roll number: ");
            const Student* s = system.getStudentManager().findStudent(rollNo);
            if (s == nullptr) {
                std::cout << "  Student '" << rollNo << "' not found.\n";
                pauseScreen(); break;
            }

            // Demonstrate polymorphism through Person reference
            const Person& personRef = *s;
            personRef.displayDetails();

            std::cout << "  Status: " << (s->isActive() ? "Active" : "Inactive") << "\n";

            // Show enrollment information
            auto enrollments = system.getEnrollmentManager().getStudentEnrollments(rollNo);
            std::cout << "  Active Enrollments: " << enrollments.size() << "\n";
            for (const auto& e : enrollments) {
                std::cout << "    - " << e.getCourseCode()
                          << " (Enrolled: " << e.getEnrollmentDate() << ")\n";
            }

            // Show fee information
            double totalPaid = system.getFeeManager().calculateTotalPaid(rollNo);
            double outstanding = system.getFeeManager().calculateOutstanding(
                rollNo, system.getStudentManager());
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Total Fee    : " << s->getTotalFee() << "\n";
            std::cout << "  Total Paid   : " << totalPaid << "\n";
            std::cout << "  Outstanding  : " << outstanding << "\n";

            pauseScreen();
            break;
        }
        case 7:
            break;
        default:
            std::cout << "  Invalid choice. Please select 1-7.\n";
            break;
        }
    } while (choice != 7);
}

// ============================================================================
// 2. Faculty Management
// ============================================================================

void ConsoleUI::facultyMenu() {
    int choice = 0;
    do {
        printHeading("FACULTY MANAGEMENT");
        std::cout << "  1. Register faculty\n";
        std::cout << "  2. Update faculty details\n";
        std::cout << "  3. Deactivate faculty\n";
        std::cout << "  4. View all active faculty\n";
        std::cout << "  5. Search faculty by ID\n";
        std::cout << "  6. Back\n\n";
        choice = readInt("Enter your choice: ");
        if (std::cin.eof()) break;

        switch (choice) {
        case 1: {
            printHeading("REGISTER FACULTY");
            std::string fid   = readLine("Faculty ID   : ");
            std::string name  = readLine("Name         : ");
            if (fid.empty() || name.empty()) {
                std::cout << "  Error: Faculty ID and name cannot be empty.\n";
                pauseScreen(); break;
            }
            std::string email = readLine("Email        : ");
            std::string phone = readLine("Phone        : ");
            std::string dept  = readLine("Department   : ");
            std::string desig = readLine("Designation  : ");

            Faculty faculty(fid, name, email, phone, dept, desig);
            if (system.getFacultyManager().addFaculty(faculty)) {
                std::cout << "  Faculty registered successfully!\n";
                autoSave();
            } else {
                std::cout << "  Error: Faculty ID '" << fid << "' already exists.\n";
            }
            pauseScreen();
            break;
        }
        case 2: {
            printHeading("UPDATE FACULTY DETAILS");
            std::string fid = readLine("Enter faculty ID to update: ");
            Faculty* f = system.getFacultyManager().findFaculty(fid);
            if (f == nullptr || !f->isActive()) {
                std::cout << "  Error: Active faculty '" << fid << "' not found.\n";
                pauseScreen(); break;
            }
            std::cout << "  Current details:\n";
            f->displayDetails();

            std::cout << "\n  Enter new details (leave blank to keep current):\n";
            std::string name  = readLine("Name [" + f->getName() + "]           : ");
            std::string email = readLine("Email [" + f->getEmail() + "]         : ");
            std::string phone = readLine("Phone [" + f->getPhone() + "]         : ");
            std::string dept  = readLine("Department [" + f->getDepartment() + "]: ");
            std::string desig = readLine("Designation [" + f->getDesignation() + "]: ");

            if (name.empty())  name  = f->getName();
            if (email.empty()) email = f->getEmail();
            if (phone.empty()) phone = f->getPhone();
            if (dept.empty())  dept  = f->getDepartment();
            if (desig.empty()) desig = f->getDesignation();

            Faculty updatedData(fid, name, email, phone, dept, desig);
            if (system.getFacultyManager().updateFaculty(fid, updatedData)) {
                std::cout << "  Faculty updated successfully!\n";
                autoSave();
            } else {
                std::cout << "  Error: Update failed.\n";
            }
            pauseScreen();
            break;
        }
        case 3: {
            printHeading("DEACTIVATE FACULTY");
            std::string fid = readLine("Enter faculty ID to deactivate: ");
            if (system.getFacultyManager().removeFaculty(fid)) {
                std::cout << "  Faculty deactivated successfully.\n";
                autoSave();
            } else {
                std::cout << "  Error: Active faculty '" << fid << "' not found.\n";
            }
            pauseScreen();
            break;
        }
        case 4: {
            printHeading("ALL ACTIVE FACULTY");
            const auto& allFaculty = system.getFacultyManager().getAllFaculty();
            bool found = false;
            for (const auto& f : allFaculty) {
                if (f.isActive()) {
                    // Demonstrate polymorphism through Person reference
                    const Person& personRef = f;
                    personRef.displayDetails();
                    std::cout << "----------------------------------------\n";
                    found = true;
                }
            }
            if (!found) {
                std::cout << "  No active faculty found.\n";
            }
            std::cout << "  Total active faculty: "
                      << system.getFacultyManager().getTotalFaculty() << "\n";
            pauseScreen();
            break;
        }
        case 5: {
            printHeading("SEARCH FACULTY");
            std::string fid = readLine("Enter faculty ID: ");
            const Faculty* f = system.getFacultyManager().findFaculty(fid);
            if (f != nullptr) {
                f->displayDetails();
                std::cout << "  Status: " << (f->isActive() ? "Active" : "Inactive") << "\n";
            } else {
                std::cout << "  Faculty '" << fid << "' not found.\n";
            }
            pauseScreen();
            break;
        }
        case 6:
            break;
        default:
            std::cout << "  Invalid choice. Please select 1-6.\n";
            break;
        }
    } while (choice != 6);
}

// ============================================================================
// 3. Course Management
// ============================================================================

void ConsoleUI::courseMenu() {
    int choice = 0;
    do {
        printHeading("COURSE MANAGEMENT");
        std::cout << "  1. Add course\n";
        std::cout << "  2. Update course\n";
        std::cout << "  3. Deactivate course\n";
        std::cout << "  4. View all active courses\n";
        std::cout << "  5. Search course by code\n";
        std::cout << "  6. Assign faculty to course\n";
        std::cout << "  7. Back\n\n";
        choice = readInt("Enter your choice: ");
        if (std::cin.eof()) break;

        switch (choice) {
        case 1: {
            printHeading("ADD COURSE");
            std::string code = readLine("Course Code : ");
            std::string name = readLine("Course Name : ");
            if (code.empty() || name.empty()) {
                std::cout << "  Error: Course code and name cannot be empty.\n";
                pauseScreen(); break;
            }
            int credits  = readInt("Credits     : ");
            if (credits <= 0) {
                std::cout << "  Error: Credits must be a positive number.\n";
                pauseScreen(); break;
            }
            int capacity = readInt("Capacity    : ");
            if (capacity <= 0) {
                std::cout << "  Error: Capacity must be a positive number.\n";
                pauseScreen(); break;
            }
            std::string facultyId = readLine("Faculty ID (leave blank for none): ");

            Course course(code, name, credits, capacity, facultyId);
            if (system.getCourseManager().addCourse(course)) {
                std::cout << "  Course added successfully!\n";
                autoSave();
            } else {
                std::cout << "  Error: Course code '" << code << "' already exists.\n";
            }
            pauseScreen();
            break;
        }
        case 2: {
            printHeading("UPDATE COURSE");
            std::string code = readLine("Enter course code to update: ");
            Course* c = system.getCourseManager().findCourse(code);
            if (c == nullptr || !c->isActive()) {
                std::cout << "  Error: Active course '" << code << "' not found.\n";
                pauseScreen(); break;
            }
            std::cout << "  Current details:\n";
            c->displayDetails();

            std::cout << "\n  Enter new details (enter 0 to keep current number):\n";
            std::string name = readLine("Course Name [" + c->getCourseName() + "]: ");
            int credits  = readInt("Credits [" + std::to_string(c->getCredits()) + "]       : ");
            int capacity = readInt("Capacity [" + std::to_string(c->getCapacity()) + "]     : ");

            if (name.empty()) name = c->getCourseName();
            if (credits  <= 0) credits  = c->getCredits();
            if (capacity <= 0) capacity = c->getCapacity();

            Course updatedData(code, name, credits, capacity, c->getFacultyId());
            if (system.getCourseManager().updateCourse(code, updatedData)) {
                std::cout << "  Course updated successfully!\n";
                autoSave();
            } else {
                std::cout << "  Error: Update failed.\n";
            }
            pauseScreen();
            break;
        }
        case 3: {
            printHeading("DEACTIVATE COURSE");
            std::string code = readLine("Enter course code to deactivate: ");
            if (system.getCourseManager().removeCourse(code)) {
                std::cout << "  Course deactivated successfully.\n";
                autoSave();
            } else {
                std::cout << "  Error: Active course '" << code << "' not found.\n";
            }
            pauseScreen();
            break;
        }
        case 4: {
            printHeading("ALL ACTIVE COURSES");
            const auto& allCourses = system.getCourseManager().getAllCourses();
            bool found = false;
            for (const auto& c : allCourses) {
                if (c.isActive()) {
                    c.displayDetails();
                    int enrolled = system.getEnrollmentManager()
                                       .getEnrolledCount(c.getCourseCode());
                    std::cout << "  Enrolled : " << enrolled
                              << " / " << c.getCapacity() << "\n";
                    std::cout << "----------------------------------------\n";
                    found = true;
                }
            }
            if (!found) {
                std::cout << "  No active courses found.\n";
            }
            std::cout << "  Total active courses: "
                      << system.getCourseManager().getTotalCourses() << "\n";
            pauseScreen();
            break;
        }
        case 5: {
            printHeading("SEARCH COURSE");
            std::string code = readLine("Enter course code: ");
            const Course* c = system.getCourseManager().findCourse(code);
            if (c != nullptr) {
                c->displayDetails();
                std::cout << "  Status: " << (c->isActive() ? "Active" : "Inactive") << "\n";
                int enrolled = system.getEnrollmentManager()
                                   .getEnrolledCount(c->getCourseCode());
                std::cout << "  Enrolled: " << enrolled
                          << " / " << c->getCapacity() << "\n";
            } else {
                std::cout << "  Course '" << code << "' not found.\n";
            }
            pauseScreen();
            break;
        }
        case 6: {
            printHeading("ASSIGN FACULTY TO COURSE");
            std::string code = readLine("Enter course code   : ");
            std::string fid  = readLine("Enter faculty ID    : ");
            if (code.empty() || fid.empty()) {
                std::cout << "  Error: Course code and faculty ID cannot be empty.\n";
                pauseScreen(); break;
            }
            if (system.getCourseManager().assignFaculty(
                    code, fid, system.getFacultyManager())) {
                std::cout << "  Faculty assigned to course successfully!\n";
                autoSave();
            } else {
                std::cout << "  Error: Assignment failed. Check that both the course\n";
                std::cout << "         and faculty exist and are active.\n";
            }
            pauseScreen();
            break;
        }
        case 7:
            break;
        default:
            std::cout << "  Invalid choice. Please select 1-7.\n";
            break;
        }
    } while (choice != 7);
}

// ============================================================================
// 4. Student Enrollment
// ============================================================================

void ConsoleUI::enrollmentMenu() {
    int choice = 0;
    do {
        printHeading("STUDENT ENROLLMENT");
        std::cout << "  1. Enroll student in course\n";
        std::cout << "  2. Drop course\n";
        std::cout << "  3. View student's active courses\n";
        std::cout << "  4. View all enrollment records\n";
        std::cout << "  5. Back\n\n";
        choice = readInt("Enter your choice: ");
        if (std::cin.eof()) break;

        switch (choice) {
        case 1: {
            printHeading("ENROLL STUDENT");
            std::string rollNo = readLine("Student Roll Number : ");
            std::string code   = readLine("Course Code         : ");
            std::string date   = readLine("Enrollment Date     : ");
            if (rollNo.empty() || code.empty() || date.empty()) {
                std::cout << "  Error: Roll number, course code, and date cannot be empty.\n";
                pauseScreen(); break;
            }

            if (system.getEnrollmentManager().enrollStudent(
                    rollNo, code, date,
                    system.getStudentManager(),
                    system.getCourseManager())) {
                std::cout << "  Student enrolled successfully!\n";
                int enrolled = system.getEnrollmentManager().getEnrolledCount(code);
                const Course* c = system.getCourseManager().findCourse(code);
                if (c != nullptr) {
                    std::cout << "  Current enrollment: " << enrolled
                              << " / " << c->getCapacity() << "\n";
                }
                autoSave();
            } else {
                std::cout << "  Error: Enrollment failed. Possible reasons:\n";
                std::cout << "    - Student or course does not exist or is inactive.\n";
                std::cout << "    - Student is already enrolled in this course.\n";
                std::cout << "    - Course has reached maximum capacity.\n";
            }
            pauseScreen();
            break;
        }
        case 2: {
            printHeading("DROP COURSE");
            std::string rollNo = readLine("Student Roll Number : ");
            std::string code   = readLine("Course Code         : ");

            if (system.getEnrollmentManager().dropCourse(rollNo, code)) {
                std::cout << "  Course dropped successfully.\n";
                autoSave();
            } else {
                std::cout << "  Error: No active enrollment found for student '"
                          << rollNo << "' in course '" << code << "'.\n";
            }
            pauseScreen();
            break;
        }
        case 3: {
            printHeading("STUDENT'S ACTIVE COURSES");
            std::string rollNo = readLine("Enter roll number: ");
            auto enrollments = system.getEnrollmentManager()
                                   .getStudentEnrollments(rollNo);
            if (enrollments.empty()) {
                std::cout << "  No active enrollments found for '"
                          << rollNo << "'.\n";
            } else {
                std::cout << "  Active courses for " << rollNo << ":\n";
                for (const auto& e : enrollments) {
                    e.displayDetails();
                    std::cout << "  ----------------------------------------\n";
                }
                std::cout << "  Total active enrollments: "
                          << enrollments.size() << "\n";
            }
            pauseScreen();
            break;
        }
        case 4: {
            printHeading("ALL ENROLLMENT RECORDS");
            const auto& allEnrollments = system.getEnrollmentManager()
                                              .getAllEnrollments();
            if (allEnrollments.empty()) {
                std::cout << "  No enrollment records found.\n";
            } else {
                for (const auto& e : allEnrollments) {
                    e.displayDetails();
                    std::cout << "  Status: "
                              << (e.isActive() ? "Active" : "Dropped") << "\n";
                    std::cout << "  ----------------------------------------\n";
                }
                std::cout << "  Total records: " << allEnrollments.size() << "\n";
            }
            pauseScreen();
            break;
        }
        case 5:
            break;
        default:
            std::cout << "  Invalid choice. Please select 1-5.\n";
            break;
        }
    } while (choice != 5);
}

// ============================================================================
// 5. Attendance Management
// ============================================================================

void ConsoleUI::attendanceMenu() {
    int choice = 0;
    do {
        printHeading("ATTENDANCE MANAGEMENT");
        std::cout << "  1. Mark attendance\n";
        std::cout << "  2. Correct attendance record\n";
        std::cout << "  3. View student attendance\n";
        std::cout << "  4. Check attendance percentage\n";
        std::cout << "  5. Generate attendance report\n";
        std::cout << "  6. Back\n\n";
        choice = readInt("Enter your choice: ");
        if (std::cin.eof()) break;

        switch (choice) {
        case 1: {
            printHeading("MARK ATTENDANCE");
            std::string rollNo = readLine("Student Roll Number : ");
            std::string code   = readLine("Course Code         : ");
            std::string date   = readLine("Date                : ");
            if (rollNo.empty() || code.empty() || date.empty()) {
                std::cout << "  Error: Roll number, course code, and date cannot be empty.\n";
                pauseScreen(); break;
            }
            int status = readInt("Status (1=Present, 0=Absent): ");
            if (status != 0 && status != 1) {
                std::cout << "  Error: Status must be 1 (Present) or 0 (Absent).\n";
                pauseScreen(); break;
            }

            bool present = (status == 1);
            if (system.getAttendanceManager().markAttendance(
                    rollNo, code, date, present,
                    system.getStudentManager(),
                    system.getCourseManager(),
                    system.getEnrollmentManager())) {
                std::cout << "  Attendance marked: "
                          << (present ? "Present" : "Absent") << "\n";
                autoSave();
            } else {
                std::cout << "  Error: Could not mark attendance. Possible reasons:\n";
                std::cout << "    - Student or course does not exist or is inactive.\n";
                std::cout << "    - Student is not enrolled in this course.\n";
                std::cout << "    - Attendance already marked for this date.\n";
            }
            pauseScreen();
            break;
        }
        case 2: {
            printHeading("CORRECT ATTENDANCE RECORD");
            std::string rollNo = readLine("Student Roll Number : ");
            std::string code   = readLine("Course Code         : ");
            std::string date   = readLine("Date                : ");
            if (rollNo.empty() || code.empty() || date.empty()) {
                std::cout << "  Error: Roll number, course code, and date cannot be empty.\n";
                pauseScreen(); break;
            }
            int status = readInt("New Status (1=Present, 0=Absent): ");
            if (status != 0 && status != 1) {
                std::cout << "  Error: Status must be 1 (Present) or 0 (Absent).\n";
                pauseScreen(); break;
            }

            bool present = (status == 1);
            if (system.getAttendanceManager().updateAttendance(
                    rollNo, code, date, present)) {
                std::cout << "  Attendance corrected to: "
                          << (present ? "Present" : "Absent") << "\n";
                autoSave();
            } else {
                std::cout << "  Error: No attendance record found for the "
                          << "given details.\n";
            }
            pauseScreen();
            break;
        }
        case 3: {
            printHeading("STUDENT ATTENDANCE RECORDS");
            std::string rollNo = readLine("Enter roll number: ");
            auto records = system.getAttendanceManager()
                               .getStudentAttendance(rollNo);
            if (records.empty()) {
                std::cout << "  No attendance records found for '"
                          << rollNo << "'.\n";
            } else {
                for (const auto& r : records) {
                    r.displayDetails();
                    std::cout << "  ----------------------------------------\n";
                }
                std::cout << "  Total records: " << records.size() << "\n";
            }
            pauseScreen();
            break;
        }
        case 4: {
            printHeading("ATTENDANCE PERCENTAGE");
            std::string rollNo = readLine("Student Roll Number : ");
            std::string code   = readLine("Course Code         : ");

            int totalClasses = system.getAttendanceManager()
                                   .getTotalClasses(rollNo, code);
            if (totalClasses == 0) {
                std::cout << "  No attendance records found.\n";
            } else {
                double pct = system.getAttendanceManager()
                                 .calculatePercentage(rollNo, code);
                std::cout << std::fixed << std::setprecision(2);
                std::cout << "  Total Classes : " << totalClasses << "\n";
                std::cout << "  Attendance    : " << pct << "%\n";
            }
            pauseScreen();
            break;
        }
        case 5: {
            printHeading("ATTENDANCE REPORT");
            double threshold = readDouble(
                "Enter minimum attendance requirement (%): ");
            if (threshold < 0.0 || threshold > 100.0) {
                std::cout << "  Error: Requirement threshold must be between 0 and 100.\n";
                pauseScreen(); break;
            }

            auto report = system.getReportManager().getAttendanceReport(
                system.getStudentManager(),
                system.getEnrollmentManager(),
                system.getAttendanceManager(),
                threshold);

            if (report.empty()) {
                std::cout << "  No attendance data available.\n";
            } else {
                std::cout << std::fixed << std::setprecision(2);
                std::cout << std::left
                          << std::setw(14) << "Roll No"
                          << std::setw(20) << "Student Name"
                          << std::setw(12) << "Course"
                          << std::setw(10) << "Classes"
                          << std::setw(14) << "Attendance %"
                          << "Status" << "\n";
                std::cout << std::string(80, '-') << "\n";
                for (const auto& row : report) {
                    std::cout << std::left
                              << std::setw(14) << row.rollNo
                              << std::setw(20) << row.studentName
                              << std::setw(12) << row.courseCode
                              << std::setw(10) << row.totalClasses
                              << std::setw(14) << row.attendancePercentage
                              << (row.belowRequirement ? "BELOW" : "OK")
                              << "\n";
                }
                std::cout << std::string(80, '-') << "\n";

                // Show students below requirement
                std::cout << "\n  Students below " << threshold
                          << "% attendance:\n";
                bool anyBelow = false;
                for (const auto& row : report) {
                    if (row.belowRequirement) {
                        std::cout << "    " << row.rollNo << " - "
                                  << row.studentName
                                  << " (" << row.courseCode << ": "
                                  << row.attendancePercentage << "%)\n";
                        anyBelow = true;
                    }
                }
                if (!anyBelow) {
                    std::cout << "    None. All students meet the "
                              << "requirement.\n";
                }
            }
            pauseScreen();
            break;
        }
        case 6:
            break;
        default:
            std::cout << "  Invalid choice. Please select 1-6.\n";
            break;
        }
    } while (choice != 6);
}

// ============================================================================
// 6. Fee Management
// ============================================================================

void ConsoleUI::feeMenu() {
    int choice = 0;
    do {
        printHeading("FEE MANAGEMENT");
        std::cout << "  1. View student fee details\n";
        std::cout << "  2. Record fee payment\n";
        std::cout << "  3. View payment history\n";
        std::cout << "  4. Generate fee receipt\n";
        std::cout << "  5. View total college fee collection\n";
        std::cout << "  6. Back\n\n";
        choice = readInt("Enter your choice: ");
        if (std::cin.eof()) break;

        switch (choice) {
        case 1: {
            printHeading("STUDENT FEE DETAILS");
            std::string rollNo = readLine("Enter roll number: ");
            const Student* s = system.getStudentManager().findStudent(rollNo);
            if (s == nullptr) {
                std::cout << "  Student '" << rollNo << "' not found.\n";
                pauseScreen(); break;
            }

            double totalPaid = system.getFeeManager().calculateTotalPaid(rollNo);
            double outstanding = system.getFeeManager().calculateOutstanding(
                rollNo, system.getStudentManager());

            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Student     : " << s->getName()
                      << " (" << rollNo << ")\n";
            std::cout << "  Total Fee   : " << s->getTotalFee() << "\n";
            std::cout << "  Total Paid  : " << totalPaid << "\n";
            std::cout << "  Outstanding : " << outstanding << "\n";
            pauseScreen();
            break;
        }
        case 2: {
            printHeading("RECORD FEE PAYMENT");
            std::string rollNo = readLine("Student Roll Number : ");
            const Student* s = system.getStudentManager().findStudent(rollNo);
            if (s == nullptr || !s->isActive()) {
                std::cout << "  Error: Active student '" << rollNo
                          << "' not found.\n";
                pauseScreen(); break;
            }

            double outstanding = system.getFeeManager().calculateOutstanding(
                rollNo, system.getStudentManager());
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Outstanding balance: " << outstanding << "\n";

            if (outstanding <= 0.0) {
                std::cout << "  No outstanding fees for this student.\n";
                pauseScreen(); break;
            }

            std::string receiptNo = readLine("Receipt Number      : ");
            if (receiptNo.empty()) {
                std::cout << "  Error: Receipt number cannot be empty.\n";
                pauseScreen(); break;
            }
            // Check for duplicate receipt
            if (system.getFeeManager().findPaymentByReceipt(receiptNo) != nullptr) {
                std::cout << "  Error: Receipt number '" << receiptNo
                          << "' already exists.\n";
                pauseScreen(); break;
            }

            double amount = readDouble("Amount              : ");
            if (amount <= 0.0) {
                std::cout << "  Error: Amount must be positive.\n";
                pauseScreen(); break;
            }
            if (amount > outstanding) {
                std::cout << "  Error: Amount exceeds outstanding balance of "
                          << outstanding << ".\n";
                pauseScreen(); break;
            }

            std::string payDate = readLine("Payment Date        : ");
            std::string payMode = readLine("Payment Mode        : ");

            FeePayment payment(receiptNo, rollNo, amount, payDate, payMode);
            if (system.getFeeManager().recordPayment(
                    payment, system.getStudentManager())) {
                std::cout << "  Payment recorded successfully!\n";
                autoSave();
            } else {
                std::cout << "  Error: Payment recording failed.\n";
            }
            pauseScreen();
            break;
        }
        case 3: {
            printHeading("PAYMENT HISTORY");
            std::string rollNo = readLine("Enter roll number: ");
            auto history = system.getFeeManager().getPaymentHistory(rollNo);
            if (history.empty()) {
                std::cout << "  No payment records found for '"
                          << rollNo << "'.\n";
            } else {
                std::cout << std::fixed << std::setprecision(2);
                for (const auto& p : history) {
                    p.displayDetails();
                    std::cout << "  ----------------------------------------\n";
                }
                double totalPaid = system.getFeeManager()
                                       .calculateTotalPaid(rollNo);
                std::cout << "  Total payments: " << history.size()
                          << "  |  Total paid: " << totalPaid << "\n";
            }
            pauseScreen();
            break;
        }
        case 4: {
            printHeading("FEE RECEIPT");
            std::string receiptNo = readLine("Enter receipt number: ");
            std::string receipt = system.getFeeManager()
                                      .generateReceipt(receiptNo);
            if (receipt.empty()) {
                std::cout << "  Receipt '" << receiptNo << "' not found.\n";
            } else {
                std::cout << "\n" << receipt;
            }
            pauseScreen();
            break;
        }
        case 5: {
            printHeading("TOTAL COLLEGE FEE COLLECTION");
            double total = system.getFeeManager().getTotalCollection();
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Total fee collection: " << total << "\n";
            std::cout << "  Total payments made : "
                      << system.getFeeManager().getAllPayments().size() << "\n";
            pauseScreen();
            break;
        }
        case 6:
            break;
        default:
            std::cout << "  Invalid choice. Please select 1-6.\n";
            break;
        }
    } while (choice != 6);
}

// ============================================================================
// 7. Reports & Analytics
// ============================================================================

void ConsoleUI::reportMenu() {
    int choice = 0;
    do {
        printHeading("REPORTS & ANALYTICS");
        std::cout << "  1. College dashboard\n";
        std::cout << "  2. Student fee report\n";
        std::cout << "  3. Attendance report\n";
        std::cout << "  4. Course enrollment report\n";
        std::cout << "  5. Back\n\n";
        choice = readInt("Enter your choice: ");
        if (std::cin.eof()) break;

        switch (choice) {
        case 1: {
            printHeading("COLLEGE DASHBOARD");
            DashboardSummary ds = system.getReportManager().getDashboardSummary(
                system.getStudentManager(),
                system.getFacultyManager(),
                system.getCourseManager(),
                system.getEnrollmentManager(),
                system.getFeeManager());

            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Total Students          : " << ds.totalStudents << "\n";
            std::cout << "  Total Faculty           : " << ds.totalFaculty << "\n";
            std::cout << "  Total Courses           : " << ds.totalCourses << "\n";
            std::cout << "  Total Active Enrollments: " << ds.totalEnrollments << "\n";
            std::cout << "  Total Fee Collection    : " << ds.totalFeeCollection << "\n";
            std::cout << "  Total Outstanding Fees  : " << ds.totalOutstandingFees << "\n";
            pauseScreen();
            break;
        }
        case 2: {
            printHeading("STUDENT FEE REPORT");
            auto report = system.getReportManager().getFeeReport(
                system.getStudentManager(),
                system.getFeeManager());

            if (report.empty()) {
                std::cout << "  No active students found.\n";
            } else {
                std::cout << std::fixed << std::setprecision(2);
                std::cout << std::left
                          << std::setw(14) << "Roll No"
                          << std::setw(20) << "Student Name"
                          << std::setw(14) << "Total Fee"
                          << std::setw(14) << "Total Paid"
                          << "Outstanding" << "\n";
                std::cout << std::string(76, '-') << "\n";
                for (const auto& row : report) {
                    std::cout << std::left
                              << std::setw(14) << row.rollNo
                              << std::setw(20) << row.studentName
                              << std::setw(14) << row.totalFee
                              << std::setw(14) << row.totalPaid
                              << row.outstanding << "\n";
                }
                std::cout << std::string(76, '-') << "\n";
            }
            pauseScreen();
            break;
        }
        case 3: {
            printHeading("ATTENDANCE REPORT");
            double threshold = readDouble(
                "Enter minimum attendance requirement (%): ");
            if (threshold < 0.0 || threshold > 100.0) {
                std::cout << "  Error: Requirement threshold must be between 0 and 100.\n";
                pauseScreen(); break;
            }

            auto report = system.getReportManager().getAttendanceReport(
                system.getStudentManager(),
                system.getEnrollmentManager(),
                system.getAttendanceManager(),
                threshold);

            if (report.empty()) {
                std::cout << "  No attendance data available.\n";
            } else {
                std::cout << std::fixed << std::setprecision(2);
                std::cout << std::left
                          << std::setw(14) << "Roll No"
                          << std::setw(20) << "Student Name"
                          << std::setw(12) << "Course"
                          << std::setw(10) << "Classes"
                          << std::setw(14) << "Attendance %"
                          << "Status" << "\n";
                std::cout << std::string(80, '-') << "\n";
                for (const auto& row : report) {
                    std::cout << std::left
                              << std::setw(14) << row.rollNo
                              << std::setw(20) << row.studentName
                              << std::setw(12) << row.courseCode
                              << std::setw(10) << row.totalClasses
                              << std::setw(14) << row.attendancePercentage
                              << (row.belowRequirement ? "BELOW" : "OK")
                              << "\n";
                }
                std::cout << std::string(80, '-') << "\n";
            }
            pauseScreen();
            break;
        }
        case 4: {
            printHeading("COURSE ENROLLMENT REPORT");
            auto report = system.getReportManager().getEnrollmentReport(
                system.getCourseManager(),
                system.getEnrollmentManager());

            if (report.empty()) {
                std::cout << "  No active courses found.\n";
            } else {
                std::cout << std::left
                          << std::setw(12) << "Code"
                          << std::setw(22) << "Course Name"
                          << std::setw(14) << "Faculty ID"
                          << std::setw(10) << "Enrolled"
                          << "Capacity" << "\n";
                std::cout << std::string(70, '-') << "\n";
                for (const auto& row : report) {
                    std::cout << std::left
                              << std::setw(12) << row.courseCode
                              << std::setw(22) << row.courseName
                              << std::setw(14) << (row.facultyId.empty()
                                                       ? "N/A" : row.facultyId)
                              << std::setw(10) << row.enrolledStudents
                              << row.capacity << "\n";
                }
                std::cout << std::string(70, '-') << "\n";
            }
            pauseScreen();
            break;
        }
        case 5:
            break;
        default:
            std::cout << "  Invalid choice. Please select 1-5.\n";
            break;
        }
    } while (choice != 5);
}

// ============================================================================
// 8. Data Management
// ============================================================================

void ConsoleUI::dataMenu() {
    int choice = 0;
    do {
        printHeading("DATA MANAGEMENT");
        std::cout << "  1. Save all data\n";
        std::cout << "  2. Reload data from files\n";
        std::cout << "  3. Back\n\n";
        choice = readInt("Enter your choice: ");
        if (std::cin.eof()) break;

        switch (choice) {
        case 1:
            if (system.saveAllData()) {
                std::cout << "  All data saved successfully.\n";
            } else {
                std::cout << "  Error: Failed to save data.\n";
            }
            pauseScreen();
            break;
        case 2: {
            std::string confirm = readLine(
                "  This will replace all in-memory data. Continue? (yes/no): ");
            if (confirm == "yes" || confirm == "YES"
                || confirm == "y" || confirm == "Y") {
                if (system.loadAllData()) {
                    std::cout << "  Data reloaded successfully.\n";
                } else {
                    std::cout << "  Warning: Some data files could not be loaded.\n";
                }
            } else {
                std::cout << "  Reload cancelled.\n";
            }
            pauseScreen();
            break;
        }
        case 3:
            break;
        default:
            std::cout << "  Invalid choice. Please select 1-3.\n";
            break;
        }
    } while (choice != 3);
}

} // namespace college
