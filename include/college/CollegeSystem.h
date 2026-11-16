#ifndef COLLEGE_COLLEGE_SYSTEM_H
#define COLLEGE_COLLEGE_SYSTEM_H

// College Management System - CollegeSystem
// Central coordinator that owns all managers and the data manager.

#include "college/Managers.h"
#include "college/DataManager.h"

namespace college {

class CollegeSystem {
private:
    StudentManager studentManager;
    FacultyManager facultyManager;
    CourseManager courseManager;
    EnrollmentManager enrollmentManager;
    AttendanceManager attendanceManager;
    FeeManager feeManager;
    ReportManager reportManager;
    DataManager dataManager;

public:
    CollegeSystem();

    // Data operations
    bool loadAllData();
    bool saveAllData() const;

    // Non-const accessors
    StudentManager& getStudentManager();
    FacultyManager& getFacultyManager();
    CourseManager& getCourseManager();
    EnrollmentManager& getEnrollmentManager();
    AttendanceManager& getAttendanceManager();
    FeeManager& getFeeManager();
    ReportManager& getReportManager();

    // Const accessors
    const StudentManager& getStudentManager() const;
    const FacultyManager& getFacultyManager() const;
    const CourseManager& getCourseManager() const;
    const EnrollmentManager& getEnrollmentManager() const;
    const AttendanceManager& getAttendanceManager() const;
    const FeeManager& getFeeManager() const;
    const ReportManager& getReportManager() const;
};

} // namespace college

#endif // COLLEGE_COLLEGE_SYSTEM_H
