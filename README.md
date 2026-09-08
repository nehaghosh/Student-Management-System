# Student Management System

A console-based Student Management System developed in C++ using Object-Oriented Programming (OOP), basic Data Structures, and file handling.

## Features

* Add student records
* Display all students
* Search student by ID
* Search students by name
* Update student records
* Delete student records
* Performance report sorted by marks
* Attendance warning report
* Email format validation
* Input validation
* Persistent data storage using CSV

## Concepts Used

### Object-Oriented Programming

* Classes and Objects
* Encapsulation
* Inheritance
* Abstraction
* Polymorphism
* Constructors
* Copy Constructor

### Data Structures & Algorithms

* vector
* Linear Search
* Bubble Sort

### File Handling

* File reading and writing using `fstream`
* CSV-based persistent storage
* Validation of records while loading data

## Technologies

* C++
* C++17
* Git
* GitHub

## Project Structure

```text
Student-Management-System/
│
├── StudentManagementSystem.cpp
├── students.csv
└── README.md
```

## How to Run

### Compile

```bash
g++ -std=c++17 -o sms StudentManagementSystem.cpp
```

### Run

```bash
./sms
```

The program automatically reads student records from `students.csv` and saves changes back to the file.

## Menu

```text
1. Add Student
2. Display All Students
3. Search by Student ID
4. Search by Name
5. Update Student Record
6. Delete Student Record
7. Performance Report (Sorted)
8. Attendance Warning Report
0. Exit
```

## Data Validation

The system validates:

* Student ID range and duplicate IDs
* Student names
* Email format
* Marks between 0 and 100
* Attendance between 0 and 100
* Invalid or corrupted CSV records

## Purpose

This project demonstrates the practical implementation of OOP, basic DSA concepts, input validation, and file handling in C++.
