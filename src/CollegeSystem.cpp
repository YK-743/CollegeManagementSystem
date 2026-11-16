// College Management System - CollegeSystem Implementation

#include "college/CollegeSystem.h"

namespace college {

CollegeSystem::CollegeSystem()
    : dataManager("data") {
    // All managers are default-constructed (empty collections).
}

// --- Data operations ---

bool CollegeSystem::loadAllData() {
    return dataManager.loadAllData(
        studentManager, facultyManager, courseManager,
        enrollmentManager, attendanceManager, feeManager);
}

bool CollegeSystem::saveAllData() const {
    return dataManager.saveAllData(
        studentManager, facultyManager, courseManager,
        enrollmentManager, attendanceManager, feeManager);
}

// --- Non-const accessors ---

StudentManager& CollegeSystem::getStudentManager() { return studentManager; }
FacultyManager& CollegeSystem::getFacultyManager() { return facultyManager; }
CourseManager& CollegeSystem::getCourseManager() { return courseManager; }
EnrollmentManager& CollegeSystem::getEnrollmentManager() { return enrollmentManager; }
AttendanceManager& CollegeSystem::getAttendanceManager() { return attendanceManager; }
FeeManager& CollegeSystem::getFeeManager() { return feeManager; }
ReportManager& CollegeSystem::getReportManager() { return reportManager; }

// --- Const accessors ---

const StudentManager& CollegeSystem::getStudentManager() const { return studentManager; }
const FacultyManager& CollegeSystem::getFacultyManager() const { return facultyManager; }
const CourseManager& CollegeSystem::getCourseManager() const { return courseManager; }
const EnrollmentManager& CollegeSystem::getEnrollmentManager() const { return enrollmentManager; }
const AttendanceManager& CollegeSystem::getAttendanceManager() const { return attendanceManager; }
const FeeManager& CollegeSystem::getFeeManager() const { return feeManager; }
const ReportManager& CollegeSystem::getReportManager() const { return reportManager; }

} // namespace college
