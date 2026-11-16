#ifndef COLLEGE_CONSOLE_UI_H
#define COLLEGE_CONSOLE_UI_H

// College Management System - Console UI
// Complete console-based user interface for all application modules.

#include "college/CollegeSystem.h"
#include <string>

namespace college {

class ConsoleUI {
private:
    CollegeSystem& system;

    // --- Input helpers ---
    std::string readLine(const std::string& prompt) const;
    int readInt(const std::string& prompt) const;
    double readDouble(const std::string& prompt) const;

    // --- Display helpers ---
    void printHeading(const std::string& title) const;
    void pauseScreen() const;
    void autoSave();

    // --- Sub-menus ---
    void studentMenu();
    void facultyMenu();
    void courseMenu();
    void enrollmentMenu();
    void attendanceMenu();
    void feeMenu();
    void reportMenu();
    void dataMenu();

public:
    explicit ConsoleUI(CollegeSystem& system);

    void run();
    void showMainMenu();
};

} // namespace college

#endif // COLLEGE_CONSOLE_UI_H
