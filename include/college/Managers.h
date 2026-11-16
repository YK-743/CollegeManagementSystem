#ifndef COLLEGE_MANAGERS_H
#define COLLEGE_MANAGERS_H

// College Management System - Manager Classes
// Contains declarations for all manager classes and report data structures.
// Managers handle business logic. They do not perform any user I/O.

#include "college/Models.h"
#include <vector>
#include <string>

namespace college {

// Forward declaration for friend access
class DataManager;

// ============================================================================
// CLASS 1: StudentManager
// ============================================================================
class StudentManager {
    friend class DataManager;
private:
    std::vector<Student> students;

public:
    bool addStudent(const Student& student);
    bool updateStudent(const std::string& rollNo, const Student& updatedData);
    bool removeStudent(const std::string& rollNo);

    Student* findStudent(const std::string& rollNo);
    const Student* findStudent(const std::string& rollNo) const;

    const std::vector<Student>& getAllStudents() const;
    int getTotalStudents() const;
};

// ============================================================================
// CLASS 2: FacultyManager
// ============================================================================
class FacultyManager {
    friend class DataManager;
private:
    std::vector<Faculty> facultyMembers;

public:
    bool addFaculty(const Faculty& faculty);
    bool updateFaculty(const std::string& facultyId, const Faculty& updatedData);
    bool removeFaculty(const std::string& facultyId);

    Faculty* findFaculty(const std::string& facultyId);
    const Faculty* findFaculty(const std::string& facultyId) const;

    const std::vector<Faculty>& getAllFaculty() const;
    int getTotalFaculty() const;
};

// ============================================================================
// CLASS 3: CourseManager
// ============================================================================
class CourseManager {
    friend class DataManager;
private:
    std::vector<Course> courses;

public:
    bool addCourse(const Course& course);
    bool updateCourse(const std::string& courseCode, const Course& updatedData);
    bool removeCourse(const std::string& courseCode);

    Course* findCourse(const std::string& courseCode);
    const Course* findCourse(const std::string& courseCode) const;

    const std::vector<Course>& getAllCourses() const;
    int getTotalCourses() const;

    bool assignFaculty(const std::string& courseCode,
                       const std::string& facultyId,
                       const FacultyManager& facultyManager);
};

// ============================================================================
// CLASS 4: EnrollmentManager
// ============================================================================
class EnrollmentManager {
    friend class DataManager;
private:
    std::vector<Enrollment> enrollments;

public:
    bool enrollStudent(const std::string& rollNo,
                       const std::string& courseCode,
                       const std::string& enrollmentDate,
                       const StudentManager& studentManager,
                       const CourseManager& courseManager);

    bool dropCourse(const std::string& rollNo,
                    const std::string& courseCode);

    bool isAlreadyEnrolled(const std::string& rollNo,
                           const std::string& courseCode) const;

    int getEnrolledCount(const std::string& courseCode) const;

    std::vector<Enrollment> getStudentEnrollments(const std::string& rollNo) const;

    const std::vector<Enrollment>& getAllEnrollments() const;
};

// ============================================================================
// CLASS 5: AttendanceManager
// ============================================================================
class AttendanceManager {
    friend class DataManager;
private:
    std::vector<AttendanceRecord> attendanceRecords;

public:
    bool markAttendance(const std::string& rollNo,
                        const std::string& courseCode,
                        const std::string& date,
                        bool present,
                        const StudentManager& studentManager,
                        const CourseManager& courseManager,
                        const EnrollmentManager& enrollmentManager);

    bool updateAttendance(const std::string& rollNo,
                          const std::string& courseCode,
                          const std::string& date,
                          bool present);

    double calculatePercentage(const std::string& rollNo,
                               const std::string& courseCode) const;

    std::vector<AttendanceRecord> getStudentAttendance(const std::string& rollNo) const;

    std::vector<AttendanceRecord> getCourseAttendance(const std::string& courseCode) const;

    const std::vector<AttendanceRecord>& getAllAttendanceRecords() const;

    int getTotalClasses(const std::string& rollNo,
                        const std::string& courseCode) const;
};

// ============================================================================
// CLASS 6: FeeManager
// ============================================================================
class FeeManager {
    friend class DataManager;
private:
    std::vector<FeePayment> payments;

public:
    bool recordPayment(const FeePayment& payment,
                       const StudentManager& studentManager);

    double calculateTotalPaid(const std::string& rollNo) const;

    double calculateOutstanding(const std::string& rollNo,
                                const StudentManager& studentManager) const;

    std::vector<FeePayment> getPaymentHistory(const std::string& rollNo) const;

    const std::vector<FeePayment>& getAllPayments() const;

    const FeePayment* findPaymentByReceipt(const std::string& receiptNo) const;

    double getTotalCollection() const;

    std::string generateReceipt(const std::string& receiptNo) const;
};

// ============================================================================
// Report Data Structures
// ============================================================================

struct DashboardSummary {
    int totalStudents;
    int totalFaculty;
    int totalCourses;
    int totalEnrollments;
    double totalFeeCollection;
    double totalOutstandingFees;
};

struct FeeReportRow {
    std::string rollNo;
    std::string studentName;
    double totalFee;
    double totalPaid;
    double outstanding;
};

struct AttendanceReportRow {
    std::string rollNo;
    std::string studentName;
    std::string courseCode;
    double attendancePercentage;
    int totalClasses;
    bool belowRequirement;
};

struct CourseEnrollmentReportRow {
    std::string courseCode;
    std::string courseName;
    std::string facultyId;
    int enrolledStudents;
    int capacity;
};

// ============================================================================
// CLASS 7: ReportManager
// ============================================================================
class ReportManager {
public:
    DashboardSummary getDashboardSummary(
        const StudentManager& studentManager,
        const FacultyManager& facultyManager,
        const CourseManager& courseManager,
        const EnrollmentManager& enrollmentManager,
        const FeeManager& feeManager) const;

    std::vector<FeeReportRow> getFeeReport(
        const StudentManager& studentManager,
        const FeeManager& feeManager) const;

    std::vector<AttendanceReportRow> getAttendanceReport(
        const StudentManager& studentManager,
        const EnrollmentManager& enrollmentManager,
        const AttendanceManager& attendanceManager,
        double requiredPercentage) const;

    std::vector<CourseEnrollmentReportRow> getEnrollmentReport(
        const CourseManager& courseManager,
        const EnrollmentManager& enrollmentManager) const;
};

} // namespace college

#endif // COLLEGE_MANAGERS_H
