// Temporary smoke test for StudentManager, FacultyManager, and CourseManager.
// This file lives under build/ and is not part of the main application.

#include "college/Models.h"
#include "college/Managers.h"
#include <iostream>
#include <string>

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

void testStudentManager() {
    std::cout << "\n=== StudentManager Tests ===" << std::endl;
    college::StudentManager sm;

    // 1. Add a student
    college::Student s1("22CS101", "Ravi Kumar", "ravi@college.edu", "9876543210",
                        "Computer Science", 4, 75000.0);
    check("Add student s1", sm.addStudent(s1));

    // 2. Add another student
    college::Student s2("22CS102", "Priya Singh", "priya@college.edu", "9876543211",
                        "Electronics", 3, 70000.0);
    check("Add student s2", sm.addStudent(s2));

    // 3. Reject duplicate roll number
    college::Student s1dup("22CS101", "Duplicate Name", "dup@college.edu", "0000000000",
                           "Maths", 1, 50000.0);
    check("Reject duplicate roll number", !sm.addStudent(s1dup));

    // 4. Find an existing student
    const college::Student* found = sm.findStudent("22CS101");
    check("Find existing student 22CS101", found != nullptr);
    check("Found student has correct name", found != nullptr && found->getName() == "Ravi Kumar");

    // 5. Find a nonexistent student
    check("Find nonexistent student returns nullptr", sm.findStudent("99XX999") == nullptr);

    // 6. Update student details
    college::Student s1updated("22CS101", "Ravi K. Sharma", "ravi.sharma@college.edu",
                               "9876500000", "Information Technology", 5, 75000.0);
    check("Update student details", sm.updateStudent("22CS101", s1updated));
    found = sm.findStudent("22CS101");
    check("Name updated correctly", found != nullptr && found->getName() == "Ravi K. Sharma");
    check("Email updated correctly", found != nullptr && found->getEmail() == "ravi.sharma@college.edu");
    check("Department updated correctly", found != nullptr && found->getDepartment() == "Information Technology");
    check("Semester updated correctly", found != nullptr && found->getSemester() == 5);

    // 7. Verify active count before deactivation
    check("Active count is 2 before removal", sm.getTotalStudents() == 2);

    // 8. Deactivate a student
    check("Remove (deactivate) student 22CS101", sm.removeStudent("22CS101"));

    // 9. Verify deactivated student not in active count
    check("Active count is 1 after removal", sm.getTotalStudents() == 1);

    // 10. Verify record is still preserved
    found = sm.findStudent("22CS101");
    check("Deactivated record still findable", found != nullptr);
    check("Deactivated record is inactive", found != nullptr && !found->isActive());

    // 11. Reject updating an inactive student
    check("Reject update on inactive student", !sm.updateStudent("22CS101", s1updated));

    // 12. Reject removing an already-inactive student
    check("Reject removing already-inactive student", !sm.removeStudent("22CS101"));

    // 13. getAllStudents returns all records
    check("getAllStudents has 2 records (including inactive)", sm.getAllStudents().size() == 2);
}

void testFacultyManager() {
    std::cout << "\n=== FacultyManager Tests ===" << std::endl;
    college::FacultyManager fm;

    // 1. Add faculty members
    college::Faculty f1("FAC001", "Dr. Sharma", "sharma@college.edu", "9988776655",
                        "Computer Science", "Professor");
    check("Add faculty f1", fm.addFaculty(f1));

    college::Faculty f2("FAC002", "Dr. Gupta", "gupta@college.edu", "9988776656",
                        "Electronics", "Associate Professor");
    check("Add faculty f2", fm.addFaculty(f2));

    // 2. Reject duplicate faculty ID
    college::Faculty f1dup("FAC001", "Duplicate", "dup@college.edu", "0000000000",
                           "Maths", "Lecturer");
    check("Reject duplicate faculty ID", !fm.addFaculty(f1dup));

    // 3. Find faculty
    const college::Faculty* found = fm.findFaculty("FAC001");
    check("Find existing faculty FAC001", found != nullptr);
    check("Found faculty has correct name", found != nullptr && found->getName() == "Dr. Sharma");

    check("Find nonexistent faculty returns nullptr", fm.findFaculty("FAC999") == nullptr);

    // 4. Update faculty details
    college::Faculty f1updated("FAC001", "Dr. A. Sharma", "a.sharma@college.edu",
                               "9988770000", "Information Technology", "HOD");
    check("Update faculty details", fm.updateFaculty("FAC001", f1updated));
    found = fm.findFaculty("FAC001");
    check("Faculty name updated", found != nullptr && found->getName() == "Dr. A. Sharma");
    check("Faculty designation updated", found != nullptr && found->getDesignation() == "HOD");

    // 5. Active count before deactivation
    check("Active count is 2 before removal", fm.getTotalFaculty() == 2);

    // 6. Deactivate faculty
    check("Remove (deactivate) faculty FAC002", fm.removeFaculty("FAC002"));

    // 7. Verify active count
    check("Active count is 1 after removal", fm.getTotalFaculty() == 1);

    // 8. Verify inactive record preservation
    found = fm.findFaculty("FAC002");
    check("Deactivated faculty record still findable", found != nullptr);
    check("Deactivated faculty is inactive", found != nullptr && !found->isActive());

    // 9. Reject updating inactive faculty
    college::Faculty f2updated("FAC002", "Updated", "upd@college.edu", "1111111111",
                               "Physics", "Lecturer");
    check("Reject update on inactive faculty", !fm.updateFaculty("FAC002", f2updated));

    // 10. getAllFaculty returns all records
    check("getAllFaculty has 2 records (including inactive)", fm.getAllFaculty().size() == 2);
}

void testCourseManager() {
    std::cout << "\n=== CourseManager Tests ===" << std::endl;
    college::CourseManager cm;

    // Set up a FacultyManager with test data for assignFaculty tests
    college::FacultyManager fm;
    college::Faculty f1("FAC001", "Dr. Sharma", "sharma@college.edu", "9988776655",
                        "Computer Science", "Professor");
    college::Faculty f2("FAC002", "Dr. Gupta", "gupta@college.edu", "9988776656",
                        "Electronics", "Associate Professor");
    fm.addFaculty(f1);
    fm.addFaculty(f2);

    // 1. Add courses
    college::Course c1("CS201", "Data Structures", 4, 60, "");
    check("Add course c1", cm.addCourse(c1));

    college::Course c2("CS301", "Algorithms", 3, 50, "");
    check("Add course c2", cm.addCourse(c2));

    // 2. Reject duplicate course codes
    college::Course c1dup("CS201", "Duplicate Course", 2, 30, "");
    check("Reject duplicate course code", !cm.addCourse(c1dup));

    // 3. Find a course
    const college::Course* found = cm.findCourse("CS201");
    check("Find existing course CS201", found != nullptr);
    check("Found course has correct name", found != nullptr && found->getCourseName() == "Data Structures");

    check("Find nonexistent course returns nullptr", cm.findCourse("XX999") == nullptr);

    // 4. Update course information
    college::Course c1updated("CS201", "Advanced Data Structures", 4, 80, "");
    check("Update course details", cm.updateCourse("CS201", c1updated));
    found = cm.findCourse("CS201");
    check("Course name updated", found != nullptr && found->getCourseName() == "Advanced Data Structures");
    check("Course capacity updated", found != nullptr && found->getCapacity() == 80);

    // 5. Assign an existing active faculty to a course
    check("Assign active faculty FAC001 to CS201", cm.assignFaculty("CS201", "FAC001", fm));
    found = cm.findCourse("CS201");
    check("Faculty assigned correctly", found != nullptr && found->getFacultyId() == "FAC001");

    // 6. Reject assigning nonexistent faculty
    check("Reject assigning nonexistent faculty", !cm.assignFaculty("CS301", "FAC999", fm));

    // 7. Reject assigning inactive faculty
    fm.removeFaculty("FAC002"); // deactivate FAC002
    check("Reject assigning inactive faculty FAC002", !cm.assignFaculty("CS301", "FAC002", fm));

    // 8. Active count before deactivation
    check("Active course count is 2 before removal", cm.getTotalCourses() == 2);

    // 9. Deactivate a course
    check("Remove (deactivate) course CS301", cm.removeCourse("CS301"));

    // 10. Verify active course count
    check("Active course count is 1 after removal", cm.getTotalCourses() == 1);

    // 11. Verify record preservation
    found = cm.findCourse("CS301");
    check("Deactivated course record still findable", found != nullptr);
    check("Deactivated course is inactive", found != nullptr && !found->isActive());

    // 12. Reject updating inactive course
    college::Course c2updated("CS301", "Updated Algo", 4, 40, "");
    check("Reject update on inactive course", !cm.updateCourse("CS301", c2updated));

    // 13. Reject assigning faculty to inactive course
    check("Reject assigning faculty to inactive course", !cm.assignFaculty("CS301", "FAC001", fm));

    // 14. getAllCourses returns all records
    check("getAllCourses has 2 records (including inactive)", cm.getAllCourses().size() == 2);
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  MANAGER CLASSES SMOKE TEST" << std::endl;
    std::cout << "========================================" << std::endl;

    testStudentManager();
    testFacultyManager();
    testCourseManager();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  RESULTS: " << testsPassed << " passed, " << testsFailed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return testsFailed > 0 ? 1 : 0;
}
