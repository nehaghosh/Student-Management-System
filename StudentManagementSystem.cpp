/*
 * ============================================================
 *  STUDENT MANAGEMENT SYSTEM
 *  CSE343 - Summer Training Project
 *
 *  Concepts Used:
 *    OOP  - Classes, Encapsulation, Inheritance, Polymorphism
 *    DSA  - Linear Search, Bubble Sort, dynamic vector
 *    File - fstream for persistent storage
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>
#include <cmath>

using namespace std;

// ============================================================
//  HELPER FUNCTIONS
// ============================================================

// Remove leading and trailing spaces
string trim(const string& str) {
    size_t start = 0;

    while (start < str.size() &&
           isspace(static_cast<unsigned char>(str[start]))) {
        start++;
    }

    size_t end = str.size();

    while (end > start &&
           isspace(static_cast<unsigned char>(str[end - 1]))) {
        end--;
    }

    return str.substr(start, end - start);
}

// Convert string to lowercase
string toLowerCase(string str) {
    transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });

    return str;
}

// ============================================================
//  EMAIL VALIDATOR
// ============================================================

bool isValidEmail(const string& email) {

    // Empty or excessively long email
    if (email.empty() || email.length() > 254)
        return false;

    // No spaces or control characters
    for (unsigned char c : email) {
        if (isspace(c) || iscntrl(c))
            return false;
    }

    // Exactly one @
    size_t atPos = email.find('@');

    if (atPos == string::npos)
        return false;

    if (email.find('@', atPos + 1) != string::npos)
        return false;

    // Split into local part and domain
    string local = email.substr(0, atPos);
    string domain = email.substr(atPos + 1);

    // Both parts must exist
    if (local.empty() || domain.empty())
        return false;

    // Local part maximum length
    if (local.length() > 64)
        return false;

    // Local part cannot start or end with '.'
    if (local.front() == '.' || local.back() == '.')
        return false;

    // No consecutive dots
    if (local.find("..") != string::npos)
        return false;

    // Allowed characters in local part
    const string allowed =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!#$%&'*+-/=?^_`{|}~.";

    for (char c : local) {
        if (allowed.find(c) == string::npos)
            return false;
    }

    // Domain cannot start or end with '.'
    if (domain.front() == '.' || domain.back() == '.')
        return false;

    // No consecutive dots
    if (domain.find("..") != string::npos)
        return false;

    // Domain must contain '.'
    size_t dotPos = domain.rfind('.');

    if (dotPos == string::npos)
        return false;

    // Dot cannot be first or last
    if (dotPos == 0 || dotPos == domain.length() - 1)
        return false;

    // Check domain labels
    stringstream ss(domain);
    string part;

    while (getline(ss, part, '.')) {

        if (part.empty())
            return false;

        // Domain part cannot start/end with '-'
        if (part.front() == '-' || part.back() == '-')
            return false;

        // Maximum domain label length
        if (part.length() > 63)
            return false;

        // Only letters, numbers and hyphen
        for (char c : part) {
            if (!isalnum(static_cast<unsigned char>(c)) &&
                c != '-') {
                return false;
            }
        }
    }

    // TLD must have at least 2 characters
    string tld = domain.substr(dotPos + 1);

    if (tld.length() < 2)
        return false;

    // TLD must contain only letters
    for (char c : tld) {
        if (!isalpha(static_cast<unsigned char>(c)))
            return false;
    }

    return true;
}

// ============================================================
//  NAME VALIDATOR
// ============================================================

// Student name: letters, spaces, apostrophe, hyphen and dot only.
// At least one alphabetic character is required.
bool isValidName(const string& name) {

    string value = trim(name);

    if (value.empty() || value.length() > 100)
        return false;

    bool hasLetter = false;

    for (unsigned char c : value) {

        if (isalpha(c)) {
            hasLetter = true;
        }
        else if (c == ' ' || c == '\'' || c == '-' || c == '.') {
            // Allowed
        }
        else {
            return false;
        }
    }

    // Avoid obvious malformed names such as "--" or ".."
    if (value.front() == '-' || value.front() == '\'' ||
        value.front() == '.')
        return false;

    if (value.back() == '-' || value.back() == '\'' ||
        value.back() == '.')
        return false;

    return hasLetter;
}

// ============================================================
//  CSV HELPER
// ============================================================

// Handles commas inside names/course names
string csvEscape(const string& value) {

    if (value.find(',') == string::npos &&
        value.find('"') == string::npos) {
        return value;
    }

    string result = "\"";

    for (char c : value) {
        if (c == '"')
            result += "\"\"";
        else
            result += c;
    }

    result += "\"";

    return result;
}

// Parse CSV line
vector<string> parseCSV(const string& line) {

    vector<string> fields;
    string field;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.length(); i++) {

        char c = line[i];

        if (c == '"') {

            if (insideQuotes &&
                i + 1 < line.length() &&
                line[i + 1] == '"') {

                field += '"';
                i++;
            }
            else {
                insideQuotes = !insideQuotes;
            }
        }
        else if (c == ',' && !insideQuotes) {

            fields.push_back(field);
            field.clear();
        }
        else {

            field += c;
        }
    }

    // Invalid CSV if quotation was never closed
    if (insideQuotes)
        return {};

    fields.push_back(field);

    return fields;
}

// ============================================================
//  BASE CLASS - Person
//  Abstraction + Encapsulation
// ============================================================

class Person {

protected:
    int id;
    string name;
    string email;

public:

    Person()
        : id(0), name(""), email("") {}

    Person(int id, const string& name, const string& email)
        : id(id), name(name), email(email) {}

    // Getters
    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    string getEmail() const {
        return email;
    }

    // Setters
    void setName(const string& n) {
        name = n;
    }

    void setEmail(const string& e) {
        email = e;
    }

    // Pure virtual function
    virtual void display() const = 0;

    virtual ~Person() {}
};

// ============================================================
//  DERIVED CLASS - Student
//  Inheritance + Polymorphism
// ============================================================

class Student : public Person {

private:
    string course;
    float marks;
    int attendance;

public:

    // Default constructor
    Student()
        : Person(),
          course(""),
          marks(0.0f),
          attendance(0) {}

    // Parameterized constructor
    Student(int id,
            const string& name,
            const string& email,
            const string& course,
            float marks,
            int attendance)
        : Person(id, name, email),
          course(course),
          marks(marks),
          attendance(attendance) {}

    // Copy constructor
    Student(const Student& s)
        : Person(s.id, s.name, s.email),
          course(s.course),
          marks(s.marks),
          attendance(s.attendance) {}

    // Getters
    string getCourse() const {
        return course;
    }

    float getMarks() const {
        return marks;
    }

    int getAttendance() const {
        return attendance;
    }

    // Setters
    void setCourse(const string& c) {
        course = c;
    }

    void setMarks(float m) {
        marks = m;
    }

    void setAttendance(int a) {
        attendance = a;
    }

    // Calculate grade
    string getGrade() const {

        if (marks >= 90) return "A+";
        if (marks >= 80) return "A";
        if (marks >= 70) return "B+";
        if (marks >= 60) return "B";
        if (marks >= 50) return "C";

        return "F";
    }

    // Polymorphism
    void display() const override {

        cout << left
             << setw(6)  << id
             << setw(18) << name
             << setw(20) << email
             << setw(12) << course
             << setw(8)  << fixed << setprecision(2) << marks
             << setw(8)  << getGrade()
             << attendance << "%" << endl;
    }

    // ========================================================
    //  Convert Student to CSV
    // ========================================================

    string toCSV() const {

        ostringstream markStream;

        markStream << fixed
                   << setprecision(2)
                   << marks;

        return to_string(id) + "," +
               csvEscape(name) + "," +
               csvEscape(email) + "," +
               csvEscape(course) + "," +
               markStream.str() + "," +
               to_string(attendance);
    }

    // ========================================================
    //  Convert CSV to Student
    // ========================================================

    static bool fromCSV(const string& line, Student& student) {

        vector<string> fields = parseCSV(line);

        // Must contain exactly 6 fields
        if (fields.size() != 6)
            return false;

        try {

            // -------------------------
            // ID
            // -------------------------

            size_t idPos = 0;
            int id = stoi(fields[0], &idPos);

            if (idPos != fields[0].length())
                return false;

            if (id < 1 || id > 99999)
                return false;

            // -------------------------
            // Name / Email / Course
            // -------------------------

            string name = trim(fields[1]);
            string email = trim(fields[2]);
            string course = trim(fields[3]);

            if (!isValidName(name) || course.empty())
                return false;

            if (!isValidEmail(email))
                return false;

            // -------------------------
            // Marks
            // -------------------------

            size_t marksPos = 0;
            float marks = stof(fields[4], &marksPos);

            if (marksPos != fields[4].length())
                return false;

            if (!isfinite(marks) || marks < 0 || marks > 100)
                return false;

            // -------------------------
            // Attendance
            // -------------------------

            size_t attendancePos = 0;
            int attendance =
                stoi(fields[5], &attendancePos);

            if (attendancePos != fields[5].length())
                return false;

            if (attendance < 0 || attendance > 100)
                return false;

            // Everything valid
            student = Student(
                id,
                name,
                email,
                course,
                marks,
                attendance
            );

            return true;
        }
        catch (...) {

            return false;
        }
    }
};

// ============================================================
//  FILE HANDLER
// ============================================================

class FileHandler {

private:
    string filename;

public:

    FileHandler(const string& f)
        : filename(f) {}

    // Save all students
    bool save(const vector<Student>& students) {

        ofstream fout(filename);

        if (!fout) {

            cerr << "[Error] Cannot open file for writing.\n";
            return false;
        }

        for (const auto& s : students)
            fout << s.toCSV() << "\n";

        if (!fout.good()) {

            cerr << "[Error] Failed while writing data.\n";
            return false;
        }

        fout.close();

        return true;
    }
    

    // Load students
    vector<Student> load() {

        vector<Student> students;

        ifstream fin(filename);

        // File does not exist yet
        if (!fin)
            return students;

        string line;
        int lineNumber = 0;

        while (getline(fin, line)) {

            lineNumber++;

            if (trim(line).empty())
                continue;

            Student student;

            // Only add valid records
            if (Student::fromCSV(line, student)) {

                // Prevent duplicate IDs
                bool duplicate = false;

                for (const auto& existing : students) {

                    if (existing.getId() ==
                        student.getId()) {

                        duplicate = true;
                        break;
                    }
                }

                if (!duplicate) {

                    students.push_back(student);
                }
                else {

                    cerr << "[Warning] Duplicate ID on line "
                         << lineNumber
                         << " skipped.\n";
                }
            }
            else {

                cerr << "[Warning] Invalid record on line "
                     << lineNumber
                     << " skipped.\n";
            }
        }

        fin.close();

        return students;
    }
};

// ============================================================
//  STUDENT MANAGEMENT SYSTEM
// ============================================================

class StudentManagementSystem {

private:

    vector<Student> students;
    FileHandler fileHandler;

    // ========================================================
    //  Print table header
    // ========================================================

    void printHeader() const {

        cout << string(88, '-') << "\n";

        cout << left
             << setw(6)  << "ID"
             << setw(18) << "Name"
             << setw(20) << "Email"
             << setw(12) << "Course"
             << setw(8)  << "Marks"
             << setw(8)  << "Grade"
             << "Attendance%" << "\n";

        cout << string(88, '-') << "\n";
    }

    // ========================================================
    //  Check duplicate ID
    // ========================================================

    bool idExists(int id) const {

        for (const auto& s : students) {

            if (s.getId() == id)
                return true;
        }

        return false;
    }

    // ========================================================
    //  Linear Search by ID
    // ========================================================

    int linearSearchById(int id) const {

        for (int i = 0;
             i < static_cast<int>(students.size());
             i++) {

            if (students[i].getId() == id)
                return i;
        }

        return -1;
    }

    // ========================================================
    //  Linear Search by Name
    // ========================================================

    vector<int> searchByName(
        const string& keyword) const {

        vector<int> results;

        string kw = toLowerCase(trim(keyword));

        for (int i = 0;
             i < static_cast<int>(students.size());
             i++) {

            string nm =
                toLowerCase(students[i].getName());

            if (nm.find(kw) != string::npos)
                results.push_back(i);
        }

        return results;
    }

    // ========================================================
    //  Bubble Sort by Marks - Descending
    // ========================================================

    void bubbleSortByMarks(
        vector<Student>& list) const {

        int n = static_cast<int>(list.size());

        for (int i = 0; i < n - 1; i++) {

            bool swapped = false;

            for (int j = 0;
                 j < n - i - 1;
                 j++) {

                if (list[j].getMarks() <
                    list[j + 1].getMarks()) {

                    swap(list[j], list[j + 1]);

                    swapped = true;
                }
            }

            // Stop if already sorted
            if (!swapped)
                break;
        }
    }

    // ========================================================
    //  Safe Integer Input
    // ========================================================

    int readInt(
        const string& prompt,
        int lo,
        int hi) {

        while (true) {

            cout << prompt;

            string input;
            getline(cin, input);

            input = trim(input);

            if (input.empty()) {

                cout << "  Invalid input. "
                        "Enter a number between "
                     << lo << " and " << hi << ".\n";

                continue;
            }

            try {

                size_t pos = 0;

                int val = stoi(input, &pos);

                // Reject things like 123abc
                if (pos != input.length())
                    throw invalid_argument("Extra characters");

                if (val >= lo && val <= hi)
                    return val;
            }
            catch (...) {
                // Invalid input
            }

            cout << "  Invalid input. "
                    "Enter a number between "
                 << lo << " and " << hi << ".\n";
        }
    }

    // ========================================================
    //  Safe Float Input
    // ========================================================

    float readFloat(
        const string& prompt,
        float lo,
        float hi) {

        while (true) {

            cout << prompt;

            string input;
            getline(cin, input);

            input = trim(input);

            if (input.empty()) {

                cout << "  Invalid input. "
                        "Enter a number between "
                     << lo << " and " << hi << ".\n";

                continue;
            }

            try {

                size_t pos = 0;

                float val = stof(input, &pos);

                // Reject things like 85abc
                if (pos != input.length())
                    throw invalid_argument("Extra characters");

                if (val >= lo && val <= hi)
                    return val;
            }
            catch (...) {
                // Invalid input
            }

            cout << "  Invalid input. "
                    "Enter a number between "
                 << lo << " and " << hi << ".\n";
        }
    }

    // ========================================================
    //  Safe String Input
    // ========================================================

    string readLine(const string& prompt) {

        while (true) {

            cout << prompt;

            string val;
            getline(cin, val);

            val = trim(val);

            if (!val.empty())
                return val;

            cout << "  Cannot be empty. Try again.\n";
        }
    }

    // ========================================================
    //  Name Input
    // ========================================================

    string readName(const string& prompt) {

        while (true) {

            cout << prompt;

            string val;
            getline(cin, val);

            val = trim(val);

            if (isValidName(val))
                return val;

            cout << "  Invalid name. Use letters, spaces, apostrophe, "
                    "hyphen or dot only. Numbers are not allowed.\n";
        }
    }

    // ========================================================
    //  Email Input
    // ========================================================

    string readEmail(const string& prompt) {

        while (true) {

            cout << prompt;

            string val;
            getline(cin, val);

            val = trim(val);

            if (val.empty()) {

                cout << "  Email cannot be empty.\n";
            }
            else if (!isValidEmail(val)) {

                cout << "  Invalid email!\n";
                cout << "  Example: name@gmail.com\n";
            }
            else {

                return val;
            }
        }
    }

    // ========================================================
    //  Safe stof for Update
    // ========================================================

    bool safeStof(
        const string& input,
        float& out) {

        string s = trim(input);

        if (s.empty())
            return false;

        try {

            size_t pos = 0;

            float value = stof(s, &pos);

            // Entire input must be numeric
            if (pos != s.length() || !isfinite(value))
                return false;

            out = value;

            return true;
        }
        catch (...) {

            return false;
        }
    }

    // ========================================================
    //  Safe stoi for Update
    // ========================================================

    bool safeStoi(
        const string& input,
        int& out) {

        string s = trim(input);

        if (s.empty())
            return false;

        try {

            size_t pos = 0;

            int value = stoi(s, &pos);

            // Entire input must be numeric
            if (pos != s.length() || !isfinite(value))
                return false;

            out = value;

            return true;
        }
        catch (...) {

            return false;
        }
    }

public:

    // ========================================================
    //  Constructor
    // ========================================================

    StudentManagementSystem(
        const string& dataFile = "students.csv")
        : fileHandler(dataFile) {

        students = fileHandler.load();

        cout << "  [Info] Loaded "
             << students.size()
             << " record(s) from file.\n\n";
    }

    // ========================================================
    //  1. Add Student
    // ========================================================

    void addStudent() {

        cout << "\n=== ADD STUDENT ===\n";

        int id =
            readInt("  Student ID   : ", 1, 99999);

        if (idExists(id)) {

            cout << "  [Error] ID "
                 << id
                 << " already exists.\n";

            return;
        }

        string name =
            readName("  Full Name     : ");

        string email =
            readEmail("  Email         : ");

        string course =
            readLine("  Course        : ");

        float marks =
            readFloat(
                "  Marks (0-100) : ",
                0,
                100
            );

        int attend =
            readInt(
                "  Attendance %  : ",
                0,
                100
            );

        students.emplace_back(
            id,
            name,
            email,
            course,
            marks,
            attend
        );

        if (fileHandler.save(students)) {

            cout << "  [OK] Student added successfully.\n";
        }
        else {

            // Roll back if saving failed
            students.pop_back();

            cout << "  [Error] Student could not be saved.\n";
        }
    }

    // ========================================================
    //  2. Display All Students
    // ========================================================

    void displayAll() const {

        cout << "\n=== ALL STUDENTS ===\n";

        if (students.empty()) {

            cout << "  No records found.\n";
            return;
        }

        printHeader();

        for (const auto& s : students)
            s.display();

        cout << string(88, '-') << "\n";

        cout << "  Total records: "
             << students.size()
             << "\n";
    }

    // ========================================================
    //  3. Search by ID
    // ========================================================

    void searchById() {

        cout << "\n=== SEARCH BY ID ===\n";

        int id =
            readInt(
                "  Enter Student ID: ",
                1,
                99999
            );

        int idx =
            linearSearchById(id);

        if (idx == -1) {

            cout << "  [Not Found] "
                    "No student with ID "
                 << id << ".\n";

            return;
        }

        printHeader();

        students[idx].display();

        cout << string(88, '-') << "\n";
    }

    // ========================================================
    //  4. Search by Name
    // ========================================================

    void searchByNameMenu() {

        cout << "\n=== SEARCH BY NAME ===\n";

        string kw =
            readLine("  Enter name/keyword: ");

        auto results =
            searchByName(kw);

        if (results.empty()) {

            cout << "  [Not Found] "
                    "No matching student.\n";

            return;
        }

        printHeader();

        for (int i : results)
            students[i].display();

        cout << string(88, '-') << "\n";

        cout << "  "
             << results.size()
             << " match(es) found.\n";
    }

    // ========================================================
    //  5. Update Student
    // ========================================================

    void updateStudent() {

        cout << "\n=== UPDATE STUDENT ===\n";

        int id =
            readInt(
                "  Enter Student ID to update: ",
                1,
                99999
            );

        int idx =
            linearSearchById(id);

        if (idx == -1) {

            cout << "  [Not Found] "
                    "No student with ID "
                 << id << ".\n";

            return;
        }

        Student oldStudent = students[idx];
        Student& s = students[idx];

        cout << "  Current record:\n";

        printHeader();

        s.display();

        cout << string(88, '-') << "\n";

        cout << "  Enter new values "
                "(press Enter to keep current):\n";

        string input;

        // ----------------------------------------------------
        // Name
        // ----------------------------------------------------

        while (true) {

            cout << "  Name  ["
                 << s.getName()
                 << "]: ";

            getline(cin, input);

            input = trim(input);

            // Empty = keep old name
            if (input.empty())
                break;

            if (isValidName(input)) {
                s.setName(input);
                break;
            }

            cout << "  Invalid name. Use letters, spaces, apostrophe, "
                    "hyphen or dot only. Numbers are not allowed.\n";
        }

        // ----------------------------------------------------
        // Email
        // ----------------------------------------------------

        while (true) {

            cout << "  Email ["
                 << s.getEmail()
                 << "]: ";

            getline(cin, input);

            input = trim(input);

            // Empty = keep old email
            if (input.empty())
                break;

            if (isValidEmail(input)) {

                s.setEmail(input);
                break;
            }

            cout << "  Invalid email! "
                    "Example: name@gmail.com\n";
        }

        // ----------------------------------------------------
        // Course
        // ----------------------------------------------------

        cout << "  Course ["
             << s.getCourse()
             << "]: ";

        getline(cin, input);

        input = trim(input);

        if (!input.empty())
            s.setCourse(input);

        // ----------------------------------------------------
        // Marks
        // ----------------------------------------------------

        while (true) {

            cout << "  Marks ["
                 << s.getMarks()
                 << "]: ";

            getline(cin, input);

            input = trim(input);

            // Empty = keep old marks
            if (input.empty())
                break;

            float m;

            if (safeStof(input, m) &&
                m >= 0 &&
                m <= 100) {

                s.setMarks(m);
                break;
            }

            cout << "  Invalid! Marks must be "
                    "between 0 and 100.\n";
        }

        // ----------------------------------------------------
        // Attendance
        // ----------------------------------------------------

        while (true) {

            cout << "  Attendance% ["
                 << s.getAttendance()
                 << "]: ";

            getline(cin, input);

            input = trim(input);

            // Empty = keep old attendance
            if (input.empty())
                break;

            int a;

            if (safeStoi(input, a) &&
                a >= 0 &&
                a <= 100) {

                s.setAttendance(a);
                break;
            }

            cout << "  Invalid! Attendance must be "
                    "between 0 and 100.\n";
        }

        if (fileHandler.save(students)) {

            cout << "  [OK] Record updated.\n";
        }
        else {

            // Restore old record if file saving failed
            students[idx] = oldStudent;

            cout << "  [Error] Could not save changes. "
                    "Changes were rolled back.\n";
        }
    }

    // ========================================================
    //  6. Delete Student
    // ========================================================

    void deleteStudent() {

        cout << "\n=== DELETE STUDENT ===\n";

        if (students.empty()) {

            cout << "  No records to delete.\n";
            return;
        }

        int id =
            readInt(
                "  Enter Student ID to delete: ",
                1,
                99999
            );

        int idx =
            linearSearchById(id);

        if (idx == -1) {

            cout << "  [Not Found] "
                    "No student with ID "
                 << id << ".\n";

            return;
        }

        cout << "  Are you sure you want to delete "
             << students[idx].getName()
             << "? (y/n): ";

        char confirm;

        cin >> confirm;

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        if (confirm != 'y' &&
            confirm != 'Y') {

            cout << "  Delete cancelled.\n";
            return;
        }

        // Keep a copy in case saving fails
        Student deletedStudent =
            students[idx];

        students.erase(
            students.begin() + idx
        );

        if (fileHandler.save(students)) {

            cout << "  [OK] Record deleted.\n";
        }
        else {

            // Restore deleted student
            students.insert(
                students.begin() + idx,
                deletedStudent
            );

            cout << "  [Error] Could not save changes. "
                    "Delete cancelled.\n";
        }
    }

    // ========================================================
    //  7. Performance Report
    // ========================================================

    void generateReport() const {

        cout << "\n=== PERFORMANCE REPORT "
                "(Sorted by Marks - High to Low) ===\n";

        if (students.empty()) {

            cout << "  No records found.\n";
            return;
        }

        vector<Student> sorted = students;

        // Use Bubble Sort function
        bubbleSortByMarks(sorted);

        printHeader();

        int rank = 1;

        for (const auto& s : sorted) {

            cout << "["
                 << setw(2)
                 << rank++
                 << "] ";

            s.display();
        }

        cout << string(88, '-') << "\n";

        // Calculate class average
        float total = 0;

        for (const auto& s : sorted)
            total += s.getMarks();

        cout << "  Class Average: "
             << fixed
             << setprecision(2)
             << (total / sorted.size())
             << " / 100\n";
    }

    // ========================================================
    //  8. Attendance Warning
    // ========================================================

    void attendanceWarning() const {

        cout << "\n=== ATTENDANCE WARNING "
                "(Below 75%) ===\n";

        if (students.empty()) {

            cout << "  No records found.\n";
            return;
        }

        bool found = false;

        for (const auto& s : students) {

            if (s.getAttendance() < 75) {

                if (!found)
                    printHeader();

                s.display();
                found = true;
            }
        }

        if (!found) {

            cout << "  All students have attendance "
                    ">= 75%. Great!\n";
        }
        else {

            cout << string(88, '-') << "\n";
        }
    }
};

// ============================================================
//  MAIN
// ============================================================

int main() {

    cout << "============================================\n";
    cout << "   STUDENT MANAGEMENT SYSTEM - C++ (OOP)\n";
    cout << "   CSE343 | Neha Ghosh | LPU\n";
    cout << "============================================\n\n";

    StudentManagementSystem sms("students.csv");

    bool running = true;

    while (running) {

        cout << "\n--- MAIN MENU --------------------------\n";
        cout << "  1. Add Student\n";
        cout << "  2. Display All Students\n";
        cout << "  3. Search by Student ID\n";
        cout << "  4. Search by Name\n";
        cout << "  5. Update Student Record\n";
        cout << "  6. Delete Student Record\n";
        cout << "  7. Performance Report (Sorted)\n";
        cout << "  8. Attendance Warning Report\n";
        cout << "  0. Exit\n";
        cout << "----------------------------------------\n";

        int choice;

        cout << "  Choose an option: ";

        string input;
        getline(cin, input);

        input = trim(input);

        try {

            size_t pos = 0;

            choice = stoi(input, &pos);

            // Reject input such as 2abc
            if (pos != input.length())
                throw invalid_argument("Invalid choice");

        }
        catch (...) {

            cout << "  Invalid choice. Try again.\n";
            continue;
        }

        switch (choice) {

            case 1:
                sms.addStudent();
                break;

            case 2:
                sms.displayAll();
                break;

            case 3:
                sms.searchById();
                break;

            case 4:
                sms.searchByNameMenu();
                break;

            case 5:
                sms.updateStudent();
                break;

            case 6:
                sms.deleteStudent();
                break;

            case 7:
                sms.generateReport();
                break;

            case 8:
                sms.attendanceWarning();
                break;

            case 0:

                cout << "\n  Goodbye! "
                        "Data saved to students.csv\n\n";

                running = false;
                break;

            default:

                cout << "  Invalid choice. "
                        "Enter 0 to 8.\n";
        }
    }

    return 0;
}
