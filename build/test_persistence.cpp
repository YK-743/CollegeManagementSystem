// Temporary persistence test for DataManager.
// Uses build/testdata/ as storage to avoid overwriting actual data/.

#include "college/Models.h"
#include "college/Managers.h"
#include "college/DataManager.h"
#include <iostream>
#include <fstream>
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

// Helper: write a raw file for malformed-data tests
void writeRawFile(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
    f.close();
}

// ============================================================================
// Test A — Student persistence
// ============================================================================
void testStudentPersistence() {
    std::cout << "\n=== Test A: Student Persistence ===" << std::endl;
    college::DataManager dm("build/testdata");

    // Create and populate
    college::StudentManager sm;
    sm.addStudent(college::Student("22CS101", "Ravi Kumar", "ravi@c.edu", "9876543210",
                                   "Computer Science", 4, 75000.0));
    sm.addStudent(college::Student("22CS102", "Priya Singh", "priya@c.edu", "9876543211",
                                   "Electronics", 3, 60000.0));
    sm.addStudent(college::Student("22CS103", "Inactive Student", "inactive@c.edu", "0000000000",
                                   "Maths", 1, 50000.0));
    sm.removeStudent("22CS103"); // deactivate

    // Save
    check("Save students", dm.saveStudents(sm));

    // Load into new manager
    college::StudentManager loaded;
    check("Load students", dm.loadStudents(loaded));

    // Verify
    check("Loaded 3 students total", static_cast<int>(loaded.getAllStudents().size()) == 3);
    check("Active count is 2", loaded.getTotalStudents() == 2);

    const college::Student* s1 = loaded.findStudent("22CS101");
    check("s1 found", s1 != nullptr);
    check("s1 name with spaces preserved", s1 != nullptr && s1->getName() == "Ravi Kumar");
    check("s1 email correct", s1 != nullptr && s1->getEmail() == "ravi@c.edu");
    check("s1 department correct", s1 != nullptr && s1->getDepartment() == "Computer Science");
    check("s1 semester correct", s1 != nullptr && s1->getSemester() == 4);
    check("s1 totalFee correct", s1 != nullptr && approxEqual(s1->getTotalFee(), 75000.0));
    check("s1 is active", s1 != nullptr && s1->isActive());

    const college::Student* s3 = loaded.findStudent("22CS103");
    check("s3 (inactive) found", s3 != nullptr);
    check("s3 is inactive", s3 != nullptr && !s3->isActive());
}

// ============================================================================
// Test B — Faculty persistence
// ============================================================================
void testFacultyPersistence() {
    std::cout << "\n=== Test B: Faculty Persistence ===" << std::endl;
    college::DataManager dm("build/testdata");

    college::FacultyManager fm;
    fm.addFaculty(college::Faculty("FAC001", "Dr. Sharma", "sharma@c.edu", "9988776655",
                                   "Computer Science", "Professor"));
    fm.addFaculty(college::Faculty("FAC002", "Dr. Gupta", "gupta@c.edu", "9988776656",
                                   "Electronics", "Associate Professor"));
    fm.removeFaculty("FAC002"); // deactivate

    check("Save faculty", dm.saveFaculty(fm));

    college::FacultyManager loaded;
    check("Load faculty", dm.loadFaculty(loaded));

    check("Loaded 2 faculty total", static_cast<int>(loaded.getAllFaculty().size()) == 2);
    check("Active count is 1", loaded.getTotalFaculty() == 1);

    const college::Faculty* f1 = loaded.findFaculty("FAC001");
    check("f1 found", f1 != nullptr);
    check("f1 name correct", f1 != nullptr && f1->getName() == "Dr. Sharma");
    check("f1 designation correct", f1 != nullptr && f1->getDesignation() == "Professor");
    check("f1 is active", f1 != nullptr && f1->isActive());

    const college::Faculty* f2 = loaded.findFaculty("FAC002");
    check("f2 (inactive) found", f2 != nullptr);
    check("f2 is inactive", f2 != nullptr && !f2->isActive());
}

// ============================================================================
// Test C — Course persistence
// ============================================================================
void testCoursePersistence() {
    std::cout << "\n=== Test C: Course Persistence ===" << std::endl;
    college::DataManager dm("build/testdata");

    college::CourseManager cm;
    cm.addCourse(college::Course("CS201", "Data Structures", 4, 60, "FAC001"));
    cm.addCourse(college::Course("CS301", "Algorithms", 3, 50, ""));  // empty faculty
    cm.addCourse(college::Course("CS401", "Inactive Course", 3, 40, "FAC002"));
    cm.removeCourse("CS401"); // deactivate

    check("Save courses", dm.saveCourses(cm));

    college::CourseManager loaded;
    check("Load courses", dm.loadCourses(loaded));

    check("Loaded 3 courses total", static_cast<int>(loaded.getAllCourses().size()) == 3);
    check("Active count is 2", loaded.getTotalCourses() == 2);

    const college::Course* c1 = loaded.findCourse("CS201");
    check("c1 found", c1 != nullptr);
    check("c1 name correct", c1 != nullptr && c1->getCourseName() == "Data Structures");
    check("c1 credits correct", c1 != nullptr && c1->getCredits() == 4);
    check("c1 capacity correct", c1 != nullptr && c1->getCapacity() == 60);
    check("c1 faculty correct", c1 != nullptr && c1->getFacultyId() == "FAC001");

    const college::Course* c2 = loaded.findCourse("CS301");
    check("c2 empty faculty preserved", c2 != nullptr && c2->getFacultyId().empty());

    const college::Course* c3 = loaded.findCourse("CS401");
    check("c3 (inactive) found", c3 != nullptr);
    check("c3 is inactive", c3 != nullptr && !c3->isActive());
}

// ============================================================================
// Test D — Enrollment persistence
// ============================================================================
void testEnrollmentPersistence() {
    std::cout << "\n=== Test D: Enrollment Persistence ===" << std::endl;
    college::DataManager dm("build/testdata");

    // Set up managers for enrolling
    college::StudentManager sm;
    college::CourseManager cm;
    sm.addStudent(college::Student("22CS101", "Ravi", "r@c.edu", "1234567890",
                                   "CS", 4, 75000.0));
    cm.addCourse(college::Course("CS201", "DS", 4, 60, ""));

    college::EnrollmentManager em;
    em.enrollStudent("22CS101", "CS201", "2026-08-01", sm, cm);
    em.dropCourse("22CS101", "CS201"); // drop first enrollment
    em.enrollStudent("22CS101", "CS201", "2026-09-01", sm, cm); // re-enrol

    check("Save enrollments", dm.saveEnrollments(em));

    college::EnrollmentManager loaded;
    check("Load enrollments", dm.loadEnrollments(loaded));

    const auto& all = loaded.getAllEnrollments();
    check("Loaded 2 enrollment records", static_cast<int>(all.size()) == 2);
    check("First record is dropped", !all[0].isActive());
    check("Second record is active", all[1].isActive());
    check("First record date correct", all[0].getEnrollmentDate() == "2026-08-01");
    check("Second record date correct", all[1].getEnrollmentDate() == "2026-09-01");
}

// ============================================================================
// Test E — Attendance persistence
// ============================================================================
void testAttendancePersistence() {
    std::cout << "\n=== Test E: Attendance Persistence ===" << std::endl;
    college::DataManager dm("build/testdata");

    // Set up for marking
    college::StudentManager sm;
    college::CourseManager cm;
    college::EnrollmentManager em;
    sm.addStudent(college::Student("22CS101", "Ravi", "r@c.edu", "1234567890",
                                   "CS", 4, 75000.0));
    cm.addCourse(college::Course("CS201", "DS", 4, 60, ""));
    em.enrollStudent("22CS101", "CS201", "2026-08-01", sm, cm);

    college::AttendanceManager am;
    am.markAttendance("22CS101", "CS201", "2026-09-01", true,  sm, cm, em);
    am.markAttendance("22CS101", "CS201", "2026-09-02", true,  sm, cm, em);
    am.markAttendance("22CS101", "CS201", "2026-09-03", false, sm, cm, em);

    check("Save attendance", dm.saveAttendance(am));

    college::AttendanceManager loaded;
    check("Load attendance", dm.loadAttendance(loaded));

    const auto& all = loaded.getAllAttendanceRecords();
    check("Loaded 3 attendance records", static_cast<int>(all.size()) == 3);
    check("Record 1 is present", all[0].isPresent());
    check("Record 2 is present", all[1].isPresent());
    check("Record 3 is absent", !all[2].isPresent());
    check("Record 1 date correct", all[0].getDate() == "2026-09-01");

    // Verify percentage calculation works on loaded data
    double pct = loaded.calculatePercentage("22CS101", "CS201");
    check("Loaded percentage ~66.67%", approxEqual(pct, 66.67));
}

// ============================================================================
// Test F — Payment persistence
// ============================================================================
void testPaymentPersistence() {
    std::cout << "\n=== Test F: Payment Persistence ===" << std::endl;
    college::DataManager dm("build/testdata");

    college::StudentManager sm;
    sm.addStudent(college::Student("22CS101", "Ravi", "r@c.edu", "1234567890",
                                   "CS", 4, 75000.0));

    college::FeeManager feeM;
    feeM.recordPayment(college::FeePayment("REC001", "22CS101", 25000.0, "2026-07-15", "UPI"), sm);
    feeM.recordPayment(college::FeePayment("REC002", "22CS101", 20000.0, "2026-08-15", "Card"), sm);
    feeM.recordPayment(college::FeePayment("REC003", "22CS101", 10000.0, "2026-09-15", "Cash"), sm);

    check("Save payments", dm.savePayments(feeM));

    college::FeeManager loaded;
    check("Load payments", dm.loadPayments(loaded));

    const auto& all = loaded.getAllPayments();
    check("Loaded 3 payment records", static_cast<int>(all.size()) == 3);
    check("REC001 amount correct", approxEqual(all[0].getAmount(), 25000.0));
    check("REC002 amount correct", approxEqual(all[1].getAmount(), 20000.0));
    check("REC003 amount correct", approxEqual(all[2].getAmount(), 10000.0));
    check("REC001 receipt no correct", all[0].getReceiptNo() == "REC001");
    check("REC001 payment mode correct", all[0].getPaymentMode() == "UPI");
    check("REC002 payment date correct", all[1].getPaymentDate() == "2026-08-15");

    // Verify total collection on loaded data
    check("Loaded total collection = 55000", approxEqual(loaded.getTotalCollection(), 55000.0));
}

// ============================================================================
// Test G — Complete round-trip
// ============================================================================
void testCompleteRoundTrip() {
    std::cout << "\n=== Test G: Complete Round-Trip ===" << std::endl;
    college::DataManager dm("build/testdata");

    // Populate all managers
    college::StudentManager sm;
    college::FacultyManager fm;
    college::CourseManager cm;
    college::EnrollmentManager em;
    college::AttendanceManager am;
    college::FeeManager feeM;

    sm.addStudent(college::Student("22CS101", "Ravi Kumar", "ravi@c.edu", "9876543210",
                                   "CS", 4, 75000.0));
    sm.addStudent(college::Student("22CS102", "Priya Singh", "priya@c.edu", "9876543211",
                                   "EC", 3, 60000.0));
    sm.addStudent(college::Student("22CS103", "Dropped Student", "drop@c.edu", "0000000000",
                                   "ME", 1, 50000.0));
    sm.removeStudent("22CS103");

    fm.addFaculty(college::Faculty("FAC001", "Dr. Sharma", "sharma@c.edu", "9988776655",
                                   "CS", "Professor"));

    cm.addCourse(college::Course("CS201", "Data Structures", 4, 60, "FAC001"));
    cm.addCourse(college::Course("CS301", "Algorithms", 3, 50, ""));
    cm.addCourse(college::Course("CS401", "Old Course", 2, 30, ""));
    cm.removeCourse("CS401");

    em.enrollStudent("22CS101", "CS201", "2026-08-01", sm, cm);
    em.enrollStudent("22CS102", "CS201", "2026-08-01", sm, cm);
    em.enrollStudent("22CS101", "CS301", "2026-08-01", sm, cm);
    em.dropCourse("22CS101", "CS301"); // drop one enrollment

    am.markAttendance("22CS101", "CS201", "2026-09-01", true,  sm, cm, em);
    am.markAttendance("22CS101", "CS201", "2026-09-02", true,  sm, cm, em);
    am.markAttendance("22CS101", "CS201", "2026-09-03", true,  sm, cm, em);
    am.markAttendance("22CS101", "CS201", "2026-09-04", true,  sm, cm, em);
    am.markAttendance("22CS101", "CS201", "2026-09-05", false, sm, cm, em);

    feeM.recordPayment(college::FeePayment("REC001", "22CS101", 25000.0, "2026-07-15", "UPI"), sm);
    feeM.recordPayment(college::FeePayment("REC002", "22CS101", 20000.0, "2026-08-15", "Card"), sm);
    feeM.recordPayment(college::FeePayment("REC003", "22CS102", 15000.0, "2026-07-20", "Cash"), sm);

    // Save all
    check("saveAllData succeeds", dm.saveAllData(sm, fm, cm, em, am, feeM));

    // Load into fresh managers
    college::StudentManager sm2;
    college::FacultyManager fm2;
    college::CourseManager cm2;
    college::EnrollmentManager em2;
    college::AttendanceManager am2;
    college::FeeManager feeM2;

    check("loadAllData succeeds", dm.loadAllData(sm2, fm2, cm2, em2, am2, feeM2));

    // Compare collection sizes
    check("Students: 3 total", static_cast<int>(sm2.getAllStudents().size()) == 3);
    check("Students: 2 active", sm2.getTotalStudents() == 2);
    check("Faculty: 1 total", static_cast<int>(fm2.getAllFaculty().size()) == 1);
    check("Courses: 3 total", static_cast<int>(cm2.getAllCourses().size()) == 3);
    check("Courses: 2 active", cm2.getTotalCourses() == 2);
    check("Enrollments: 3 total", static_cast<int>(em2.getAllEnrollments().size()) == 3);
    check("Attendance: 5 total", static_cast<int>(am2.getAllAttendanceRecords().size()) == 5);
    check("Payments: 3 total", static_cast<int>(feeM2.getAllPayments().size()) == 3);

    // Verify inactive statuses
    const college::Student* s3 = sm2.findStudent("22CS103");
    check("Inactive student preserved", s3 != nullptr && !s3->isActive());

    const college::Course* c3 = cm2.findCourse("CS401");
    check("Inactive course preserved", c3 != nullptr && !c3->isActive());

    // Verify dropped enrollment preserved
    int droppedCount = 0;
    int activeEnrollCount = 0;
    for (const auto& e : em2.getAllEnrollments()) {
        if (!e.isActive()) ++droppedCount;
        else ++activeEnrollCount;
    }
    check("Dropped enrollments preserved (1 dropped)", droppedCount == 1);
    check("Active enrollments correct (2 active)", activeEnrollCount == 2);

    // Verify FeeManager calculations on loaded data
    check("Total collection = 60000", approxEqual(feeM2.getTotalCollection(), 60000.0));

    // Verify AttendanceManager percentage on loaded data
    double pct = am2.calculatePercentage("22CS101", "CS201");
    check("Attendance percentage = 80%", approxEqual(pct, 80.0));
    check("Total classes = 5", am2.getTotalClasses("22CS101", "CS201") == 5);
}

// ============================================================================
// Test H — First launch (no files exist)
// ============================================================================
void testFirstLaunch() {
    std::cout << "\n=== Test H: First Launch (No Files) ===" << std::endl;
    // Use a directory with no data files
    college::DataManager dm("build/testdata_empty");

    college::StudentManager sm;
    college::FacultyManager fm;
    college::CourseManager cm;
    college::EnrollmentManager em;
    college::AttendanceManager am;
    college::FeeManager feeM;

    check("loadAllData on empty directory succeeds",
          dm.loadAllData(sm, fm, cm, em, am, feeM));

    check("Students empty", sm.getAllStudents().empty());
    check("Faculty empty", fm.getAllFaculty().empty());
    check("Courses empty", cm.getAllCourses().empty());
    check("Enrollments empty", em.getAllEnrollments().empty());
    check("Attendance empty", am.getAllAttendanceRecords().empty());
    check("Payments empty", feeM.getAllPayments().empty());
}

// ============================================================================
// Test I — Malformed data
// ============================================================================
void testMalformedData() {
    std::cout << "\n=== Test I: Malformed Data ===" << std::endl;
    college::DataManager dm("build/testdata_bad");

    // Pre-populate a manager with valid data
    college::StudentManager sm;
    sm.addStudent(college::Student("22CS101", "Ravi", "r@c.edu", "1234567890",
                                   "CS", 4, 75000.0));

    // Write a malformed students.txt (wrong field count)
    writeRawFile("build/testdata_bad/students.txt",
                 "22CS101|Ravi|r@c.edu\n");

    // Loading should fail
    check("Load malformed students returns false", !dm.loadStudents(sm));

    // Original data should NOT have been replaced
    check("Original student data preserved after failed load",
          static_cast<int>(sm.getAllStudents().size()) == 1);
    check("Original student still correct",
          sm.findStudent("22CS101") != nullptr &&
          sm.findStudent("22CS101")->getName() == "Ravi");

    // Test invalid numeric value
    writeRawFile("build/testdata_bad/students.txt",
                 "22CS101|Ravi|r@c.edu|1234567890|CS|abc|75000|1\n");
    check("Load students with invalid semester returns false", !dm.loadStudents(sm));

    // Test invalid boolean
    writeRawFile("build/testdata_bad/students.txt",
                 "22CS101|Ravi|r@c.edu|1234567890|CS|4|75000|2\n");
    check("Load students with invalid active flag returns false", !dm.loadStudents(sm));
}

// ============================================================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  PERSISTENCE SMOKE TEST" << std::endl;
    std::cout << "========================================" << std::endl;

    testStudentPersistence();
    testFacultyPersistence();
    testCoursePersistence();
    testEnrollmentPersistence();
    testAttendancePersistence();
    testPaymentPersistence();
    testCompleteRoundTrip();
    testFirstLaunch();
    testMalformedData();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  RESULTS: " << testsPassed << " passed, " << testsFailed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return testsFailed > 0 ? 1 : 0;
}
