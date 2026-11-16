#ifndef COLLEGE_DATA_MANAGER_H
#define COLLEGE_DATA_MANAGER_H

// College Management System - Data Manager
// Handles persistent file I/O for all manager collections.
// Uses pipe-separated text files with field escaping.

#include "college/Managers.h"
#include <string>

namespace college {

class DataManager {
private:
    std::string dataDirectory;

    // Field encoding helpers
    static std::string escapeField(const std::string& field);
    static std::string unescapeField(const std::string& field);
    static std::vector<std::string> splitLine(const std::string& line);

public:
    explicit DataManager(const std::string& directory = "data");

    // Save methods
    bool saveStudents(const StudentManager& manager) const;
    bool saveFaculty(const FacultyManager& manager) const;
    bool saveCourses(const CourseManager& manager) const;
    bool saveEnrollments(const EnrollmentManager& manager) const;
    bool saveAttendance(const AttendanceManager& manager) const;
    bool savePayments(const FeeManager& manager) const;

    // Load methods
    bool loadStudents(StudentManager& manager) const;
    bool loadFaculty(FacultyManager& manager) const;
    bool loadCourses(CourseManager& manager) const;
    bool loadEnrollments(EnrollmentManager& manager) const;
    bool loadAttendance(AttendanceManager& manager) const;
    bool loadPayments(FeeManager& manager) const;

    // Combined operations
    bool saveAllData(
        const StudentManager& studentManager,
        const FacultyManager& facultyManager,
        const CourseManager& courseManager,
        const EnrollmentManager& enrollmentManager,
        const AttendanceManager& attendanceManager,
        const FeeManager& feeManager) const;

    bool loadAllData(
        StudentManager& studentManager,
        FacultyManager& facultyManager,
        CourseManager& courseManager,
        EnrollmentManager& enrollmentManager,
        AttendanceManager& attendanceManager,
        FeeManager& feeManager) const;
};

} // namespace college

#endif // COLLEGE_DATA_MANAGER_H
