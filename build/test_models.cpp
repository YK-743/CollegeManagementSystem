// Temporary smoke test for all seven model classes.
// This file lives under build/ and is not part of the main application.

#include "college/Models.h"
#include <iostream>
#include <string>

int main() {
    using namespace college;

    std::cout << "========================================" << std::endl;
    std::cout << "  MODEL CLASSES SMOKE TEST" << std::endl;
    std::cout << "========================================" << std::endl;

    // --- Test 1: Student ---
    std::cout << "\n[TEST 1] Student construction and getters" << std::endl;
    Student student("22CS101", "Ravi Kumar", "ravi@college.edu", "9876543210",
                    "Computer Science", 4, 75000.0);
    student.displayDetails();
    std::cout << "getRollNo()     = " << student.getRollNo() << std::endl;
    std::cout << "getDepartment() = " << student.getDepartment() << std::endl;
    std::cout << "getSemester()   = " << student.getSemester() << std::endl;
    std::cout << "getTotalFee()   = " << student.getTotalFee() << std::endl;
    std::cout << "isActive()      = " << (student.isActive() ? "true" : "false") << std::endl;

    // --- Test 2: Faculty ---
    std::cout << "\n[TEST 2] Faculty construction and getters" << std::endl;
    Faculty faculty("FAC001", "Dr. Sharma", "sharma@college.edu", "9988776655",
                    "Computer Science", "Professor");
    faculty.displayDetails();
    std::cout << "getFacultyId()  = " << faculty.getFacultyId() << std::endl;
    std::cout << "getDesignation()= " << faculty.getDesignation() << std::endl;

    // --- Test 3: Course ---
    std::cout << "\n[TEST 3] Course construction and getters" << std::endl;
    Course course("CS201", "Data Structures", 4, 60, "");
    course.displayDetails();
    std::cout << "Assigning faculty FAC001..." << std::endl;
    course.assignFaculty("FAC001");
    std::cout << "getFacultyId()  = " << course.getFacultyId() << std::endl;

    // --- Test 4: Enrollment ---
    std::cout << "\n[TEST 4] Enrollment construction and drop" << std::endl;
    Enrollment enrollment("22CS101", "CS201", "2026-08-01");
    enrollment.displayDetails();
    std::cout << "Dropping enrollment..." << std::endl;
    enrollment.drop();
    std::cout << "isActive()      = " << (enrollment.isActive() ? "true" : "false") << std::endl;

    // --- Test 5: AttendanceRecord ---
    std::cout << "\n[TEST 5] AttendanceRecord construction and setter" << std::endl;
    AttendanceRecord attendance("22CS101", "CS201", "2026-09-15", true);
    attendance.displayDetails();
    std::cout << "Marking absent..." << std::endl;
    attendance.setPresent(false);
    std::cout << "isPresent()     = " << (attendance.isPresent() ? "true" : "false") << std::endl;

    // --- Test 6: FeePayment ---
    std::cout << "\n[TEST 6] FeePayment construction and getters" << std::endl;
    FeePayment payment("REC001", "22CS101", 25000.0, "2026-07-15", "UPI");
    payment.displayDetails();
    std::cout << "getAmount()     = " << payment.getAmount() << std::endl;
    std::cout << "getPaymentMode()= " << payment.getPaymentMode() << std::endl;

    // --- Test 7: Polymorphism via Person pointers ---
    std::cout << "\n[TEST 7] Polymorphism - Person pointers" << std::endl;
    std::cout << "Calling displayDetails() through Person*:" << std::endl;

    Person* personPtr1 = &student;
    Person* personPtr2 = &faculty;

    std::cout << "\nPerson pointer -> Student:" << std::endl;
    personPtr1->displayDetails();

    std::cout << "\nPerson pointer -> Faculty:" << std::endl;
    personPtr2->displayDetails();

    // --- Test 8: Student updateDetails ---
    std::cout << "\n[TEST 8] Student updateDetails" << std::endl;
    student.updateDetails("Ravi K. Sharma", "ravi.sharma@college.edu",
                          "9876500000", "Information Technology", 5);
    std::cout << "After update:" << std::endl;
    student.displayDetails();

    // --- Test 9: Faculty setActive ---
    std::cout << "\n[TEST 9] Faculty setActive(false)" << std::endl;
    faculty.setActive(false);
    std::cout << "isActive()      = " << (faculty.isActive() ? "true" : "false") << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << "  ALL TESTS PASSED" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
