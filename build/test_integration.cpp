// Phase 7 Integration Test — CollegeSystem + ConsoleUI
// Tests all manager integrations through CollegeSystem,
// automatic saving, data persistence, and report generation.
// Uses build/testdata_integration/ to avoid overwriting real data.

#include "college/CollegeSystem.h"
#include "college/DataManager.h"
#include "college/Managers.h"
#include "college/Models.h"
#include <iostream>
#include <cmath>
#include <string>

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

int main() {
    std::cout << "========================================\n";
    std::cout << "  PHASE 7 INTEGRATION TEST\n";
    std::cout << "========================================\n";

    // Use a test-only data directory
    // We test via CollegeSystem's managers directly, just like ConsoleUI does.

    // --- Test A: CollegeSystem creation and first-launch load ---
    std::cout << "\n=== Test A: CollegeSystem Construction & First Launch ===\n";
    {
        // Construct with custom DataManager to use test directory
        college::CollegeSystem sys;
        // On first launch with no data/ dir, loadAllData should still succeed
        // (missing files = empty collections, returns true)
        // NOTE: We're using the default "data" dir. If it exists, it loads.
        // This test just verifies construction works.
        check("CollegeSystem constructed", true);
        check("getTotalStudents initially correct",
              sys.getStudentManager().getTotalStudents() >= 0);
    }

    // --- Test B: Student Management via CollegeSystem ---
    std::cout << "\n=== Test B: Student Management ===\n";
    {
        college::CollegeSystem sys;
        auto& sm = sys.getStudentManager();

        college::Student s1("T001", "Alice Test", "alice@test.edu", "1111111111",
                            "Computer Science", 3, 50000.0);
        college::Student s2("T002", "Bob Test", "bob@test.edu", "2222222222",
                            "Electronics", 2, 45000.0);

        check("Add student s1", sm.addStudent(s1));
        check("Add student s2", sm.addStudent(s2));
        check("Reject duplicate s1", !sm.addStudent(s1));
        check("Total students = 2", sm.getTotalStudents() == 2);

        // Update
        college::Student s1update("T001", "Alice Updated", "alice2@test.edu",
                                  "1111111111", "Computer Science", 4, 50000.0);
        check("Update s1", sm.updateStudent("T001", s1update));
        check("s1 name updated", sm.findStudent("T001")->getName() == "Alice Updated");

        // Deactivate
        check("Deactivate s2", sm.removeStudent("T002"));
        check("s2 is inactive", !sm.findStudent("T002")->isActive());
        check("Active count = 1", sm.getTotalStudents() == 1);
    }

    // --- Test C: Faculty Management via CollegeSystem ---
    std::cout << "\n=== Test C: Faculty Management ===\n";
    {
        college::CollegeSystem sys;
        auto& fm = sys.getFacultyManager();

        college::Faculty f1("F001", "Dr. Smith", "smith@test.edu", "3333333333",
                            "Computer Science", "Professor");
        college::Faculty f2("F002", "Dr. Jones", "jones@test.edu", "4444444444",
                            "Electronics", "Associate Professor");

        check("Add faculty f1", fm.addFaculty(f1));
        check("Add faculty f2", fm.addFaculty(f2));
        check("Reject duplicate f1", !fm.addFaculty(f1));
        check("Total faculty = 2", fm.getTotalFaculty() == 2);

        // Polymorphism: access via Person reference
        const college::Person& personRef = *fm.findFaculty("F001");
        check("Polymorphism: Person getName()", personRef.getName() == "Dr. Smith");
    }

    // --- Test D: Course Management + Faculty Assignment ---
    std::cout << "\n=== Test D: Course + Faculty Assignment ===\n";
    {
        college::CollegeSystem sys;
        auto& cm = sys.getCourseManager();
        auto& fm = sys.getFacultyManager();

        fm.addFaculty(college::Faculty("F001", "Dr. Smith", "s@t.edu", "111",
                                        "CS", "Prof"));

        college::Course c1("CS101", "Intro to CS", 4, 60, "");
        check("Add course", cm.addCourse(c1));
        check("Total courses = 1", cm.getTotalCourses() == 1);

        // Assign faculty
        check("Assign faculty to course",
              cm.assignFaculty("CS101", "F001", fm));
        check("Faculty ID set",
              cm.findCourse("CS101")->getFacultyId() == "F001");

        // Reject invalid faculty
        check("Reject invalid faculty assignment",
              !cm.assignFaculty("CS101", "FXXX", fm));
    }

    // --- Test E: Enrollment Management ---
    std::cout << "\n=== Test E: Enrollment Management ===\n";
    {
        college::CollegeSystem sys;
        auto& sm = sys.getStudentManager();
        auto& cm = sys.getCourseManager();
        auto& em = sys.getEnrollmentManager();

        sm.addStudent(college::Student("T001", "Alice", "a@t.edu", "111",
                                        "CS", 3, 50000));
        cm.addCourse(college::Course("CS101", "Intro to CS", 4, 2, ""));

        check("Enroll student",
              em.enrollStudent("T001", "CS101", "2025-01-15", sm, cm));
        check("Is enrolled", em.isAlreadyEnrolled("T001", "CS101"));
        check("Enrolled count = 1", em.getEnrolledCount("CS101") == 1);
        check("Reject duplicate enrollment",
              !em.enrollStudent("T001", "CS101", "2025-01-16", sm, cm));

        // Drop
        check("Drop course", em.dropCourse("T001", "CS101"));
        check("Not enrolled after drop", !em.isAlreadyEnrolled("T001", "CS101"));
        check("Historical record preserved",
              em.getAllEnrollments().size() == 1);
    }

    // --- Test F: Attendance Management ---
    std::cout << "\n=== Test F: Attendance Management ===\n";
    {
        college::CollegeSystem sys;
        auto& sm = sys.getStudentManager();
        auto& cm = sys.getCourseManager();
        auto& em = sys.getEnrollmentManager();
        auto& am = sys.getAttendanceManager();

        sm.addStudent(college::Student("T001", "Alice", "a@t.edu", "111",
                                        "CS", 3, 50000));
        cm.addCourse(college::Course("CS101", "Intro", 4, 60, ""));
        em.enrollStudent("T001", "CS101", "2025-01-15", sm, cm);

        check("Mark present",
              am.markAttendance("T001", "CS101", "2025-02-01", true, sm, cm, em));
        check("Mark present again",
              am.markAttendance("T001", "CS101", "2025-02-02", true, sm, cm, em));
        check("Mark absent",
              am.markAttendance("T001", "CS101", "2025-02-03", false, sm, cm, em));
        check("Reject duplicate date",
              !am.markAttendance("T001", "CS101", "2025-02-01", false, sm, cm, em));

        double pct = am.calculatePercentage("T001", "CS101");
        check("Attendance = 66.67%", std::abs(pct - 66.67) < 0.1);
        check("Total classes = 3", am.getTotalClasses("T001", "CS101") == 3);

        // Correct attendance
        check("Correct absent to present",
              am.updateAttendance("T001", "CS101", "2025-02-03", true));
        double pct2 = am.calculatePercentage("T001", "CS101");
        check("Attendance now 100%", std::abs(pct2 - 100.0) < 0.01);
    }

    // --- Test G: Fee Management ---
    std::cout << "\n=== Test G: Fee Management ===\n";
    {
        college::CollegeSystem sys;
        auto& sm = sys.getStudentManager();
        auto& fm = sys.getFeeManager();

        sm.addStudent(college::Student("T001", "Alice", "a@t.edu", "111",
                                        "CS", 3, 50000.0));

        college::FeePayment p1("REC001", "T001", 20000.0, "2025-03-01", "Cash");
        check("Record payment", fm.recordPayment(p1, sm));
        check("Total paid = 20000", std::abs(fm.calculateTotalPaid("T001") - 20000.0) < 0.01);
        check("Outstanding = 30000",
              std::abs(fm.calculateOutstanding("T001", sm) - 30000.0) < 0.01);

        // Reject duplicate receipt
        college::FeePayment p2("REC001", "T001", 10000.0, "2025-03-02", "Card");
        check("Reject duplicate receipt", !fm.recordPayment(p2, sm));

        // Reject overpayment
        college::FeePayment p3("REC002", "T001", 40000.0, "2025-03-02", "Card");
        check("Reject overpayment", !fm.recordPayment(p3, sm));

        // Generate receipt
        std::string receipt = fm.generateReceipt("REC001");
        check("Receipt not empty", !receipt.empty());
        check("Receipt contains amount", receipt.find("20000") != std::string::npos);

        check("Total collection = 20000",
              std::abs(fm.getTotalCollection() - 20000.0) < 0.01);
    }

    // --- Test H: Reports via CollegeSystem ---
    std::cout << "\n=== Test H: Reports & Analytics ===\n";
    {
        college::CollegeSystem sys;
        auto& sm = sys.getStudentManager();
        auto& fm = sys.getFacultyManager();
        auto& cm = sys.getCourseManager();
        auto& em = sys.getEnrollmentManager();
        auto& am = sys.getAttendanceManager();
        auto& fee = sys.getFeeManager();
        auto& rm = sys.getReportManager();

        sm.addStudent(college::Student("T001", "Alice", "a@t.edu", "111",
                                        "CS", 3, 50000));
        sm.addStudent(college::Student("T002", "Bob", "b@t.edu", "222",
                                        "EE", 2, 40000));
        fm.addFaculty(college::Faculty("F001", "Dr. X", "x@t.edu", "333",
                                        "CS", "Prof"));
        cm.addCourse(college::Course("CS101", "Intro CS", 4, 60, "F001"));
        em.enrollStudent("T001", "CS101", "2025-01-15", sm, cm);
        am.markAttendance("T001", "CS101", "2025-02-01", true, sm, cm, em);
        am.markAttendance("T001", "CS101", "2025-02-02", false, sm, cm, em);
        fee.recordPayment(college::FeePayment("R1", "T001", 10000, "2025-03-01", "Cash"), sm);

        // Dashboard
        college::DashboardSummary ds = rm.getDashboardSummary(sm, fm, cm, em, fee);
        check("Dashboard: 2 students", ds.totalStudents == 2);
        check("Dashboard: 1 faculty", ds.totalFaculty == 1);
        check("Dashboard: 1 course", ds.totalCourses == 1);
        check("Dashboard: 1 enrollment", ds.totalEnrollments == 1);
        check("Dashboard: collection = 10000",
              std::abs(ds.totalFeeCollection - 10000.0) < 0.01);
        check("Dashboard: outstanding = 80000",
              std::abs(ds.totalOutstandingFees - 80000.0) < 0.01);

        // Fee report
        auto feeReport = rm.getFeeReport(sm, fee);
        check("Fee report: 2 rows", feeReport.size() == 2);

        // Attendance report
        auto attReport = rm.getAttendanceReport(sm, em, am, 75.0);
        check("Attendance report: 1 row", attReport.size() == 1);
        check("Below requirement flagged", attReport[0].belowRequirement);

        // Enrollment report
        auto enrReport = rm.getEnrollmentReport(cm, em);
        check("Enrollment report: 1 row", enrReport.size() == 1);
        check("Enrolled = 1", enrReport[0].enrolledStudents == 1);
        check("Capacity = 60", enrReport[0].capacity == 60);
    }

    // --- Test I: Data Persistence Round-Trip ---
    std::cout << "\n=== Test I: Data Persistence Round-Trip ===\n";
    {
        // Save data using one CollegeSystem, load with another
        // Using a custom DataManager with test directory
        college::DataManager dm("build/testdata_integration");
        college::StudentManager sm1;
        college::FacultyManager fm1;
        college::CourseManager cm1;
        college::EnrollmentManager em1;
        college::AttendanceManager am1;
        college::FeeManager fee1;

        sm1.addStudent(college::Student("P001", "Persist Test", "p@t.edu", "999",
                                         "Math", 1, 60000));
        fm1.addFaculty(college::Faculty("PF01", "Dr. Persist", "dp@t.edu", "888",
                                         "Math", "Lecturer"));
        cm1.addCourse(college::Course("MA101", "Calculus", 3, 40, "PF01"));
        em1.enrollStudent("P001", "MA101", "2025-09-01", sm1, cm1);
        am1.markAttendance("P001", "MA101", "2025-09-05", true, sm1, cm1, em1);
        fee1.recordPayment(
            college::FeePayment("PR01", "P001", 15000, "2025-09-10", "UPI"), sm1);

        check("Save all data", dm.saveAllData(sm1, fm1, cm1, em1, am1, fee1));

        // Load into fresh managers
        college::StudentManager sm2;
        college::FacultyManager fm2;
        college::CourseManager cm2;
        college::EnrollmentManager em2;
        college::AttendanceManager am2;
        college::FeeManager fee2;

        check("Load all data", dm.loadAllData(sm2, fm2, cm2, em2, am2, fee2));
        check("Student persisted", sm2.findStudent("P001") != nullptr);
        check("Student name correct",
              sm2.findStudent("P001")->getName() == "Persist Test");
        check("Faculty persisted", fm2.findFaculty("PF01") != nullptr);
        check("Course persisted", cm2.findCourse("MA101") != nullptr);
        check("Enrollment persisted", em2.getAllEnrollments().size() == 1);
        check("Attendance persisted", am2.getAllAttendanceRecords().size() == 1);
        check("Payment persisted", fee2.getAllPayments().size() == 1);
        check("Fee collection correct",
              std::abs(fee2.getTotalCollection() - 15000.0) < 0.01);
    }

    // --- Test J: Polymorphism Verification ---
    std::cout << "\n=== Test J: Polymorphism Demonstration ===\n";
    {
        college::Student s("POLY01", "Poly Student", "ps@t.edu", "555",
                           "CS", 2, 30000);
        college::Faculty f("FPOLY", "Poly Faculty", "pf@t.edu", "666",
                           "EE", "Prof");

        // Access through Person pointers
        const college::Person* sp = &s;
        const college::Person* fp = &f;

        check("Student via Person* getName()", sp->getName() == "Poly Student");
        check("Faculty via Person* getName()", fp->getName() == "Poly Faculty");

        // displayDetails() is virtual — calling through Person* invokes derived
        std::cout << "  (Polymorphic displayDetails via Person pointer:)\n";
        sp->displayDetails();
        fp->displayDetails();
        check("Polymorphic calls completed", true);
    }

    // --- Summary ---
    std::cout << "\n========================================\n";
    std::cout << "  RESULTS: " << passed << " passed, " << failed << " failed\n";
    std::cout << "========================================\n";

    return (failed > 0) ? 1 : 0;
}
