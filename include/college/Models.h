#ifndef COLLEGE_MODELS_H
#define COLLEGE_MODELS_H

// College Management System - Model Classes
// Contains declarations for all domain/model classes.

#include <string>

namespace college {

// ============================================================================
// CLASS 1: Person (Abstract Base Class)
// ============================================================================
class Person {
private:
    std::string id;
    std::string name;
    std::string email;
    std::string phone;

public:
    Person(const std::string& id, const std::string& name,
           const std::string& email, const std::string& phone);
    virtual ~Person();

    // Getters
    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getEmail() const;
    const std::string& getPhone() const;

    // Setters
    void setName(const std::string& name);
    void setEmail(const std::string& email);
    void setPhone(const std::string& phone);

    // Pure virtual - makes Person abstract
    virtual void displayDetails() const = 0;
};

// ============================================================================
// CLASS 2: Student (inherits from Person)
// ============================================================================
class Student : public Person {
private:
    std::string department;
    int semester;
    double totalFee;
    bool active;

public:
    Student(const std::string& rollNo, const std::string& name,
            const std::string& email, const std::string& phone,
            const std::string& department, int semester, double totalFee);

    // Getters
    const std::string& getRollNo() const;
    const std::string& getDepartment() const;
    int getSemester() const;
    double getTotalFee() const;
    bool isActive() const;

    // Setters / Mutators
    void updateDetails(const std::string& name, const std::string& email,
                       const std::string& phone, const std::string& department,
                       int semester);
    void setActive(bool status);

    // Override
    void displayDetails() const override;
};

// ============================================================================
// CLASS 3: Faculty (inherits from Person)
// ============================================================================
class Faculty : public Person {
private:
    std::string department;
    std::string designation;
    bool active;

public:
    Faculty(const std::string& facultyId, const std::string& name,
            const std::string& email, const std::string& phone,
            const std::string& department, const std::string& designation);

    // Getters
    const std::string& getFacultyId() const;
    const std::string& getDepartment() const;
    const std::string& getDesignation() const;
    bool isActive() const;

    // Setters / Mutators
    void updateDetails(const std::string& name, const std::string& email,
                       const std::string& phone, const std::string& department,
                       const std::string& designation);
    void setActive(bool status);

    // Override
    void displayDetails() const override;
};

// ============================================================================
// CLASS 4: Course
// ============================================================================
class Course {
private:
    std::string courseCode;
    std::string courseName;
    int credits;
    int capacity;
    std::string facultyId;
    bool active;

public:
    Course(const std::string& courseCode, const std::string& courseName,
           int credits, int capacity, const std::string& facultyId);

    // Getters
    const std::string& getCourseCode() const;
    const std::string& getCourseName() const;
    int getCredits() const;
    int getCapacity() const;
    const std::string& getFacultyId() const;
    bool isActive() const;

    // Setters / Mutators
    void assignFaculty(const std::string& facultyId);
    void updateDetails(const std::string& courseName, int credits, int capacity);
    void setActive(bool status);

    void displayDetails() const;
};

// ============================================================================
// CLASS 5: Enrollment
// ============================================================================
class Enrollment {
private:
    std::string studentRollNo;
    std::string courseCode;
    std::string enrollmentDate;
    bool active;

public:
    Enrollment(const std::string& studentRollNo, const std::string& courseCode,
               const std::string& enrollmentDate);

    // Getters
    const std::string& getStudentRollNo() const;
    const std::string& getCourseCode() const;
    const std::string& getEnrollmentDate() const;
    bool isActive() const;

    // Mutator
    void drop();

    void displayDetails() const;
};

// ============================================================================
// CLASS 6: AttendanceRecord
// ============================================================================
class AttendanceRecord {
private:
    std::string studentRollNo;
    std::string courseCode;
    std::string date;
    bool present;

public:
    AttendanceRecord(const std::string& studentRollNo, const std::string& courseCode,
                     const std::string& date, bool present);

    // Getters
    const std::string& getStudentRollNo() const;
    const std::string& getCourseCode() const;
    const std::string& getDate() const;
    bool isPresent() const;

    // Setter
    void setPresent(bool status);

    void displayDetails() const;
};

// ============================================================================
// CLASS 7: FeePayment
// ============================================================================
class FeePayment {
private:
    std::string receiptNo;
    std::string studentRollNo;
    double amount;
    std::string paymentDate;
    std::string paymentMode;

public:
    FeePayment(const std::string& receiptNo, const std::string& studentRollNo,
               double amount, const std::string& paymentDate,
               const std::string& paymentMode);

    // Getters
    const std::string& getReceiptNo() const;
    const std::string& getStudentRollNo() const;
    double getAmount() const;
    const std::string& getPaymentDate() const;
    const std::string& getPaymentMode() const;

    void displayDetails() const;
};

} // namespace college

#endif // COLLEGE_MODELS_H
