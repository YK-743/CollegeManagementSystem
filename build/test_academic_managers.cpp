// Temporary smoke test for EnrollmentManager and AttendanceManager.
// This file lives under build/ and is not part of the main application.

#include "college/Models.h"
#include "college/Managers.h"
#include <iostream>
#include <string>
#include <cmath>

static int testsPassed = 0;
static int testsFailed = 0;

void check(const std::string& testName, bool condition) {
    if (condition) {
        std::cout << "  PASS: " << testName << std::endl;
        ++testsPassed;
    } else {
        std::cout << "  FAIL: " << testName << std::endl;
        ++testsFailed;
    }
}

// Helper: compare doubles with tolerance
bool approxEqual(double a, double b, double tolerance = 0.01) {
    return std::fabs(a - b) < tolerance;
}

void testEnrollmentManager() {
    std::cout << "\n=== EnrollmentManager Tests ===" << std::endl;

    // Set up prerequisite managers with test data
    college::StudentManager sm;
    college::CourseManager cm;

    college::Student s1("22CS101", "Ravi Kumar", "ravi@college.edu", "9876543210",
                        "Computer Science", 4, 75000.0);
    college::Student s2("22CS102", "Priya Singh", "priya@college.edu", "9876543211",
                        "Electronics", 3, 70000.0);
    college::Student s3("22CS103", "Inactive Student", "inactive@college.edu", "0000000000",
                        "Maths", 1, 50000.0);
    sm.addStudent(s1);
    sm.addStudent(s2);
    sm.addStudent(s3);
    sm.removeStudent("22CS103"); // deactivate s3

    // Course with capacity 2 for capacity testing
    college::Course c1("CS201", "Data Structures", 4, 2, "");
    college::Course c2("CS301", "Algorithms", 3, 50, "");
    college::Course c3("CS401", "Inactive Course", 3, 50, "");
    cm.addCourse(c1);
    cm.addCourse(c2);
    cm.addCourse(c3);
    cm.removeCourse("CS401"); // deactivate c3

    college::EnrollmentManager em;

    // 1. Enrol a valid student in a valid course
    check("Enrol s1 in CS201",
          em.enrollStudent("22CS101", "CS201", "2026-08-01", sm, cm));

    // 2. Reject nonexistent student
    check("Reject nonexistent student",
          !em.enrollStudent("99XX999", "CS201", "2026-08-01", sm, cm));

    // 3. Reject nonexistent course
    check("Reject nonexistent course",
          !em.enrollStudent("22CS101", "XX999", "2026-08-01", sm, cm));

    // 4. Reject inactive student
    check("Reject inactive student",
          !em.enrollStudent("22CS103", "CS201", "2026-08-01", sm, cm));

    // 5. Reject inactive course
    check("Reject inactive course",
          !em.enrollStudent("22CS101", "CS401", "2026-08-01", sm, cm));

    // 6. Reject duplicate active enrollment
    check("Reject duplicate active enrollment",
          !em.enrollStudent("22CS101", "CS201", "2026-08-02", sm, cm));

    // 7. Enforce course capacity (CS201 has capacity 2)
    check("Enrol s2 in CS201 (fills to capacity)",
          em.enrollStudent("22CS102", "CS201", "2026-08-01", sm, cm));
    // CS201 now at capacity (2 active enrollments)
    // We need a third active student to test capacity overflow
    college::Student s4("22CS104", "Test Student", "test@college.edu", "1111111111",
                        "Computer Science", 2, 60000.0);
    sm.addStudent(s4);
    check("Reject enrollment when course at capacity",
          !em.enrollStudent("22CS104", "CS201", "2026-08-01", sm, cm));

    // 8. Drop an active enrollment
    check("Drop s1 from CS201",
          em.dropCourse("22CS101", "CS201"));

    // 9. Reject dropping an already dropped enrollment
    check("Reject dropping already dropped enrollment",
          !em.dropCourse("22CS101", "CS201"));

    // 10. Verify dropped enrollment is preserved
    const auto& allEnrollments = em.getAllEnrollments();
    bool foundDropped = false;
    for (const auto& e : allEnrollments) {
        if (e.getStudentRollNo() == "22CS101" &&
            e.getCourseCode() == "CS201" &&
            !e.isActive()) {
            foundDropped = true;
            break;
        }
    }
    check("Dropped enrollment record is preserved", foundDropped);

    // 11. Verify dropping releases course capacity
    check("After drop, s4 can enrol in CS201 (capacity freed)",
          em.enrollStudent("22CS104", "CS201", "2026-08-02", sm, cm));

    // 12. Allow re-enrollment after dropping
    // First drop s4 to make room, then re-enrol s1
    em.dropCourse("22CS104", "CS201");
    check("Re-enrol s1 in CS201 after dropping",
          em.enrollStudent("22CS101", "CS201", "2026-09-01", sm, cm));

    // 13. Verify historical and active enrollments coexist
    int s1Records = 0;
    int s1Active = 0;
    for (const auto& e : em.getAllEnrollments()) {
        if (e.getStudentRollNo() == "22CS101" && e.getCourseCode() == "CS201") {
            ++s1Records;
            if (e.isActive()) {
                ++s1Active;
            }
        }
    }
    check("s1 has 2 total enrollment records for CS201", s1Records == 2);
    check("s1 has 1 active enrollment record for CS201", s1Active == 1);

    // 14. Verify getEnrolledCount() counts only active records
    // CS201 should have: s2 (active), s1 re-enrolled (active), s4 (dropped) = 2 active
    check("getEnrolledCount(CS201) is 2",
          em.getEnrolledCount("CS201") == 2);

    // 15. Verify getStudentEnrollments() returns active enrollments only
    // Enrol s1 in CS301 too
    em.enrollStudent("22CS101", "CS301", "2026-08-01", sm, cm);
    auto s1Enrollments = em.getStudentEnrollments("22CS101");
    check("getStudentEnrollments(s1) returns 2 active enrollments",
          static_cast<int>(s1Enrollments.size()) == 2);
    // Verify they are all active
    bool allActive = true;
    for (const auto& e : s1Enrollments) {
        if (!e.isActive()) {
            allActive = false;
            break;
        }
    }
    check("All returned student enrollments are active", allActive);
}

void testAttendanceManager() {
    std::cout << "\n=== AttendanceManager Tests ===" << std::endl;

    // Set up prerequisite managers
    college::StudentManager sm;
    college::CourseManager cm;
    college::EnrollmentManager em;

    college::Student s1("22CS101", "Ravi Kumar", "ravi@college.edu", "9876543210",
                        "Computer Science", 4, 75000.0);
    college::Student s2("22CS102", "Priya Singh", "priya@college.edu", "9876543211",
                        "Electronics", 3, 70000.0);
    sm.addStudent(s1);
    sm.addStudent(s2);

    college::Course c1("CS201", "Data Structures", 4, 60, "");
    college::Course c2("CS301", "Algorithms", 3, 50, "");
    cm.addCourse(c1);
    cm.addCourse(c2);

    // Enrol s1 in CS201 and CS301, s2 in CS201
    em.enrollStudent("22CS101", "CS201", "2026-08-01", sm, cm);
    em.enrollStudent("22CS101", "CS301", "2026-08-01", sm, cm);
    em.enrollStudent("22CS102", "CS201", "2026-08-01", sm, cm);

    college::AttendanceManager am;

    // 1. Mark a student present
    check("Mark s1 present in CS201 on 2026-09-01",
          am.markAttendance("22CS101", "CS201", "2026-09-01", true, sm, cm, em));

    // 2. Mark a student absent
    check("Mark s1 absent in CS201 on 2026-09-02",
          am.markAttendance("22CS101", "CS201", "2026-09-02", false, sm, cm, em));

    // 3. Reject attendance for nonexistent student
    check("Reject nonexistent student attendance",
          !am.markAttendance("99XX999", "CS201", "2026-09-01", true, sm, cm, em));

    // 4. Reject attendance for nonexistent course
    check("Reject nonexistent course attendance",
          !am.markAttendance("22CS101", "XX999", "2026-09-01", true, sm, cm, em));

    // 5. Reject attendance for a student not enrolled
    check("Reject attendance for unenrolled student",
          !am.markAttendance("22CS102", "CS301", "2026-09-01", true, sm, cm, em));

    // 6. Reject duplicate attendance for the same date
    check("Reject duplicate attendance same date",
          !am.markAttendance("22CS101", "CS201", "2026-09-01", true, sm, cm, em));

    // 7. Correct an existing attendance record (change absent to present)
    check("Correct attendance: 2026-09-02 absent -> present",
          am.updateAttendance("22CS101", "CS201", "2026-09-02", true));
    // Verify the correction took effect by checking total present in percentage
    // After correction: 2 present out of 2 total = 100%

    // 8. Reject correcting a nonexistent record
    check("Reject correcting nonexistent record",
          !am.updateAttendance("22CS101", "CS201", "2026-12-31", true));

    // Add more attendance records to test percentage calculation:
    // s1 in CS201: day1=present, day2=present(corrected), day3=present, day4=present, day5=absent
    am.markAttendance("22CS101", "CS201", "2026-09-03", true, sm, cm, em);
    am.markAttendance("22CS101", "CS201", "2026-09-04", true, sm, cm, em);
    am.markAttendance("22CS101", "CS201", "2026-09-05", false, sm, cm, em);

    // Now: present=4 (day1, day2 corrected, day3, day4), absent=1 (day5)
    // Total=5, Present=4, Percentage=80.0%

    // 9. Calculate attendance percentage correctly
    double pct = am.calculatePercentage("22CS101", "CS201");
    check("Attendance percentage is 80.0%", approxEqual(pct, 80.0));

    // 10. Verify 0.0 when no records exist
    check("Percentage is 0.0 for no records",
          approxEqual(am.calculatePercentage("22CS102", "CS301"), 0.0));

    // 11. Verify student attendance history
    auto s1Attendance = am.getStudentAttendance("22CS101");
    check("getStudentAttendance(s1) returns 5 records",
          static_cast<int>(s1Attendance.size()) == 5);

    // 12. Verify course attendance history
    // Mark s2 in CS201 too
    am.markAttendance("22CS102", "CS201", "2026-09-01", true, sm, cm, em);
    auto cs201Attendance = am.getCourseAttendance("CS201");
    check("getCourseAttendance(CS201) returns 6 records",
          static_cast<int>(cs201Attendance.size()) == 6);

    // 13. Verify total class count
    check("getTotalClasses(s1, CS201) is 5",
          am.getTotalClasses("22CS101", "CS201") == 5);
    check("getTotalClasses(s2, CS201) is 1",
          am.getTotalClasses("22CS102", "CS201") == 1);

    // --- Test: attendance history survives after dropping a course ---
    std::cout << "\n  --- Attendance persistence after drop ---" << std::endl;
    em.dropCourse("22CS101", "CS201");
    auto s1AttAfterDrop = am.getStudentAttendance("22CS101");
    check("Attendance records preserved after dropping course",
          static_cast<int>(s1AttAfterDrop.size()) == 5);
    double pctAfterDrop = am.calculatePercentage("22CS101", "CS201");
    check("Percentage still calculable after drop", approxEqual(pctAfterDrop, 80.0));

    // --- Verify getAllAttendanceRecords returns entire collection ---
    check("getAllAttendanceRecords returns 6 total records",
          static_cast<int>(am.getAllAttendanceRecords().size()) == 6);
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  ACADEMIC MANAGERS SMOKE TEST" << std::endl;
    std::cout << "========================================" << std::endl;

    testEnrollmentManager();
    testAttendanceManager();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  RESULTS: " << testsPassed << " passed, " << testsFailed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return testsFailed > 0 ? 1 : 0;
}
