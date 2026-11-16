// College Management System - Entry Point

#include "college/CollegeSystem.h"
#include "college/ConsoleUI.h"
#include <iostream>
#include <exception>

int main() {
    try {
        college::CollegeSystem system;

        // Load existing data at startup
        if (!system.loadAllData()) {
            std::cout << "Note: Some data files could not be loaded.\n";
            std::cout << "This is normal on first launch.\n\n";
        }

        college::ConsoleUI ui(system);
        ui.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
