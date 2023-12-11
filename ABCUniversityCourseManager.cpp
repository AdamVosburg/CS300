// Created by Adam Vosburg
// CS-300
// Professor Mike Susalla
// 12/7/2023

// The follwoing prgram uses a hashtable to effectively load and sort course codes.



#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <memory>
#include <filesystem>

using namespace std;

// Structure to represent a course
struct Course {
    string courseCode;
    string courseTitle;
    vector<unique_ptr<Course>> prerequisites;
};

// Class to manage ABC University courses
class ABCUniversityCourseManager {
private:
    unordered_map<string, unique_ptr<Course>> courseTable;

public:
    bool validateFileFormatAndLoadCourses(ifstream& file);
    void printCourseInformation(const string& courseCode);
    bool loadCoursesFromFile(const string& filePath);
    void displayFullCatalog();
    void displayMenu(char* argv[]);

private:
    vector<string> splitLineByComma(const string& line);
    unique_ptr<Course> createCourse(const vector<string>& parameters);
    void displayCourseNotFoundError(const string& courseCode);
    void displayMessage(const string& message);
    void displayErrorMessage(const string& message);
};

// Function to validate file format and load courses from a file
bool ABCUniversityCourseManager::validateFileFormatAndLoadCourses(ifstream& file) {
    string line;
    while (getline(file, line)) {
        
        // Split the line into parameters using commas
        vector<string> parameters = splitLineByComma(line);

        // Check for the required number of parameters
        if (parameters.size() < 2) {
            displayErrorMessage("File format error: Insufficient parameters on line: " + line);
            return false;
        }

        // Create a new Course object
        auto course = createCourse(parameters);

        // Add the Course to the Hashtable using the courseCode as the key
        courseTable[course->courseCode] = move(course);
    }

    return true;
}

// Function to print course information using Hashtable
void ABCUniversityCourseManager::printCourseInformation(const string& courseCode) {
    Course* course = courseTable[courseCode].get();
    if (course) {
        // Print course information and prerequisites
        displayMessage("Course Code: " + course->courseCode);
        displayMessage("Course Title: " + course->courseTitle);
        if (!course->prerequisites.empty()) {
            displayMessage("Prerequisites:");
            for (const auto& prerequisite : course->prerequisites) {
                displayMessage("- Course Code: " + prerequisite->courseCode);
                }
        } else {
            displayMessage("No Prerequisites for this course");
        }
    } else {
        displayCourseNotFoundError(courseCode);
    }
}

// Function to load courses from the file into the data structure
bool ABCUniversityCourseManager::loadCoursesFromFile(const string& filePath) {
    // Remove quotes if present in the file path
    string cleanedFilePath = filePath;
    if (!filePath.empty() && filePath.front() == '"' && filePath.back() == '"') {
        cleanedFilePath = filePath.substr(1, filePath.length() - 2);
    }

    // Check if the file exists before attempting to open it
    if (!cleanedFilePath.empty() && filesystem::exists(cleanedFilePath)) {
        // Use the file path directly without modifying the current working directory
        ifstream file(cleanedFilePath);
        if (file.is_open()) {
            cout << "File opened successfully!" << endl;
            if (validateFileFormatAndLoadCourses(file)) {
                displayMessage("Courses are successfully loaded");
                cout << "Please press enter to continue..." << endl;
                return true;
            }
            file.close();
        } else {
            displayErrorMessage("Error opening file. Make sure the file exists and the path is correct.");
        }
    } else {
        displayErrorMessage("File not found or empty file path provided.");
    }

    // Return false to indicate failure
    return false;
}

void ABCUniversityCourseManager::displayFullCatalog() {
    // Extract courses from the courseTable
    vector<const Course*> courses;
    for (const auto& entry : courseTable) {
        courses.push_back(entry.second.get());
    }

    // Sort the courses vector by courseCode
    sort(courses.begin(), courses.end(), [](const auto* a, const auto* b) {
        return a->courseCode < b->courseCode;
    });

    // Print the sorted list of courses
    cout << "Alphanumeric List of Courses:" << endl;
    for (const auto* course : courses) {
        cout << "- Course Code: " << course->courseCode << endl;
        cout << "  Course Title: " << course->courseTitle << endl;
    }

    // Prompt the user to press Enter
    cout << "Press Enter to continue..." << endl;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}



void ABCUniversityCourseManager::displayMenu(char* argv[]) {
    int option;
    string courseCode;

    // Check if a command-line argument is provided
    if (argv && argv[1]) {
        // If file loading is successful, proceed with the menu
        if (loadCoursesFromFile(argv[1])) {
            cout << "File loading successful. Proceeding with the menu." << endl;
        } else {
            cout << "File loading failed. Exiting the program." << endl;
            return;
        }
    }

    do {
        // Display the menu options
        displayMessage("Menu:");
        displayMessage("1. Load Data Structure");
        displayMessage("2. Display Full Course Catalog");
        displayMessage("3. Search for Course");
        displayMessage("9. Exit");
        displayMessage("Enter your option (1, 2, 3, or 9): ");

        // Check if the input is a valid integer
        while (!(cin >> option)) {
            cin.clear();  // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discarding invalid input
            displayErrorMessage("Invalid input. Please enter a number (1, 2, 3, or 9).");
        }

        switch (option) {
            case 1:
                if (!argv || !argv[1]) {
                    string filePath;
                    cout << "Enter the full path and file name: ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear buffer
                    getline(cin, filePath);
                    cout << "Attempting to load file: " << filePath << endl;
                    loadCoursesFromFile(filePath);
                }
                break;
            case 2:
                displayFullCatalog();  // Call the full course catalog
                break;
            case 3:
                displayMessage("Enter the course code to search: ");
                cin >> courseCode;
                cout << "Searching for course with code: " << courseCode << endl;
                printCourseInformation(courseCode);
                break;
            case 9:
                displayMessage("Exiting the program.");
                break;
            default:
                displayErrorMessage("Invalid option. Please enter a valid option (1, 2, 3, or 9).");
        }

        // Clear the input buffer before displaying the menu again
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

    } while (option != 9);
}


// Helper function to split a line into parameters by separating the commas
vector<string> ABCUniversityCourseManager::splitLineByComma(const string& line) {
    vector<string> parameters;

    size_t start = 0;
    size_t commaPos = line.find(',');

    while (commaPos != string::npos) {
        parameters.push_back(line.substr(start, commaPos - start));

        // Skip consecutive commas
        while (line[commaPos + 1] == ',') {
            parameters.push_back("");  // Add an empty field
            ++commaPos;
        }

        start = commaPos + 1;
        commaPos = line.find(',', start);
    }

    // Add the last parameter (or the only parameter if there is no comma)
    parameters.push_back(line.substr(start));

    return parameters;
}

// Helper function to create a new Course object
unique_ptr<Course> ABCUniversityCourseManager::createCourse(const vector<string>& parameters) {
    auto course = make_unique<Course>();
    course->courseCode = parameters[0];
    course->courseTitle = parameters[1];

    // Adjust the loop to handle prerequisites correctly
    for (size_t i = 2; i < parameters.size(); ++i) {
        // Ignore empty strings
        if (!parameters[i].empty()) {
            string prerequisiteCode = parameters[i];
            auto prerequisiteCourse = courseTable[prerequisiteCode].get();

            // If the prerequisite hasn't been encountered yet, create a placeholder
            if (!prerequisiteCourse) {
                prerequisiteCourse = new Course();
                prerequisiteCourse->courseCode = prerequisiteCode;
                // You might want to set a default title or handle this case differently
            }

            course->prerequisites.push_back(unique_ptr<Course>(prerequisiteCourse));

        }
    }

    return course;
}

// Helper function to display an error message for an invalid course code
void ABCUniversityCourseManager::displayCourseNotFoundError(const string& courseCode) {
    displayErrorMessage("Course Not Found: " + courseCode);
}

// Helper function to display a message
void ABCUniversityCourseManager::displayMessage(const string& message) {
    cout << message << endl;
    cout.flush();
}

// Helper function to display an error message
void ABCUniversityCourseManager::displayErrorMessage(const string& message) {
    cerr << "Error: " << message << endl;
}

int main(int argc, char* argv[]) {
    ABCUniversityCourseManager courseManager;
    courseManager.displayMenu(argv);
    return 0;
}
