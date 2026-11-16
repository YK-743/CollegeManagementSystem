// Temporary smoke test for FeeManager and ReportManager.
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

bool approxEqual(double a, double b, double tolerance = 0.01) {
    return std::fabs(a - b) < tolerance;
}

// ---------------------------------------------------------------------------
// Shared test data setup
// ---------------------------------------------------------------------------
struct TestFixture {
    college::StudentManager sm;
    college::FacultyManager fm;
    college::CourseManager cm;
    college::EnrollmentManager em;
    college::AttendanceManager am;
    college::FeeManager feeM;

    void setup() {
        // Students
        sm.addStudent(college::Student("22CS101", "Ravi Kumar", "ravi@c.edu", "9876543210",
                                       "CS", 4, 75000.0));
        sm.addStudent(college::Student("22CS102", "Priya Singh", "priya@c.edu", "9876543211",
                                       "EC", 3, 60000.0));
        sm.addStudent(college::Student("22CS103", "Inactive Student", "inactive@c.edu", "0000000000",
                                       "ME", 1, 50000.0));
        sm.removeStudent("22CS103"); // deactivate

        // Faculty
        fm.addFaculty(college::Faculty("FAC001", "Dr. Sharma", "sharma@c.edu", "9988776655",
                                       "CS", "Professor"));

        // Courses
        cm.addCourse(college::Course("CS201", "Data Structures", 4, 60, "FAC001"));
        cm.addCourse(college::Course("CS301", "Algorithms", 3, 50, ""));
        cm.addCourse(college::Course("CS401", "Inactive Course", 3, 50, ""));
        cm.removeCourse("CS401"); // deactivate

        // Enrollments
        em.enrollStudent("22CS101", "CS201", "2026-08-01", sm, cm);
        em.enrollStudent("22CS101", "CS301", "2026-08-01", sm, cm);
        em.enrollStudent("22CS102", "CS201", "2026-08-01", sm, cm);

        // Attendance for s1 in CS201: 4 present, 1 absent = 80%
        am.markAttendance("22CS101", "CS201", "2026-09-01", true,  sm, cm, em);
        am.markAttendance("22CS101", "CS201", "2026-09-02", true,  sm, cm, em);
        am.markAttendance("22CS101", "CS201", "2026-09-03", true,  sm, cm, em);
        am.markAttendance("22CS101", "CS201", "2026-09-04", true,  sm, cm, em);
        am.markAttendance("22CS101", "CS201", "2026-09-05", false, sm, cm, em);

        // Attendance for s2 in CS201: 2 present, 1 absent = 66.67%
        am.markAttendance("22CS102", "CS201", "2026-09-01", true,  sm, cm, em);
        am.markAttendance("22CS102", "CS201", "2026-09-02", true,  sm, cm, em);
        am.markAttendance("22CS102", "CS201", "2026-09-03", false, sm, cm, em);
    }
};

// ---------------------------------------------------------------------------
void testFeeManager() {
    std::cout << "\n=== FeeManager Tests ===" << std::endl;

    TestFixture t;
    t.setup();
    college::FeeManager& feeM = t.feeM;
    college::StudentManager& sm = t.sm;

    // 1. Record a valid payment
    college::FeePayment p1("REC001", "22CS101", 25000.0, "2026-07-15", "UPI");
    check("Record valid payment REC001", feeM.recordPayment(p1, sm));

    // 2. Reject payment for nonexistent student
    college::FeePayment pBad1("REC_X", "99XX999", 1000.0, "2026-07-15", "Cash");
    check("Reject payment for nonexistent student", !feeM.recordPayment(pBad1, sm));

    // 3. Reject payment for inactive student
    college::FeePayment pBad2("REC_Y", "22CS103", 1000.0, "2026-07-15", "Cash");
    check("Reject payment for inactive student", !feeM.recordPayment(pBad2, sm));

    // 4. Reject zero payment
    college::FeePayment pZero("REC_Z", "22CS101", 0.0, "2026-07-15", "Cash");
    check("Reject zero payment", !feeM.recordPayment(pZero, sm));

    // 5. Reject negative payment
    college::FeePayment pNeg("REC_N", "22CS101", -500.0, "2026-07-15", "Cash");
    check("Reject negative payment", !feeM.recordPayment(pNeg, sm));

    // 6. Reject duplicate receipt number
    college::FeePayment pDup("REC001", "22CS101", 5000.0, "2026-08-01", "Card");
    check("Reject duplicate receipt number", !feeM.recordPayment(pDup, sm));

    // Record another payment for s1
    college::FeePayment p2("REC002", "22CS101", 20000.0, "2026-08-15", "Card");
    check("Record second payment REC002", feeM.recordPayment(p2, sm));

    // 7. Reject payment exceeding outstanding fees
    // s1: totalFee=75000, paid=45000, outstanding=30000
    college::FeePayment pOver("REC003", "22CS101", 30001.0, "2026-09-01", "Cash");
    check("Reject payment exceeding outstanding", !feeM.recordPayment(pOver, sm));

    // 8. Accept payment equal to remaining outstanding fees
    college::FeePayment pExact("REC003", "22CS101", 30000.0, "2026-09-01", "Cash");
    check("Accept payment equal to remaining outstanding", feeM.recordPayment(pExact, sm));

    // 9. Calculate total paid correctly
    check("Total paid for s1 is 75000", approxEqual(feeM.calculateTotalPaid("22CS101"), 75000.0));

    // 10. Calculate outstanding fees correctly
    check("Outstanding for s1 is 0.0",
          approxEqual(feeM.calculateOutstanding("22CS101", sm), 0.0));

    // 11. Return -1.0 for nonexistent student
    check("Outstanding returns -1.0 for nonexistent student",
          approxEqual(feeM.calculateOutstanding("99XX999", sm), -1.0));

    // Record a payment for s2
    college::FeePayment p4("REC004", "22CS102", 15000.0, "2026-07-20", "UPI");
    feeM.recordPayment(p4, sm);

    // 12. Retrieve payment history
    auto s1History = feeM.getPaymentHistory("22CS101");
    check("Payment history for s1 has 3 records", static_cast<int>(s1History.size()) == 3);

    auto s2History = feeM.getPaymentHistory("22CS102");
    check("Payment history for s2 has 1 record", static_cast<int>(s2History.size()) == 1);

    // 13. Find an existing receipt
    const college::FeePayment* found = feeM.findPaymentByReceipt("REC001");
    check("Find existing receipt REC001", found != nullptr);
    check("Found receipt has correct amount", found != nullptr && approxEqual(found->getAmount(), 25000.0));

    // 14. Return nullptr for nonexistent receipt
    check("Return nullptr for nonexistent receipt", feeM.findPaymentByReceipt("REC999") == nullptr);

    // 15. Generate a valid receipt
    std::string receipt = feeM.generateReceipt("REC001");
    check("Generated receipt is not empty", !receipt.empty());
    check("Receipt contains receipt number", receipt.find("REC001") != std::string::npos);
    check("Receipt contains student roll no", receipt.find("22CS101") != std::string::npos);
    check("Receipt contains amount 25000.00", receipt.find("25000.00") != std::string::npos);
    check("Receipt contains payment date", receipt.find("2026-07-15") != std::string::npos);
    check("Receipt contains payment mode", receipt.find("UPI") != std::string::npos);
    check("Receipt contains CMS heading", receipt.find("COLLEGE MANAGEMENT SYSTEM") != std::string::npos);

    // 16. Return empty string for invalid receipt
    check("Empty string for invalid receipt", feeM.generateReceipt("REC999").empty());

    // 17. Calculate total college fee collection correctly
    // REC001=25000 + REC002=20000 + REC003=30000 + REC004=15000 = 90000
    check("Total college collection is 90000",
          approxEqual(feeM.getTotalCollection(), 90000.0));
}

// ---------------------------------------------------------------------------
void testReportManager() {
    std::cout << "\n=== ReportManager Tests ===" << std::endl;

    TestFixture t;
    t.setup();

    // Record some fee payments
    t.feeM.recordPayment(college::FeePayment("REC001", "22CS101", 25000.0, "2026-07-15", "UPI"), t.sm);
    t.feeM.recordPayment(college::FeePayment("REC002", "22CS101", 20000.0, "2026-08-15", "Card"), t.sm);
    t.feeM.recordPayment(college::FeePayment("REC003", "22CS102", 15000.0, "2026-07-20", "Cash"), t.sm);

    college::ReportManager rm;

    // --- Dashboard tests ---
    std::cout << "\n  --- Dashboard Summary ---" << std::endl;
    auto dashboard = rm.getDashboardSummary(t.sm, t.fm, t.cm, t.em, t.feeM);

    // 1. Correct active student count (s1 + s2 = 2, s3 inactive)
    check("Dashboard: active students = 2", dashboard.totalStudents == 2);

    // 2. Correct active faculty count
    check("Dashboard: active faculty = 1", dashboard.totalFaculty == 1);

    // 3. Correct active course count (CS201 + CS301 = 2, CS401 inactive)
    check("Dashboard: active courses = 2", dashboard.totalCourses == 2);

    // 4. Correct active enrollment count (s1-CS201, s1-CS301, s2-CS201 = 3)
    check("Dashboard: active enrollments = 3", dashboard.totalEnrollments == 3);

    // 5. Total fee collection (25000 + 20000 + 15000 = 60000)
    check("Dashboard: total collection = 60000",
          approxEqual(dashboard.totalFeeCollection, 60000.0));

    // 6. Total outstanding fees
    // s1: 75000 - 45000 = 30000, s2: 60000 - 15000 = 45000 => 75000
    check("Dashboard: outstanding fees = 75000",
          approxEqual(dashboard.totalOutstandingFees, 75000.0));

    // --- Fee Report tests ---
    std::cout << "\n  --- Fee Report ---" << std::endl;
    auto feeReport = rm.getFeeReport(t.sm, t.feeM);

    // 7. Fee report includes active students
    check("Fee report has 2 rows (active students only)", static_cast<int>(feeReport.size()) == 2);

    // 8. Fee report calculates individual outstanding balances
    bool foundS1 = false, foundS2 = false;
    for (const auto& row : feeReport) {
        if (row.rollNo == "22CS101") {
            foundS1 = true;
            check("Fee report s1: totalFee=75000", approxEqual(row.totalFee, 75000.0));
            check("Fee report s1: totalPaid=45000", approxEqual(row.totalPaid, 45000.0));
            check("Fee report s1: outstanding=30000", approxEqual(row.outstanding, 30000.0));
        }
        if (row.rollNo == "22CS102") {
            foundS2 = true;
            check("Fee report s2: outstanding=45000", approxEqual(row.outstanding, 45000.0));
        }
    }
    check("Fee report contains s1", foundS1);
    check("Fee report contains s2", foundS2);

    // 9. Fee report excludes inactive students
    bool foundInactive = false;
    for (const auto& row : feeReport) {
        if (row.rollNo == "22CS103") {
            foundInactive = true;
        }
    }
    check("Fee report excludes inactive student s3", !foundInactive);

    // --- Attendance Report tests ---
    std::cout << "\n  --- Attendance Report ---" << std::endl;
    auto attReport = rm.getAttendanceReport(t.sm, t.em, t.am, 75.0);

    // 10. Attendance report calculates percentages correctly
    // s1 CS201: 4/5=80%, s2 CS201: 2/3=66.67%
    // s1 CS301: 0 records -> skipped
    bool foundS1CS201 = false, foundS2CS201 = false;
    for (const auto& row : attReport) {
        if (row.rollNo == "22CS101" && row.courseCode == "CS201") {
            foundS1CS201 = true;
            check("Attendance s1 CS201: percentage ~80%",
                  approxEqual(row.attendancePercentage, 80.0));
            check("Attendance s1 CS201: totalClasses=5", row.totalClasses == 5);
            // 11. Identifies students below threshold
            check("Attendance s1 CS201: above 75% requirement", !row.belowRequirement);
        }
        if (row.rollNo == "22CS102" && row.courseCode == "CS201") {
            foundS2CS201 = true;
            check("Attendance s2 CS201: percentage ~66.67%",
                  approxEqual(row.attendancePercentage, 66.67));
            check("Attendance s2 CS201: below 75% requirement", row.belowRequirement);
        }
    }
    check("Attendance report contains s1-CS201", foundS1CS201);
    check("Attendance report contains s2-CS201", foundS2CS201);

    // 12. Does not classify students with zero attendance as low-attendance
    bool foundS1CS301 = false;
    for (const auto& row : attReport) {
        if (row.rollNo == "22CS101" && row.courseCode == "CS301") {
            foundS1CS301 = true;
        }
    }
    check("Attendance report skips s1-CS301 (0 records)", !foundS1CS301);

    // 13. Excludes inactive students
    bool foundInactiveAtt = false;
    for (const auto& row : attReport) {
        if (row.rollNo == "22CS103") {
            foundInactiveAtt = true;
        }
    }
    check("Attendance report excludes inactive student", !foundInactiveAtt);

    // --- Enrollment Report tests ---
    std::cout << "\n  --- Enrollment Report ---" << std::endl;
    auto enrollReport = rm.getEnrollmentReport(t.cm, t.em);

    // 14. Correct active enrollment counts
    bool foundCS201 = false, foundCS301 = false;
    for (const auto& row : enrollReport) {
        if (row.courseCode == "CS201") {
            foundCS201 = true;
            check("Enrollment CS201: 2 enrolled", row.enrolledStudents == 2);
            check("Enrollment CS201: capacity=60", row.capacity == 60);
            check("Enrollment CS201: faculty=FAC001", row.facultyId == "FAC001");
        }
        if (row.courseCode == "CS301") {
            foundCS301 = true;
            check("Enrollment CS301: 1 enrolled", row.enrolledStudents == 1);
        }
    }
    check("Enrollment report contains CS201", foundCS201);
    check("Enrollment report contains CS301", foundCS301);

    // 15. Excludes inactive courses
    bool foundInactiveCourse = false;
    for (const auto& row : enrollReport) {
        if (row.courseCode == "CS401") {
            foundInactiveCourse = true;
        }
    }
    check("Enrollment report excludes inactive course CS401", !foundInactiveCourse);
    check("Enrollment report has 2 rows (active courses only)",
          static_cast<int>(enrollReport.size()) == 2);
}

// ---------------------------------------------------------------------------
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  FINANCE & REPORTS SMOKE TEST" << std::endl;
    std::cout << "========================================" << std::endl;

    testFeeManager();
    testReportManager();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  RESULTS: " << testsPassed << " passed, " << testsFailed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return testsFailed > 0 ? 1 : 0;
}
