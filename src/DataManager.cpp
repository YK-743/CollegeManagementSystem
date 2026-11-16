// College Management System - Data Manager Implementation
// Handles persistent file I/O using pipe-separated text files.

#include "college/DataManager.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace college {

// ============================================================================
// Constructor
// ============================================================================

DataManager::DataManager(const std::string& directory)
    : dataDirectory(directory) {
}

// ============================================================================
// Field Encoding Helpers
// ============================================================================

std::string DataManager::escapeField(const std::string& field) {
    std::string result;
    result.reserve(field.size());
    for (char ch : field) {
        switch (ch) {
            case '\\': result += "\\\\"; break;
            case '|':  result += "\\|";  break;
            case '\n': result += "\\n";  break;
            case '\r': result += "\\r";  break;
            default:   result += ch;     break;
        }
    }
    return result;
}

std::string DataManager::unescapeField(const std::string& field) {
    std::string result;
    result.reserve(field.size());
    for (std::size_t i = 0; i < field.size(); ++i) {
        if (field[i] == '\\' && i + 1 < field.size()) {
            char next = field[i + 1];
            switch (next) {
                case '\\': result += '\\'; ++i; break;
                case '|':  result += '|';  ++i; break;
                case 'n':  result += '\n'; ++i; break;
                case 'r':  result += '\r'; ++i; break;
                default:   result += field[i]; break;
            }
        } else {
            result += field[i];
        }
    }
    return result;
}

std::vector<std::string> DataManager::splitLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;

    for (std::size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '\\' && i + 1 < line.size()) {
            // Escaped character — keep as-is for unescapeField to handle
            current += line[i];
            current += line[i + 1];
            ++i;
        } else if (line[i] == '|') {
            fields.push_back(unescapeField(current));
            current.clear();
        } else {
            current += line[i];
        }
    }
    fields.push_back(unescapeField(current));
    return fields;
}

// ============================================================================
// Helper: check if file exists
// ============================================================================

static bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

// Helper: parse int safely, returns false on failure
static bool parseInteger(const std::string& str, int& out) {
    try {
        std::size_t pos = 0;
        out = std::stoi(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

// Helper: parse double safely, returns false on failure
static bool parseDouble(const std::string& str, double& out) {
    try {
        std::size_t pos = 0;
        out = std::stod(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

// Helper: parse bool (0 or 1) safely
static bool parseBool(const std::string& str, bool& out) {
    if (str == "1") { out = true; return true; }
    if (str == "0") { out = false; return true; }
    return false;
}

// ============================================================================
// Save Methods
// ============================================================================

bool DataManager::saveStudents(const StudentManager& manager) const {
    std::string path = dataDirectory + "/students.txt";
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    file << std::setprecision(17);
    for (const auto& s : manager.students) {
        file << escapeField(s.getRollNo()) << '|'
             << escapeField(s.getName()) << '|'
             << escapeField(s.getEmail()) << '|'
             << escapeField(s.getPhone()) << '|'
             << escapeField(s.getDepartment()) << '|'
             << s.getSemester() << '|'
             << s.getTotalFee() << '|'
             << (s.isActive() ? "1" : "0") << '\n';
    }

    file.close();
    return !file.fail();
}

bool DataManager::saveFaculty(const FacultyManager& manager) const {
    std::string path = dataDirectory + "/faculty.txt";
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& f : manager.facultyMembers) {
        file << escapeField(f.getFacultyId()) << '|'
             << escapeField(f.getName()) << '|'
             << escapeField(f.getEmail()) << '|'
             << escapeField(f.getPhone()) << '|'
             << escapeField(f.getDepartment()) << '|'
             << escapeField(f.getDesignation()) << '|'
             << (f.isActive() ? "1" : "0") << '\n';
    }

    file.close();
    return !file.fail();
}

bool DataManager::saveCourses(const CourseManager& manager) const {
    std::string path = dataDirectory + "/courses.txt";
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    file << std::setprecision(17);
    for (const auto& c : manager.courses) {
        file << escapeField(c.getCourseCode()) << '|'
             << escapeField(c.getCourseName()) << '|'
             << c.getCredits() << '|'
             << c.getCapacity() << '|'
             << escapeField(c.getFacultyId()) << '|'
             << (c.isActive() ? "1" : "0") << '\n';
    }

    file.close();
    return !file.fail();
}

bool DataManager::saveEnrollments(const EnrollmentManager& manager) const {
    std::string path = dataDirectory + "/enrollments.txt";
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& e : manager.enrollments) {
        file << escapeField(e.getStudentRollNo()) << '|'
             << escapeField(e.getCourseCode()) << '|'
             << escapeField(e.getEnrollmentDate()) << '|'
             << (e.isActive() ? "1" : "0") << '\n';
    }

    file.close();
    return !file.fail();
}

bool DataManager::saveAttendance(const AttendanceManager& manager) const {
    std::string path = dataDirectory + "/attendance.txt";
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& a : manager.attendanceRecords) {
        file << escapeField(a.getStudentRollNo()) << '|'
             << escapeField(a.getCourseCode()) << '|'
             << escapeField(a.getDate()) << '|'
             << (a.isPresent() ? "1" : "0") << '\n';
    }

    file.close();
    return !file.fail();
}

bool DataManager::savePayments(const FeeManager& manager) const {
    std::string path = dataDirectory + "/payments.txt";
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    file << std::setprecision(17);
    for (const auto& p : manager.payments) {
        file << escapeField(p.getReceiptNo()) << '|'
             << escapeField(p.getStudentRollNo()) << '|'
             << p.getAmount() << '|'
             << escapeField(p.getPaymentDate()) << '|'
             << escapeField(p.getPaymentMode()) << '\n';
    }

    file.close();
    return !file.fail();
}

// ============================================================================
// Load Methods
// ============================================================================

bool DataManager::loadStudents(StudentManager& manager) const {
    std::string path = dataDirectory + "/students.txt";
    if (!fileExists(path)) {
        manager.students.clear();
        return true;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::vector<Student> temp;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto fields = splitLine(line);
        if (fields.size() != 8) return false;

        int semester;
        double totalFee;
        bool active;
        if (!parseInteger(fields[5], semester)) return false;
        if (!parseDouble(fields[6], totalFee)) return false;
        if (!parseBool(fields[7], active)) return false;

        Student s(fields[0], fields[1], fields[2], fields[3],
                  fields[4], semester, totalFee);
        if (!active) {
            s.setActive(false);
        }
        temp.push_back(s);
    }

    manager.students = temp;
    return true;
}

bool DataManager::loadFaculty(FacultyManager& manager) const {
    std::string path = dataDirectory + "/faculty.txt";
    if (!fileExists(path)) {
        manager.facultyMembers.clear();
        return true;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::vector<Faculty> temp;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto fields = splitLine(line);
        if (fields.size() != 7) return false;

        bool active;
        if (!parseBool(fields[6], active)) return false;

        Faculty f(fields[0], fields[1], fields[2], fields[3],
                  fields[4], fields[5]);
        if (!active) {
            f.setActive(false);
        }
        temp.push_back(f);
    }

    manager.facultyMembers = temp;
    return true;
}

bool DataManager::loadCourses(CourseManager& manager) const {
    std::string path = dataDirectory + "/courses.txt";
    if (!fileExists(path)) {
        manager.courses.clear();
        return true;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::vector<Course> temp;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto fields = splitLine(line);
        if (fields.size() != 6) return false;

        int credits, capacity;
        bool active;
        if (!parseInteger(fields[2], credits)) return false;
        if (!parseInteger(fields[3], capacity)) return false;
        if (!parseBool(fields[5], active)) return false;

        Course c(fields[0], fields[1], credits, capacity, fields[4]);
        if (!active) {
            c.setActive(false);
        }
        temp.push_back(c);
    }

    manager.courses = temp;
    return true;
}

bool DataManager::loadEnrollments(EnrollmentManager& manager) const {
    std::string path = dataDirectory + "/enrollments.txt";
    if (!fileExists(path)) {
        manager.enrollments.clear();
        return true;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::vector<Enrollment> temp;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto fields = splitLine(line);
        if (fields.size() != 4) return false;

        bool active;
        if (!parseBool(fields[3], active)) return false;

        Enrollment e(fields[0], fields[1], fields[2]);
        if (!active) {
            e.drop();
        }
        temp.push_back(e);
    }

    manager.enrollments = temp;
    return true;
}

bool DataManager::loadAttendance(AttendanceManager& manager) const {
    std::string path = dataDirectory + "/attendance.txt";
    if (!fileExists(path)) {
        manager.attendanceRecords.clear();
        return true;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::vector<AttendanceRecord> temp;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto fields = splitLine(line);
        if (fields.size() != 4) return false;

        bool present;
        if (!parseBool(fields[3], present)) return false;

        AttendanceRecord a(fields[0], fields[1], fields[2], present);
        temp.push_back(a);
    }

    manager.attendanceRecords = temp;
    return true;
}

bool DataManager::loadPayments(FeeManager& manager) const {
    std::string path = dataDirectory + "/payments.txt";
    if (!fileExists(path)) {
        manager.payments.clear();
        return true;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::vector<FeePayment> temp;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto fields = splitLine(line);
        if (fields.size() != 5) return false;

        double amount;
        if (!parseDouble(fields[2], amount)) return false;

        FeePayment p(fields[0], fields[1], amount, fields[3], fields[4]);
        temp.push_back(p);
    }

    manager.payments = temp;
    return true;
}

// ============================================================================
// Combined Operations
// ============================================================================

bool DataManager::saveAllData(
        const StudentManager& studentManager,
        const FacultyManager& facultyManager,
        const CourseManager& courseManager,
        const EnrollmentManager& enrollmentManager,
        const AttendanceManager& attendanceManager,
        const FeeManager& feeManager) const {

    bool success = true;
    if (!saveStudents(studentManager)) success = false;
    if (!saveFaculty(facultyManager)) success = false;
    if (!saveCourses(courseManager)) success = false;
    if (!saveEnrollments(enrollmentManager)) success = false;
    if (!saveAttendance(attendanceManager)) success = false;
    if (!savePayments(feeManager)) success = false;
    return success;
}

bool DataManager::loadAllData(
        StudentManager& studentManager,
        FacultyManager& facultyManager,
        CourseManager& courseManager,
        EnrollmentManager& enrollmentManager,
        AttendanceManager& attendanceManager,
        FeeManager& feeManager) const {

    // Load into temporary managers first
    StudentManager tempSM;
    FacultyManager tempFM;
    CourseManager tempCM;
    EnrollmentManager tempEM;
    AttendanceManager tempAM;
    FeeManager tempFeeM;

    if (!loadStudents(tempSM)) return false;
    if (!loadFaculty(tempFM)) return false;
    if (!loadCourses(tempCM)) return false;
    if (!loadEnrollments(tempEM)) return false;
    if (!loadAttendance(tempAM)) return false;
    if (!loadPayments(tempFeeM)) return false;

    // All loaded successfully — commit
    studentManager.students = tempSM.students;
    facultyManager.facultyMembers = tempFM.facultyMembers;
    courseManager.courses = tempCM.courses;
    enrollmentManager.enrollments = tempEM.enrollments;
    attendanceManager.attendanceRecords = tempAM.attendanceRecords;
    feeManager.payments = tempFeeM.payments;

    return true;
}

} // namespace college
