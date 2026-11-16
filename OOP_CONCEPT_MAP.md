# Object-Oriented Programming (OOP) Concept Map
## College Management System (C++17)

This document provides a comprehensive mapping between standard Object-Oriented Programming (OOP) syllabus concepts and their exact implementation in the College Management System codebase.

---

## Summary of Coverage

| Category | Concepts |
|---|---|
| **Implemented and Demonstrable** | Classes and Objects, Abstraction, Encapsulation, Inheritance, Polymorphism (Subtype Polymorphism / Late Binding), Virtual Functions, Pure Virtual Functions, Abstract Classes, Method Overriding, Parameterized Constructors, Constructor Initializer Lists, Virtual Destructors, Friend Classes, Static Member Functions, Function / Const Overloading, Exception Handling, Namespaces, File Handling (Persistence) |
| **Partially Covered** | Templates (STL Container Templates `std::vector<T>` used extensively; no custom template classes) |
| **Not Included** | Operator Overloading, Multiple / Multilevel Inheritance, Standalone Friend Functions |

---

## 1. Implemented and Demonstrable Concepts

### 1.1 Classes and Objects
* **Concept:** User-defined types bundling attributes (data) and behaviors (methods), instantiated as objects in memory.
* **Location:**
  * Models: `include/college/Models.h` (lines 14–211), `src/Models.cpp`
  * Managers: `include/college/Managers.h` (lines 18–235), `src/Managers.cpp`
  * Persistence: `include/college/DataManager.h` (lines 13–57), `src/DataManager.cpp`
  * Coordinator: `include/college/CollegeSystem.h` (lines 12–47), `src/CollegeSystem.cpp`
  * Presentation: `include/college/ConsoleUI.h` (lines 13–41), `src/ConsoleUI.cpp`
* **Implementation Details:**
  * Domain entities (`Person`, `Student`, `Faculty`, `Course`, `Enrollment`, `AttendanceRecord`, `FeePayment`) represent real-world entities.
  * Management classes (`StudentManager`, `FacultyManager`, etc.) manage object collections using `std::vector`.
  * Instantiated as objects in `CollegeSystem` and utilized throughout `ConsoleUI`.

### 1.2 Abstraction
* **Concept:** Exposing only essential interface details to the outside world while hiding internal complexity and implementation mechanisms.
* **Location:**
  * `include/college/Models.h` (lines 14–39) — `Person` abstract base class
  * `include/college/Managers.h` (lines 213–235) — `ReportManager`
  * `include/college/DataManager.h` (lines 23–57) — `DataManager`
* **Implementation Details:**
  * `Person` specifies the abstract contract `displayDetails()`. Callers interact with `Person` without knowing whether it is a `Student` or `Faculty`.
  * `ReportManager` computes complex analytics (e.g. `getDashboardSummary`, `getAttendanceReport`) and returns structured DTOs (`DashboardSummary`, `AttendanceReportRow`) hiding inner arithmetic loops.
  * `DataManager` abstracts low-level file I/O streams (`std::ifstream`, `std::ofstream`, character escaping) behind clean method calls (`saveAllData`, `loadAllData`).

### 1.3 Encapsulation
* **Concept:** Enclosing data within a class and restricting direct external access using access specifiers (`private`, `public`), exposing controlled access through methods.
* **Location:**
  * All classes in `include/college/Models.h`
  * All classes in `include/college/Managers.h`
  * `include/college/DataManager.h`
* **Implementation Details:**
  * Every attribute in every class is `private`.
  * Read access is provided through `const` getters (e.g., `getRollNo()`, `getName()`, `isActive()`).
  * Mutation is restricted through validated domain methods (e.g., `updateDetails()`, `setActive()`, `drop()`, `setPresent()`).
  * Direct array/vector indexing is encapsulated inside manager queries (`findStudent()`, `getEnrolledCount()`).

### 1.4 Inheritance
* **Concept:** Mechanism where a derived class inherits properties and behaviors from a base class, promoting reusability and hierarchical classification.
* **Location:**
  * `include/college/Models.h`:
    * Line 44: `class Student : public Person`
    * Line 76: `class Faculty : public Person`
* **Implementation Details:**
  * `Student` and `Faculty` publicly inherit common identity attributes (`id`, `name`, `email`, `phone`) and member functions from `Person`.
  * `Student` adds academic attributes (`department`, `semester`, `totalFee`, `active`).
  * `Faculty` adds professional attributes (`department`, `designation`, `active`).

### 1.5 Polymorphism and Late Binding
* **Concept:** Calling a member function through a base class pointer or reference resolves at runtime to the derived class implementation based on the actual object type.
* **Location:**
  * Declaration: `include/college/Models.h` (line 38: `virtual void displayDetails() const = 0;`)
  * Implementation: `src/Models.cpp` (lines 66–78 for `Student`, lines 115–126 for `Faculty`)
  * Invocation:
    * `src/ConsoleUI.cpp` (lines 277–280 in `studentMenu()` case 6; lines 426–428 in `facultyMenu()` case 4)
    * `build/test_integration.cpp` (lines 322–332)
* **Implementation Details:**
  * In `ConsoleUI::studentMenu()`:
    ```cpp
    const Person& personRef = *s;
    personRef.displayDetails(); // Dynamic dispatch invokes Student::displayDetails()
    ```
  * In `ConsoleUI::facultyMenu()`:
    ```cpp
    const Person& personRef = f;
    personRef.displayDetails(); // Dynamic dispatch invokes Faculty::displayDetails()
    ```
  * The vtable pointer dispatches to the correct override at runtime.

### 1.6 Pure Virtual Functions and Abstract Classes
* **Concept:** A virtual function with `= 0` that has no implementation in the base class, making the class abstract (cannot be instantiated).
* **Location:**
  * `include/college/Models.h`:
    * Line 14: `class Person`
    * Line 38: `virtual void displayDetails() const = 0;`
* **Implementation Details:**
  * `Person` cannot be instantiated directly (`Person p(...)` is a compile error).
  * Forces all derived classes (`Student`, `Faculty`) to provide concrete implementations of `displayDetails()`.

### 1.7 Method Overriding
* **Concept:** A derived class provides a specific implementation of a virtual function declared in its base class.
* **Location:**
  * `include/college/Models.h`:
    * Line 70: `void displayDetails() const override;` (in `Student`)
    * Line 100: `void displayDetails() const override;` (in `Faculty`)
  * `src/Models.cpp`:
    * Lines 66–78: `void Student::displayDetails() const override`
    * Lines 115–126: `void Faculty::displayDetails() const override`
* **Implementation Details:**
  * The `override` specifier explicitly instructs the compiler to verify that the signature matches the base virtual declaration.

### 1.8 Constructors and Member Initializer Lists
* **Concept:** Special member functions executed upon object creation to initialize member variables.
* **Location:**
  * `src/Models.cpp`:
    * Lines 7–10: `Person::Person(...) : id(id), name(name), email(email), phone(phone) {}`
    * Lines 16–22: `Student::Student(...) : Person(rollNo, name, email, phone), department(department), semester(semester), totalFee(totalFee), active(true) {}`
    * Lines 73–78: `Faculty::Faculty(...) : Person(facultyId, name, email, phone), department(department), designation(designation), active(true) {}`
    * Lines 130–135: `Course::Course(...) : courseCode(courseCode), ...`
    * Lines 167–170: `Enrollment::Enrollment(...) : studentRollNo(studentRollNo), ...`
    * Lines 188–191: `AttendanceRecord::AttendanceRecord(...) : ...`
    * Lines 206–210: `FeePayment::FeePayment(...) : ...`
  * `src/CollegeSystem.cpp` (lines 7–10)
  * `src/ConsoleUI.cpp` (line 14)
* **Implementation Details:**
  * Parameterized constructors initialize attributes cleanly using C++ member initializer lists.
  * Base class constructor delegation is demonstrated in `Student` and `Faculty` calling `Person(...)`.

### 1.9 Virtual Destructor
* **Concept:** Ensuring proper cleanup of derived class sub-objects when deleting an object through a pointer to the base class.
* **Location:**
  * Declaration: `include/college/Models.h` (line 24: `virtual ~Person();`)
  * Definition: `src/Models.cpp` (line 12: `Person::~Person() {}`)
* **Implementation Details:**
  * Declared `virtual` in abstract base `Person` to guarantee safe polymorphic destruction without undefined behavior or resource leaks.

### 1.10 Friend Classes
* **Concept:** A mechanism granting another class access to private and protected members.
* **Location:**
  * `include/college/Managers.h`:
    * Line 21: `friend class DataManager;` inside `StudentManager`
    * Line 42: `friend class DataManager;` inside `FacultyManager`
    * Line 62: `friend class DataManager;` inside `CourseManager`
    * Line 86: `friend class DataManager;` inside `EnrollmentManager`
    * Line 114: `friend class DataManager;` inside `AttendanceManager`
    * Line 149: `friend class DataManager;` inside `FeeManager`
* **Implementation Details:**
  * `DataManager` needs to serialize and deserialize the raw vectors of entities (including inactive/dropped records) directly.
  * Granting `friend` status allows `DataManager` direct read/write access to the private `std::vector` members, while keeping those vectors strictly private from `ConsoleUI` and the rest of the application.

### 1.11 Static Member Functions
* **Concept:** Functions associated with the class itself rather than individual object instances.
* **Location:**
  * Declarations: `include/college/DataManager.h` (lines 18–20):
    * `static std::string escapeField(const std::string& field);`
    * `static std::string unescapeField(const std::string& field);`
    * `static std::vector<std::string> splitLine(const std::string& line);`
  * Definitions: `src/DataManager.cpp` (lines 16–78)
* **Implementation Details:**
  * Field escaping, unescaping, and line splitting operate purely on input strings without requiring instance state (`this`).
  * Defined as `static` private helpers within `DataManager`.

### 1.12 Function / Const Overloading
* **Concept:** Providing multiple functions with the same name differentiated by parameter list or `const` qualifier.
* **Location:**
  * `include/college/Managers.h`:
    * Lines 30–31:
      ```cpp
      Student* findStudent(const std::string& rollNo);
      const Student* findStudent(const std::string& rollNo) const;
      ```
    * Lines 50–51:
      ```cpp
      Faculty* findFaculty(const std::string& facultyId);
      const Faculty* findFaculty(const std::string& facultyId) const;
      ```
    * Lines 70–71:
      ```cpp
      Course* findCourse(const std::string& courseCode);
      const Course* findCourse(const std::string& courseCode) const;
      ```
  * `include/college/CollegeSystem.h`:
    * Lines 31–46: Overloaded const and non-const getters for all 7 managers.
* **Implementation Details:**
  * Const-overloaded member functions allow read-only operations to be called on `const` manager references while allowing mutable operations on non-const references.

### 1.13 Exception Handling
* **Concept:** Structured mechanism for detecting, propagating, and handling runtime errors without abnormal termination.
* **Location:**
  * `src/main.cpp`:
    * Line 9: `try { ... }`
    * Lines 19–22: `catch (const std::exception& e) { std::cerr << "Fatal error: " << e.what() << std::endl; return 1; }`
* **Implementation Details:**
  * Top-level `try-catch` block catches any standard exception thrown during application execution, prints diagnostics to `std::cerr`, and ensures a clean exit code.

### 1.14 Namespaces
* **Concept:** Scope resolution mechanism preventing identifier collisions in large software projects.
* **Location:**
  * Used throughout the entire project:
    * `namespace college { ... }` in `Models.h`, `Managers.h`, `DataManager.h`, `CollegeSystem.h`, `ConsoleUI.h`
    * `using namespace college;` or explicit scope `college::` in source files and test drivers.
* **Implementation Details:**
  * Encloses all domain models, managers, persistence utilities, and UI classes in the `college` namespace.

### 1.15 File Handling and Persistence
* **Concept:** Reading and writing structured records to non-volatile secondary storage.
* **Location:**
  * `include/college/DataManager.h`
  * `src/DataManager.cpp` (lines 1–510)
* **Implementation Details:**
  * Uses `std::ifstream` and `std::ofstream` for reading/writing pipe-delimited text files (`data/*.txt`).
  * Custom escape sequences (`\\`, `\|`, `\n`, `\r`) guarantee data integrity for fields containing spaces or delimiters.
  * Preserves inactive records (soft delete) across application runs.

---

## 2. Partially Covered Concepts

### 2.1 Templates
* **Status:** Partially Covered (Standard Library Templates).
* **Location:**
  * Throughout all header and source files:
    * `std::vector<Student>`, `std::vector<Course>`, etc.
    * `std::string`, `std::pair`
    * Numeric conversions via `std::stringstream`
* **Note:**
  * Standard template library (STL) containers and algorithms are used extensively. Custom user-defined template classes/functions (`template <typename T>`) were deliberately not introduced to preserve code clarity and align with beginner/intermediate academic syllabus guidelines.

---

## 3. Concepts Not Included (and Architectural Rationale)

| Concept | Status | Architectural Rationale |
|---|---|---|
| **Operator Overloading** | Not Included | Output formatting and receipt generation were implemented using explicit named methods (`displayDetails()`, `generateReceipt()`). This avoids subtle operator precedence/stream state side effects and keeps code beginner-friendly. |
| **Multiple Inheritance** | Not Included | A clean single-inheritance model (`Person` -> `Student`, `Person` -> `Faculty`) was chosen. Multiple inheritance was unnecessary and introduces diamond-inheritance complexity. |
| **Custom Friend Functions** | Not Included | Friend *classes* (`friend class DataManager;`) were implemented to grant persistence access. Individual standalone friend functions were not needed. |
| **Custom User-Defined Templates** | Not Included | All entities have distinct domain models with specific schemas. Generic programming would overcomplicate business logic without adding value. |
