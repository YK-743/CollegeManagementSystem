// Phase 8 E2E Functional & Edge-Case Verification Test
// Tests all functional modules, boundary conditions, and edge cases.
// Uses an isolated directory (build/testdata_e2e) to avoid touching live data.

#include "college/Models.h"
#include "college/Managers.h"
#include "college/DataManager.h"
#include "college/CollegeSystem.h"
#include <iostream>
#include <string>
#include <cmath>

static int passed = 0;
static int failed = 0;

void check(const std::string& label, bool condition) {
    if (condition) {
        std::cout << "  PASS: " << label << "\n";
        ++passed;
    } else {
        std::cout << "  FAIL: " << label << "\n";
        ++failed;
    }
}

bool approxEqual(double a, double b, double eps = 0.01) {
    return std::fabs(a - b) < eps;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "  PHASE 8 E2E & EDGE-CASE TEST SUITE\n";
    std::cout << "========================================\n";

    // 1. Student Edge Cases
    std::cout << "\n=== 1. Student Edge Cases ===\n";
    {
        college::StudentManager sm;
        // Names with spaces
        college::Student s1("S001", "Mary Jane Watson", "mj@univ.edu", "+1-555-0199",
                            "Computer Science & Engineering", 3, 65000.0);
        check("Register student with spaces in name and dept", sm.addStudent(s1));
        check("Reject duplicate roll number", !sm.addStudent(s1));

        // Update student
        college::Student s1up("S001", "Mary Jane Parker", "mjp@univ.edu", "+1-555-0199",
                              "Information Technology", 4, 65000.0);
        check("Update existing active student", sm.updateStudent("S001", s1up));
        check("Updated name matches", sm.findStudent("S001")->getName() == "Mary Jane Parker");
        check("Updated semester matches", sm.findStudent("S001")->getSemester() == 4);

        // Deactivation
        check("Deactivate student", sm.removeStudent("S001"));
        check("Deactivated student is inactive", !sm.findStudent("S001")->isActive());
        check("Active student count is 0", sm.getTotalStudents() == 0);
        check("Reject update on inactive student", !sm.updateStudent("S001", s1up));
        check("Reject removing already inactive student", !sm.removeStudent("S001"));
    }

    // 2. Faculty Edge Cases
    std::cout << "\n=== 2. Faculty Edge Cases ===\n";
    {
        college::FacultyManager fm;
        college::Faculty f1("F101", "Dr. Robert Bruce Banner", "banner@univ.edu", "555-1234",
                            "Nuclear Physics", "Senior Professor");
        check("Register faculty with spaces in name and designation", fm.addFaculty(f1));
        check("Reject duplicate faculty ID", !fm.addFaculty(f1));

        college::Faculty f1up("F101", "Prof. R. B. Banner", "banner2@univ.edu", "555-1234",
                              "Applied Physics", "Head of Department");
        check("Update active faculty", fm.updateFaculty("F101", f1up));
        check("Updated designation matches",
              fm.findFaculty("F101")->getDesignation() == "Head of Department");

        check("Deactivate faculty", fm.removeFaculty("F101"));
        check("Deactivated faculty is inactive", !fm.findFaculty("F101")->isActive());
        check("Active faculty count is 0", fm.getTotalFaculty() == 0);
        check("Reject update on inactive faculty", !fm.updateFaculty("F101", f1up));
    }

    // 3. Course & Faculty Assignment Edge Cases
    std::cout << "\n=== 3. Course & Faculty Assignment Edge Cases ===\n";
    {
        college::CourseManager cm;
        college::FacultyManager fm;
        fm.addFaculty(college::Faculty("F201", "Dr. Charles Xavier", "cx@univ.edu", "111-222",
                                       "Genetics", "Dean"));
        fm.addFaculty(college::Faculty("F202", "Dr. Inactive", "in@univ.edu", "000",
                                       "Physics", "Lecturer"));
        fm.removeFaculty("F202"); // Inactive

        college::Course c1("BIO301", "Advanced Molecular Genetics", 4, 30, "");
        check("Add course with spaces in name", cm.addCourse(c1));
        check("Reject duplicate course code", !cm.addCourse(c1));

        // Assign faculty
        check("Assign active faculty to course", cm.assignFaculty("BIO301", "F201", fm));
        check("Assigned faculty ID correct", cm.findCourse("BIO301")->getFacultyId() == "F201");
        check("Reject assigning inactive faculty", !cm.assignFaculty("BIO301", "F202", fm));
        check("Reject assigning nonexistent faculty", !cm.assignFaculty("BIO301", "FNONE", fm));

        // Deactivate course
        check("Deactivate course", cm.removeCourse("BIO301"));
        check("Course is inactive", !cm.findCourse("BIO301")->isActive());
        check("Reject assigning faculty to inactive course",
              !cm.assignFaculty("BIO301", "F201", fm));
    }

    // 4. Enrollment & Capacity Edge Cases
    std::cout << "\n=== 4. Enrollment & Capacity Edge Cases ===\n";
    {
        college::StudentManager sm;
        college::CourseManager cm;
        college::EnrollmentManager em;

        sm.addStudent(college::Student("S1", "Alice", "a@u.edu", "1", "CS", 1, 50000));
        sm.addStudent(college::Student("S2", "Bob", "b@u.edu", "2", "CS", 1, 50000));
        sm.addStudent(college::Student("S3", "Charlie", "c@u.edu", "3", "CS", 1, 50000));
        sm.addStudent(college::Student("SINACTIVE", "Ghost", "g@u.edu", "0", "CS", 1, 50000));
        sm.removeStudent("SINACTIVE");

        cm.addCourse(college::Course("CS100", "Basics", 2, 2, "")); // capacity = 2
        cm.addCourse(college::Course("CINACTIVE", "Old", 2, 10, ""));
        cm.removeCourse("CINACTIVE");

        // Reject invalid student / course
        check("Reject enrollment for nonexistent student",
              !em.enrollStudent("SNONE", "CS100", "2026-01-01", sm, cm));
        check("Reject enrollment for nonexistent course",
              !em.enrollStudent("S1", "CNONE", "2026-01-01", sm, cm));
        check("Reject enrollment for inactive student",
              !em.enrollStudent("SINACTIVE", "CS100", "2026-01-01", sm, cm));
        check("Reject enrollment for inactive course",
              !em.enrollStudent("S1", "CINACTIVE", "2026-01-01", sm, cm));

        // Enforce capacity
        check("Enroll student S1", em.enrollStudent("S1", "CS100", "2026-01-01", sm, cm));
        check("Reject duplicate enrollment", !em.enrollStudent("S1", "CS100", "2026-01-02", sm, cm));
        check("Enroll student S2 (course now full)",
              em.enrollStudent("S2", "CS100", "2026-01-01", sm, cm));
        check("Enrolled count is 2", em.getEnrolledCount("CS100") == 2);
        check("Reject S3 due to capacity limit",
              !em.enrollStudent("S3", "CS100", "2026-01-01", sm, cm));

        // Drop and re-enroll
        check("Drop S1", em.dropCourse("S1", "CS100"));
        check("Reject dropping already dropped enrollment", !em.dropCourse("S1", "CS100"));
        check("Capacity freed: enrolled count is 1", em.getEnrolledCount("CS100") == 1);
        check("S3 can now enroll in freed slot",
              em.enrollStudent("S3", "CS100", "2026-01-03", sm, cm));

        // Drop S2 and re-enroll S1
        check("Drop S2", em.dropCourse("S2", "CS100"));
        check("Re-enroll S1 after dropping",
              em.enrollStudent("S1", "CS100", "2026-01-04", sm, cm));
        check("Historical enrollment records preserved", em.getAllEnrollments().size() == 4);
    }

    // 5. Attendance Edge Cases
    std::cout << "\n=== 5. Attendance Edge Cases ===\n";
    {
        college::StudentManager sm;
        college::CourseManager cm;
        college::EnrollmentManager em;
        college::AttendanceManager am;

        sm.addStudent(college::Student("S1", "Alice", "a@u.edu", "1", "CS", 1, 50000));
        sm.addStudent(college::Student("S2", "Bob", "b@u.edu", "2", "CS", 1, 50000));
        cm.addCourse(college::Course("CS100", "Basics", 2, 20, ""));
        em.enrollStudent("S1", "CS100", "2026-01-01", sm, cm);
        // S2 not enrolled

        // Reject attendance without enrollment
        check("Reject attendance for unenrolled student S2",
              !am.markAttendance("S2", "CS100", "2026-01-10", true, sm, cm, em));

        // Mark present and absent
        check("Mark S1 present",
              am.markAttendance("S1", "CS100", "2026-01-10", true, sm, cm, em));
        check("Mark S1 absent",
              am.markAttendance("S1", "CS100", "2026-01-11", false, sm, cm, em));
        check("Reject duplicate attendance on same date",
              !am.markAttendance("S1", "CS100", "2026-01-10", false, sm, cm, em));

        // Correct attendance
        check("Correct 2026-01-11 from absent to present",
              am.updateAttendance("S1", "CS100", "2026-01-11", true));
        check("Reject correction on nonexistent date",
              !am.updateAttendance("S1", "CS100", "2026-12-31", true));

        // Percentage calculations
        check("Percentage is 100% after correction",
              approxEqual(am.calculatePercentage("S1", "CS100"), 100.0));
        check("Percentage is 0.0 for course with no records",
              approxEqual(am.calculatePercentage("S2", "CS100"), 0.0));
    }

    // 6. Fee Management Edge Cases
    std::cout << "\n=== 6. Fee Management Edge Cases ===\n";
    {
        college::StudentManager sm;
        college::FeeManager feeM;

        sm.addStudent(college::Student("S1", "Alice", "a@u.edu", "1", "CS", 1, 50000.0));
        sm.addStudent(college::Student("S_INACTIVE", "Ghost", "g@u.edu", "0", "CS", 1, 30000.0));
        sm.removeStudent("S_INACTIVE");

        // Reject invalid amounts
        check("Reject zero amount payment",
              !feeM.recordPayment(college::FeePayment("R1", "S1", 0.0, "2026-01-01", "Cash"), sm));
        check("Reject negative amount payment",
              !feeM.recordPayment(college::FeePayment("R1", "S1", -100.0, "2026-01-01", "Cash"), sm));

        // Reject payment for inactive or nonexistent student
        check("Reject payment for inactive student",
              !feeM.recordPayment(college::FeePayment("R1", "S_INACTIVE", 1000.0, "2026-01-01", "Cash"), sm));
        check("Reject payment for nonexistent student",
              !feeM.recordPayment(college::FeePayment("R1", "SNONE", 1000.0, "2026-01-01", "Cash"), sm));

        // Valid payments
        check("Record valid partial payment (20000)",
              feeM.recordPayment(college::FeePayment("R1", "S1", 20000.0, "2026-01-01", "Cash"), sm));
        check("Reject duplicate receipt number R1",
              !feeM.recordPayment(college::FeePayment("R1", "S1", 5000.0, "2026-01-02", "UPI"), sm));

        // Outstanding & Overpayment prevention
        check("Outstanding is 30000",
              approxEqual(feeM.calculateOutstanding("S1", sm), 30000.0));
        check("Reject payment exceeding outstanding (35000 > 30000)",
              !feeM.recordPayment(college::FeePayment("R2", "S1", 35000.0, "2026-01-03", "Card"), sm));
        check("Accept exact remaining payment (30000)",
              feeM.recordPayment(college::FeePayment("R2", "S1", 30000.0, "2026-01-03", "Card"), sm));
        check("Outstanding is now 0.0",
              approxEqual(feeM.calculateOutstanding("S1", sm), 0.0));
        check("Reject any further payment when fully paid",
              !feeM.recordPayment(college::FeePayment("R3", "S1", 1.0, "2026-01-04", "UPI"), sm));

        // Receipt generation
        std::string rcpt = feeM.generateReceipt("R1");
        check("Receipt R1 generated successfully", !rcpt.empty());
        check("Receipt contains student ID S1", rcpt.find("S1") != std::string::npos);
        check("Receipt for invalid ID returns empty", feeM.generateReceipt("RNONE").empty());
    }

    // 7. Reports & Analytics Edge Cases
    std::cout << "\n=== 7. Reports & Analytics Edge Cases ===\n";
    {
        college::StudentManager sm;
        college::FacultyManager fm;
        college::CourseManager cm;
        college::EnrollmentManager em;
        college::AttendanceManager am;
        college::FeeManager feeM;
        college::ReportManager rm;

        // Empty system reports
        auto emptyDash = rm.getDashboardSummary(sm, fm, cm, em, feeM);
        check("Empty dashboard total students = 0", emptyDash.totalStudents == 0);
        check("Empty dashboard total fees = 0.0", approxEqual(emptyDash.totalFeeCollection, 0.0));
        check("Empty fee report size = 0", rm.getFeeReport(sm, feeM).empty());
        check("Empty attendance report size = 0", rm.getAttendanceReport(sm, em, am, 75.0).empty());
        check("Empty enrollment report size = 0", rm.getEnrollmentReport(cm, em).empty());

        // Add active & inactive records
        sm.addStudent(college::Student("S1", "Active Student", "a@u.edu", "1", "CS", 1, 40000));
        sm.addStudent(college::Student("S2", "Inactive Student", "i@u.edu", "2", "CS", 1, 30000));
        sm.removeStudent("S2");

        cm.addCourse(college::Course("C1", "Active Course", 3, 40, ""));
        cm.addCourse(college::Course("C2", "Inactive Course", 3, 40, ""));
        cm.removeCourse("C2");

        em.enrollStudent("S1", "C1", "2026-01-01", sm, cm);
        am.markAttendance("S1", "C1", "2026-01-10", true, sm, cm, em);
        am.markAttendance("S1", "C1", "2026-01-11", false, sm, cm, em); // 50%
        feeM.recordPayment(college::FeePayment("R1", "S1", 15000, "2026-01-05", "Cash"), sm);

        // Reports exclude inactive entities
        auto feeRep = rm.getFeeReport(sm, feeM);
        check("Fee report includes only active student", feeRep.size() == 1 && feeRep[0].rollNo == "S1");

        auto attRep = rm.getAttendanceReport(sm, em, am, 75.0);
        check("Attendance report flags 50% below 75%", attRep.size() == 1 && attRep[0].belowRequirement);

        auto enrRep = rm.getEnrollmentReport(cm, em);
        check("Enrollment report includes only active course", enrRep.size() == 1 && enrRep[0].courseCode == "C1");
    }

    // 8. Data Persistence Edge Cases
    std::cout << "\n=== 8. Data Persistence Edge Cases ===\n";
    {
        college::DataManager dm("build/testdata_e2e");
        college::StudentManager sm1, sm2;
        college::FacultyManager fm1, fm2;
        college::CourseManager cm1, cm2;
        college::EnrollmentManager em1, em2;
        college::AttendanceManager am1, am2;
        college::FeeManager fee1, fee2;

        // Populate with complex strings containing pipes and backslashes
        sm1.addStudent(college::Student("S_PIPE", "John | Doe", "john\\doe@univ.edu", "123",
                                        "Math | Science", 2, 45000));
        fm1.addFaculty(college::Faculty("F_PIPE", "Dr. A | B", "ab@univ.edu", "456",
                                        "Physics \\ Math", "Lead | HOD"));
        cm1.addCourse(college::Course("C_PIPE", "Data | Mining", 3, 50, "F_PIPE"));
        em1.enrollStudent("S_PIPE", "C_PIPE", "2026-01-01", sm1, cm1);
        am1.markAttendance("S_PIPE", "C_PIPE", "2026-01-05", true, sm1, cm1, em1);
        fee1.recordPayment(college::FeePayment("R_PIPE", "S_PIPE", 20000, "2026-01-06", "Card | Debit"), sm1);

        check("Save data with escaped characters",
              dm.saveAllData(sm1, fm1, cm1, em1, am1, fee1));

        check("Load data with escaped characters",
              dm.loadAllData(sm2, fm2, cm2, em2, am2, fee2));

        const auto* loadedStudent = sm2.findStudent("S_PIPE");
        check("Escaped student name restored intact",
              loadedStudent != nullptr && loadedStudent->getName() == "John | Doe");
        check("Escaped student email restored intact",
              loadedStudent != nullptr && loadedStudent->getEmail() == "john\\doe@univ.edu");
        check("Escaped department restored intact",
              loadedStudent != nullptr && loadedStudent->getDepartment() == "Math | Science");

        const auto* loadedFaculty = fm2.findFaculty("F_PIPE");
        check("Escaped faculty designation restored intact",
              loadedFaculty != nullptr && loadedFaculty->getDesignation() == "Lead | HOD");

        const auto* loadedCourse = cm2.findCourse("C_PIPE");
        check("Escaped course name restored intact",
              loadedCourse != nullptr && loadedCourse->getCourseName() == "Data | Mining");

        const auto* loadedPayment = fee2.findPaymentByReceipt("R_PIPE");
        check("Escaped payment mode restored intact",
              loadedPayment != nullptr && loadedPayment->getPaymentMode() == "Card | Debit");
    }

    std::cout << "\n========================================\n";
    std::cout << "  E2E RESULTS: " << passed << " passed, " << failed << " failed\n";
    std::cout << "========================================\n";

    return (failed > 0) ? 1 : 0;
}
