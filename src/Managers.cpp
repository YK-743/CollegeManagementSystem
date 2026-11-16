// College Management System - Manager Classes Implementation

#include "college/Managers.h"
#include <sstream>
#include <iomanip>

namespace college {

// ============================================================================
// StudentManager Implementation
// ============================================================================

bool StudentManager::addStudent(const Student& student) {
    // Reject if roll number already exists (including inactive records)
    if (findStudent(student.getRollNo()) != nullptr) {
        return false;
    }
    students.push_back(student);
    return true;
}

bool StudentManager::updateStudent(const std::string& rollNo, const Student& updatedData) {
    Student* student = findStudent(rollNo);

    // Student must exist and be active
    if (student == nullptr || !student->isActive()) {
        return false;
    }

    // Use the existing Student::updateDetails() method
    // Signature: updateDetails(name, email, phone, department, semester)
    student->updateDetails(
        updatedData.getName(),
        updatedData.getEmail(),
        updatedData.getPhone(),
        updatedData.getDepartment(),
        updatedData.getSemester()
    );

    return true;
}

bool StudentManager::removeStudent(const std::string& rollNo) {
    Student* student = findStudent(rollNo);

    // Must exist and be currently active
    if (student == nullptr || !student->isActive()) {
        return false;
    }

    student->setActive(false);
    return true;
}

Student* StudentManager::findStudent(const std::string& rollNo) {
    for (auto& student : students) {
        if (student.getRollNo() == rollNo) {
            return &student;
        }
    }
    return nullptr;
}

const Student* StudentManager::findStudent(const std::string& rollNo) const {
    for (const auto& student : students) {
        if (student.getRollNo() == rollNo) {
            return &student;
        }
    }
    return nullptr;
}

const std::vector<Student>& StudentManager::getAllStudents() const {
    return students;
}

int StudentManager::getTotalStudents() const {
    int count = 0;
    for (const auto& student : students) {
        if (student.isActive()) {
            ++count;
        }
    }
    return count;
}

// ============================================================================
// FacultyManager Implementation
// ============================================================================

bool FacultyManager::addFaculty(const Faculty& faculty) {
    // Reject if faculty ID already exists (including inactive records)
    if (findFaculty(faculty.getFacultyId()) != nullptr) {
        return false;
    }
    facultyMembers.push_back(faculty);
    return true;
}

bool FacultyManager::updateFaculty(const std::string& facultyId, const Faculty& updatedData) {
    Faculty* faculty = findFaculty(facultyId);

    // Faculty must exist and be active
    if (faculty == nullptr || !faculty->isActive()) {
        return false;
    }

    // Use the existing Faculty::updateDetails() method
    // Signature: updateDetails(name, email, phone, department, designation)
    faculty->updateDetails(
        updatedData.getName(),
        updatedData.getEmail(),
        updatedData.getPhone(),
        updatedData.getDepartment(),
        updatedData.getDesignation()
    );

    return true;
}

bool FacultyManager::removeFaculty(const std::string& facultyId) {
    Faculty* faculty = findFaculty(facultyId);

    // Must exist and be currently active
    if (faculty == nullptr || !faculty->isActive()) {
        return false;
    }

    faculty->setActive(false);
    return true;
}

Faculty* FacultyManager::findFaculty(const std::string& facultyId) {
    for (auto& faculty : facultyMembers) {
        if (faculty.getFacultyId() == facultyId) {
            return &faculty;
        }
    }
    return nullptr;
}

const Faculty* FacultyManager::findFaculty(const std::string& facultyId) const {
    for (const auto& faculty : facultyMembers) {
        if (faculty.getFacultyId() == facultyId) {
            return &faculty;
        }
    }
    return nullptr;
}

const std::vector<Faculty>& FacultyManager::getAllFaculty() const {
    return facultyMembers;
}

int FacultyManager::getTotalFaculty() const {
    int count = 0;
    for (const auto& faculty : facultyMembers) {
        if (faculty.isActive()) {
            ++count;
        }
    }
    return count;
}

// ============================================================================
// CourseManager Implementation
// ============================================================================

bool CourseManager::addCourse(const Course& course) {
    // Reject if course code already exists (including inactive records)
    if (findCourse(course.getCourseCode()) != nullptr) {
        return false;
    }
    courses.push_back(course);
    return true;
}

bool CourseManager::updateCourse(const std::string& courseCode, const Course& updatedData) {
    Course* course = findCourse(courseCode);

    // Course must exist and be active
    if (course == nullptr || !course->isActive()) {
        return false;
    }

    // Use the existing Course::updateDetails() method
    // Signature: updateDetails(courseName, credits, capacity)
    course->updateDetails(
        updatedData.getCourseName(),
        updatedData.getCredits(),
        updatedData.getCapacity()
    );

    return true;
}

bool CourseManager::removeCourse(const std::string& courseCode) {
    Course* course = findCourse(courseCode);

    // Must exist and be currently active
    if (course == nullptr || !course->isActive()) {
        return false;
    }

    course->setActive(false);
    return true;
}

Course* CourseManager::findCourse(const std::string& courseCode) {
    for (auto& course : courses) {
        if (course.getCourseCode() == courseCode) {
            return &course;
        }
    }
    return nullptr;
}

const Course* CourseManager::findCourse(const std::string& courseCode) const {
    for (const auto& course : courses) {
        if (course.getCourseCode() == courseCode) {
            return &course;
        }
    }
    return nullptr;
}

const std::vector<Course>& CourseManager::getAllCourses() const {
    return courses;
}

int CourseManager::getTotalCourses() const {
    int count = 0;
    for (const auto& course : courses) {
        if (course.isActive()) {
            ++count;
        }
    }
    return count;
}

bool CourseManager::assignFaculty(const std::string& courseCode,
                                  const std::string& facultyId,
                                  const FacultyManager& facultyManager) {
    // Find the course — must exist and be active
    Course* course = findCourse(courseCode);
    if (course == nullptr || !course->isActive()) {
        return false;
    }

    // Find the faculty — must exist and be active
    const Faculty* faculty = facultyManager.findFaculty(facultyId);
    if (faculty == nullptr || !faculty->isActive()) {
        return false;
    }

    // Both validated — assign
    course->assignFaculty(facultyId);
    return true;
}

// ============================================================================
// EnrollmentManager Implementation
// ============================================================================

bool EnrollmentManager::enrollStudent(const std::string& rollNo,
                                      const std::string& courseCode,
                                      const std::string& enrollmentDate,
                                      const StudentManager& studentManager,
                                      const CourseManager& courseManager) {
    // 1-2. Verify student exists and is active
    const Student* student = studentManager.findStudent(rollNo);
    if (student == nullptr || !student->isActive()) {
        return false;
    }

    // 3-4. Verify course exists and is active
    const Course* course = courseManager.findCourse(courseCode);
    if (course == nullptr || !course->isActive()) {
        return false;
    }

    // 5. Reject duplicate active enrollment
    if (isAlreadyEnrolled(rollNo, courseCode)) {
        return false;
    }

    // 6-7. Check available course capacity (active enrollments only)
    int currentCount = getEnrolledCount(courseCode);
    if (currentCount >= course->getCapacity()) {
        return false;
    }

    // 8-9. Create and add the enrollment
    Enrollment enrollment(rollNo, courseCode, enrollmentDate);
    enrollments.push_back(enrollment);

    return true;
}

bool EnrollmentManager::dropCourse(const std::string& rollNo,
                                   const std::string& courseCode) {
    // Find an active enrollment matching student and course
    for (auto& enrollment : enrollments) {
        if (enrollment.getStudentRollNo() == rollNo &&
            enrollment.getCourseCode() == courseCode &&
            enrollment.isActive()) {
            enrollment.drop();
            return true;
        }
    }
    return false;
}

bool EnrollmentManager::isAlreadyEnrolled(const std::string& rollNo,
                                          const std::string& courseCode) const {
    for (const auto& enrollment : enrollments) {
        if (enrollment.getStudentRollNo() == rollNo &&
            enrollment.getCourseCode() == courseCode &&
            enrollment.isActive()) {
            return true;
        }
    }
    return false;
}

int EnrollmentManager::getEnrolledCount(const std::string& courseCode) const {
    int count = 0;
    for (const auto& enrollment : enrollments) {
        if (enrollment.getCourseCode() == courseCode && enrollment.isActive()) {
            ++count;
        }
    }
    return count;
}

std::vector<Enrollment> EnrollmentManager::getStudentEnrollments(
        const std::string& rollNo) const {
    std::vector<Enrollment> result;
    for (const auto& enrollment : enrollments) {
        if (enrollment.getStudentRollNo() == rollNo && enrollment.isActive()) {
            result.push_back(enrollment);
        }
    }
    return result;
}

const std::vector<Enrollment>& EnrollmentManager::getAllEnrollments() const {
    return enrollments;
}

// ============================================================================
// AttendanceManager Implementation
// ============================================================================

bool AttendanceManager::markAttendance(const std::string& rollNo,
                                       const std::string& courseCode,
                                       const std::string& date,
                                       bool present,
                                       const StudentManager& studentManager,
                                       const CourseManager& courseManager,
                                       const EnrollmentManager& enrollmentManager) {
    // 1. Verify student exists and is active
    const Student* student = studentManager.findStudent(rollNo);
    if (student == nullptr || !student->isActive()) {
        return false;
    }

    // 2. Verify course exists and is active
    const Course* course = courseManager.findCourse(courseCode);
    if (course == nullptr || !course->isActive()) {
        return false;
    }

    // 3. Verify the student is actively enrolled in the course
    if (!enrollmentManager.isAlreadyEnrolled(rollNo, courseCode)) {
        return false;
    }

    // 4. Reject duplicate attendance for the same student, course, and date
    for (const auto& record : attendanceRecords) {
        if (record.getStudentRollNo() == rollNo &&
            record.getCourseCode() == courseCode &&
            record.getDate() == date) {
            return false;
        }
    }

    // 5-6. Create and add the attendance record
    AttendanceRecord record(rollNo, courseCode, date, present);
    attendanceRecords.push_back(record);

    return true;
}

bool AttendanceManager::updateAttendance(const std::string& rollNo,
                                         const std::string& courseCode,
                                         const std::string& date,
                                         bool present) {
    // Find an existing record matching student, course, and date
    for (auto& record : attendanceRecords) {
        if (record.getStudentRollNo() == rollNo &&
            record.getCourseCode() == courseCode &&
            record.getDate() == date) {
            record.setPresent(present);
            return true;
        }
    }
    return false;
}

double AttendanceManager::calculatePercentage(const std::string& rollNo,
                                              const std::string& courseCode) const {
    int total = 0;
    int presentCount = 0;

    for (const auto& record : attendanceRecords) {
        if (record.getStudentRollNo() == rollNo &&
            record.getCourseCode() == courseCode) {
            ++total;
            if (record.isPresent()) {
                ++presentCount;
            }
        }
    }

    if (total == 0) {
        return 0.0;
    }

    return (static_cast<double>(presentCount) / static_cast<double>(total)) * 100.0;
}

std::vector<AttendanceRecord> AttendanceManager::getStudentAttendance(
        const std::string& rollNo) const {
    std::vector<AttendanceRecord> result;
    for (const auto& record : attendanceRecords) {
        if (record.getStudentRollNo() == rollNo) {
            result.push_back(record);
        }
    }
    return result;
}

std::vector<AttendanceRecord> AttendanceManager::getCourseAttendance(
        const std::string& courseCode) const {
    std::vector<AttendanceRecord> result;
    for (const auto& record : attendanceRecords) {
        if (record.getCourseCode() == courseCode) {
            result.push_back(record);
        }
    }
    return result;
}

const std::vector<AttendanceRecord>& AttendanceManager::getAllAttendanceRecords() const {
    return attendanceRecords;
}

int AttendanceManager::getTotalClasses(const std::string& rollNo,
                                       const std::string& courseCode) const {
    int count = 0;
    for (const auto& record : attendanceRecords) {
        if (record.getStudentRollNo() == rollNo &&
            record.getCourseCode() == courseCode) {
            ++count;
        }
    }
    return count;
}

// ============================================================================
// FeeManager Implementation
// ============================================================================

bool FeeManager::recordPayment(const FeePayment& payment,
                                const StudentManager& studentManager) {
    // 1-2. Verify student exists and is active
    const Student* student = studentManager.findStudent(payment.getStudentRollNo());
    if (student == nullptr || !student->isActive()) {
        return false;
    }

    // 3. Reject zero or negative amounts
    if (payment.getAmount() <= 0.0) {
        return false;
    }

    // 4. Reject duplicate receipt numbers
    if (findPaymentByReceipt(payment.getReceiptNo()) != nullptr) {
        return false;
    }

    // 5. Prevent payments exceeding the outstanding fee
    double outstanding = student->getTotalFee() - calculateTotalPaid(payment.getStudentRollNo());
    if (payment.getAmount() > outstanding) {
        return false;
    }

    // 6. All validations passed — add the payment
    payments.push_back(payment);
    return true;
}

double FeeManager::calculateTotalPaid(const std::string& rollNo) const {
    double total = 0.0;
    for (const auto& payment : payments) {
        if (payment.getStudentRollNo() == rollNo) {
            total += payment.getAmount();
        }
    }
    return total;
}

double FeeManager::calculateOutstanding(const std::string& rollNo,
                                         const StudentManager& studentManager) const {
    const Student* student = studentManager.findStudent(rollNo);
    if (student == nullptr) {
        return -1.0;
    }
    return student->getTotalFee() - calculateTotalPaid(rollNo);
}

std::vector<FeePayment> FeeManager::getPaymentHistory(const std::string& rollNo) const {
    std::vector<FeePayment> result;
    for (const auto& payment : payments) {
        if (payment.getStudentRollNo() == rollNo) {
            result.push_back(payment);
        }
    }
    return result;
}

const std::vector<FeePayment>& FeeManager::getAllPayments() const {
    return payments;
}

const FeePayment* FeeManager::findPaymentByReceipt(const std::string& receiptNo) const {
    for (const auto& payment : payments) {
        if (payment.getReceiptNo() == receiptNo) {
            return &payment;
        }
    }
    return nullptr;
}

double FeeManager::getTotalCollection() const {
    double total = 0.0;
    for (const auto& payment : payments) {
        total += payment.getAmount();
    }
    return total;
}

std::string FeeManager::generateReceipt(const std::string& receiptNo) const {
    const FeePayment* payment = findPaymentByReceipt(receiptNo);
    if (payment == nullptr) {
        return "";
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "========================================" << "\n";
    oss << "     COLLEGE MANAGEMENT SYSTEM" << "\n";
    oss << "          PAYMENT RECEIPT" << "\n";
    oss << "========================================" << "\n";
    oss << "Receipt No      : " << payment->getReceiptNo() << "\n";
    oss << "Student Roll No : " << payment->getStudentRollNo() << "\n";
    oss << "Amount Paid     : " << payment->getAmount() << "\n";
    oss << "Payment Date    : " << payment->getPaymentDate() << "\n";
    oss << "Payment Mode    : " << payment->getPaymentMode() << "\n";
    oss << "========================================" << "\n";

    return oss.str();
}

// ============================================================================
// ReportManager Implementation
// ============================================================================

DashboardSummary ReportManager::getDashboardSummary(
        const StudentManager& studentManager,
        const FacultyManager& facultyManager,
        const CourseManager& courseManager,
        const EnrollmentManager& enrollmentManager,
        const FeeManager& feeManager) const {

    DashboardSummary summary;
    summary.totalStudents = studentManager.getTotalStudents();
    summary.totalFaculty = facultyManager.getTotalFaculty();
    summary.totalCourses = courseManager.getTotalCourses();

    // Count active enrollments
    int activeEnrollments = 0;
    for (const auto& enrollment : enrollmentManager.getAllEnrollments()) {
        if (enrollment.isActive()) {
            ++activeEnrollments;
        }
    }
    summary.totalEnrollments = activeEnrollments;

    summary.totalFeeCollection = feeManager.getTotalCollection();

    // Calculate total outstanding fees for active students
    double totalOutstanding = 0.0;
    for (const auto& student : studentManager.getAllStudents()) {
        if (student.isActive()) {
            double outstanding = student.getTotalFee()
                                 - feeManager.calculateTotalPaid(student.getRollNo());
            totalOutstanding += outstanding;
        }
    }
    summary.totalOutstandingFees = totalOutstanding;

    return summary;
}

std::vector<FeeReportRow> ReportManager::getFeeReport(
        const StudentManager& studentManager,
        const FeeManager& feeManager) const {

    std::vector<FeeReportRow> report;

    for (const auto& student : studentManager.getAllStudents()) {
        if (!student.isActive()) {
            continue;
        }

        FeeReportRow row;
        row.rollNo = student.getRollNo();
        row.studentName = student.getName();
        row.totalFee = student.getTotalFee();
        row.totalPaid = feeManager.calculateTotalPaid(student.getRollNo());
        row.outstanding = row.totalFee - row.totalPaid;

        report.push_back(row);
    }

    return report;
}

std::vector<AttendanceReportRow> ReportManager::getAttendanceReport(
        const StudentManager& studentManager,
        const EnrollmentManager& enrollmentManager,
        const AttendanceManager& attendanceManager,
        double requiredPercentage) const {

    std::vector<AttendanceReportRow> report;

    for (const auto& student : studentManager.getAllStudents()) {
        if (!student.isActive()) {
            continue;
        }

        // Get active enrollments for this student
        auto enrollments = enrollmentManager.getStudentEnrollments(student.getRollNo());

        for (const auto& enrollment : enrollments) {
            int totalClasses = attendanceManager.getTotalClasses(
                student.getRollNo(), enrollment.getCourseCode());

            // Skip student-course combinations with zero attendance records
            if (totalClasses == 0) {
                continue;
            }

            double percentage = attendanceManager.calculatePercentage(
                student.getRollNo(), enrollment.getCourseCode());

            AttendanceReportRow row;
            row.rollNo = student.getRollNo();
            row.studentName = student.getName();
            row.courseCode = enrollment.getCourseCode();
            row.attendancePercentage = percentage;
            row.totalClasses = totalClasses;
            row.belowRequirement = (percentage < requiredPercentage);

            report.push_back(row);
        }
    }

    return report;
}

std::vector<CourseEnrollmentReportRow> ReportManager::getEnrollmentReport(
        const CourseManager& courseManager,
        const EnrollmentManager& enrollmentManager) const {

    std::vector<CourseEnrollmentReportRow> report;

    for (const auto& course : courseManager.getAllCourses()) {
        if (!course.isActive()) {
            continue;
        }

        CourseEnrollmentReportRow row;
        row.courseCode = course.getCourseCode();
        row.courseName = course.getCourseName();
        row.facultyId = course.getFacultyId();
        row.enrolledStudents = enrollmentManager.getEnrolledCount(course.getCourseCode());
        row.capacity = course.getCapacity();

        report.push_back(row);
    }

    return report;
}

} // namespace college
