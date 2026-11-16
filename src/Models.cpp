// College Management System - Model Classes Implementation

#include "college/Models.h"
#include <iostream>

namespace college {

// ============================================================================
// Person Implementation
// ============================================================================

Person::Person(const std::string& id, const std::string& name,
               const std::string& email, const std::string& phone)
    : id(id), name(name), email(email), phone(phone) {
}

Person::~Person() {
}

const std::string& Person::getId() const { return id; }
const std::string& Person::getName() const { return name; }
const std::string& Person::getEmail() const { return email; }
const std::string& Person::getPhone() const { return phone; }

void Person::setName(const std::string& name) { this->name = name; }
void Person::setEmail(const std::string& email) { this->email = email; }
void Person::setPhone(const std::string& phone) { this->phone = phone; }

// ============================================================================
// Student Implementation
// ============================================================================

Student::Student(const std::string& rollNo, const std::string& name,
                 const std::string& email, const std::string& phone,
                 const std::string& department, int semester, double totalFee)
    : Person(rollNo, name, email, phone),
      department(department), semester(semester), totalFee(totalFee), active(true) {
}

const std::string& Student::getRollNo() const { return getId(); }
const std::string& Student::getDepartment() const { return department; }
int Student::getSemester() const { return semester; }
double Student::getTotalFee() const { return totalFee; }
bool Student::isActive() const { return active; }

void Student::updateDetails(const std::string& name, const std::string& email,
                            const std::string& phone, const std::string& department,
                            int semester) {
    setName(name);
    setEmail(email);
    setPhone(phone);
    this->department = department;
    this->semester = semester;
}

void Student::setActive(bool status) { active = status; }

void Student::displayDetails() const {
    std::cout << "--- Student Details ---" << std::endl;
    std::cout << "Roll No     : " << getRollNo() << std::endl;
    std::cout << "Name        : " << getName() << std::endl;
    std::cout << "Email       : " << getEmail() << std::endl;
    std::cout << "Phone       : " << getPhone() << std::endl;
    std::cout << "Department  : " << department << std::endl;
    std::cout << "Semester    : " << semester << std::endl;
    std::cout << "Total Fee   : " << totalFee << std::endl;
    std::cout << "Status      : " << (active ? "Active" : "Inactive") << std::endl;
}

// ============================================================================
// Faculty Implementation
// ============================================================================

Faculty::Faculty(const std::string& facultyId, const std::string& name,
                 const std::string& email, const std::string& phone,
                 const std::string& department, const std::string& designation)
    : Person(facultyId, name, email, phone),
      department(department), designation(designation), active(true) {
}

const std::string& Faculty::getFacultyId() const { return getId(); }
const std::string& Faculty::getDepartment() const { return department; }
const std::string& Faculty::getDesignation() const { return designation; }
bool Faculty::isActive() const { return active; }

void Faculty::updateDetails(const std::string& name, const std::string& email,
                            const std::string& phone, const std::string& department,
                            const std::string& designation) {
    setName(name);
    setEmail(email);
    setPhone(phone);
    this->department = department;
    this->designation = designation;
}

void Faculty::setActive(bool status) { active = status; }

void Faculty::displayDetails() const {
    std::cout << "--- Faculty Details ---" << std::endl;
    std::cout << "Faculty ID  : " << getFacultyId() << std::endl;
    std::cout << "Name        : " << getName() << std::endl;
    std::cout << "Email       : " << getEmail() << std::endl;
    std::cout << "Phone       : " << getPhone() << std::endl;
    std::cout << "Department  : " << department << std::endl;
    std::cout << "Designation : " << designation << std::endl;
    std::cout << "Status      : " << (active ? "Active" : "Inactive") << std::endl;
}

// ============================================================================
// Course Implementation
// ============================================================================

Course::Course(const std::string& courseCode, const std::string& courseName,
               int credits, int capacity, const std::string& facultyId)
    : courseCode(courseCode), courseName(courseName), credits(credits),
      capacity(capacity), facultyId(facultyId), active(true) {
}

const std::string& Course::getCourseCode() const { return courseCode; }
const std::string& Course::getCourseName() const { return courseName; }
int Course::getCredits() const { return credits; }
int Course::getCapacity() const { return capacity; }
const std::string& Course::getFacultyId() const { return facultyId; }
bool Course::isActive() const { return active; }

void Course::assignFaculty(const std::string& facultyId) {
    this->facultyId = facultyId;
}

void Course::updateDetails(const std::string& courseName, int credits, int capacity) {
    this->courseName = courseName;
    this->credits = credits;
    this->capacity = capacity;
}

void Course::setActive(bool status) { active = status; }

void Course::displayDetails() const {
    std::cout << "--- Course Details ---" << std::endl;
    std::cout << "Course Code : " << courseCode << std::endl;
    std::cout << "Course Name : " << courseName << std::endl;
    std::cout << "Credits     : " << credits << std::endl;
    std::cout << "Capacity    : " << capacity << std::endl;
    std::cout << "Faculty ID  : " << (facultyId.empty() ? "Not Assigned" : facultyId) << std::endl;
    std::cout << "Status      : " << (active ? "Active" : "Inactive") << std::endl;
}

// ============================================================================
// Enrollment Implementation
// ============================================================================

Enrollment::Enrollment(const std::string& studentRollNo, const std::string& courseCode,
                       const std::string& enrollmentDate)
    : studentRollNo(studentRollNo), courseCode(courseCode),
      enrollmentDate(enrollmentDate), active(true) {
}

const std::string& Enrollment::getStudentRollNo() const { return studentRollNo; }
const std::string& Enrollment::getCourseCode() const { return courseCode; }
const std::string& Enrollment::getEnrollmentDate() const { return enrollmentDate; }
bool Enrollment::isActive() const { return active; }

void Enrollment::drop() { active = false; }

void Enrollment::displayDetails() const {
    std::cout << "--- Enrollment Details ---" << std::endl;
    std::cout << "Student Roll No  : " << studentRollNo << std::endl;
    std::cout << "Course Code      : " << courseCode << std::endl;
    std::cout << "Enrollment Date  : " << enrollmentDate << std::endl;
    std::cout << "Status           : " << (active ? "Active" : "Dropped") << std::endl;
}

// ============================================================================
// AttendanceRecord Implementation
// ============================================================================

AttendanceRecord::AttendanceRecord(const std::string& studentRollNo,
                                   const std::string& courseCode,
                                   const std::string& date, bool present)
    : studentRollNo(studentRollNo), courseCode(courseCode),
      date(date), present(present) {
}

const std::string& AttendanceRecord::getStudentRollNo() const { return studentRollNo; }
const std::string& AttendanceRecord::getCourseCode() const { return courseCode; }
const std::string& AttendanceRecord::getDate() const { return date; }
bool AttendanceRecord::isPresent() const { return present; }

void AttendanceRecord::setPresent(bool status) { present = status; }

void AttendanceRecord::displayDetails() const {
    std::cout << "--- Attendance Record ---" << std::endl;
    std::cout << "Student Roll No : " << studentRollNo << std::endl;
    std::cout << "Course Code     : " << courseCode << std::endl;
    std::cout << "Date            : " << date << std::endl;
    std::cout << "Status          : " << (present ? "Present" : "Absent") << std::endl;
}

// ============================================================================
// FeePayment Implementation
// ============================================================================

FeePayment::FeePayment(const std::string& receiptNo, const std::string& studentRollNo,
                       double amount, const std::string& paymentDate,
                       const std::string& paymentMode)
    : receiptNo(receiptNo), studentRollNo(studentRollNo), amount(amount),
      paymentDate(paymentDate), paymentMode(paymentMode) {
}

const std::string& FeePayment::getReceiptNo() const { return receiptNo; }
const std::string& FeePayment::getStudentRollNo() const { return studentRollNo; }
double FeePayment::getAmount() const { return amount; }
const std::string& FeePayment::getPaymentDate() const { return paymentDate; }
const std::string& FeePayment::getPaymentMode() const { return paymentMode; }

void FeePayment::displayDetails() const {
    std::cout << "--- Fee Payment Details ---" << std::endl;
    std::cout << "Receipt No      : " << receiptNo << std::endl;
    std::cout << "Student Roll No : " << studentRollNo << std::endl;
    std::cout << "Amount          : " << amount << std::endl;
    std::cout << "Payment Date    : " << paymentDate << std::endl;
    std::cout << "Payment Mode    : " << paymentMode << std::endl;
}

} // namespace college
