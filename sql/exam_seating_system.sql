-- ============================================================
-- Exam Seating Arrangement Management System
-- Database Schema and Sample Data
-- ============================================================

CREATE DATABASE IF NOT EXISTS exam_seating_system
  CHARACTER SET utf8mb4
  COLLATE utf8mb4_unicode_ci;

USE exam_seating_system;

-- ============================================================
-- 1. ADMIN TABLE
-- ============================================================
DROP TABLE IF EXISTS admin;
CREATE TABLE admin (
    admin_id    INT AUTO_INCREMENT PRIMARY KEY,
    username    VARCHAR(50)  NOT NULL UNIQUE,
    password    VARCHAR(128) NOT NULL
) ENGINE=InnoDB;

-- ============================================================
-- 2. DEPARTMENTS TABLE
-- ============================================================
DROP TABLE IF EXISTS departments;
CREATE TABLE departments (
    department_id   INT AUTO_INCREMENT PRIMARY KEY,
    department_name VARCHAR(100) NOT NULL UNIQUE
) ENGINE=InnoDB;

-- ============================================================
-- 3. STUDENTS TABLE
-- ============================================================
DROP TABLE IF EXISTS students;
CREATE TABLE students (
    student_id        INT AUTO_INCREMENT PRIMARY KEY,
    roll_number       VARCHAR(30)  NOT NULL UNIQUE,
    registration_number VARCHAR(30) NOT NULL UNIQUE,
    full_name         VARCHAR(100) NOT NULL,
    department        VARCHAR(100) NOT NULL,
    semester          INT          NOT NULL,
    year              INT          NOT NULL,
    phone             VARCHAR(15)  NOT NULL,
    email             VARCHAR(100) NOT NULL
) ENGINE=InnoDB;

-- ============================================================
-- 4. CLASSROOMS TABLE
-- ============================================================
DROP TABLE IF EXISTS classrooms;
CREATE TABLE classrooms (
    room_id    INT AUTO_INCREMENT PRIMARY KEY,
    room_name  VARCHAR(50) NOT NULL UNIQUE,
    building   VARCHAR(100) NOT NULL,
    floor      INT         NOT NULL,
    capacity   INT         NOT NULL
) ENGINE=InnoDB;

-- ============================================================
-- 5. EXAMS TABLE
-- ============================================================
DROP TABLE IF EXISTS exams;
CREATE TABLE exams (
    exam_id     INT AUTO_INCREMENT PRIMARY KEY,
    subject     VARCHAR(100) NOT NULL,
    exam_date   DATE         NOT NULL,
    start_time  TIME         NOT NULL,
    end_time    TIME         NOT NULL,
    semester    INT          NOT NULL,
    department  VARCHAR(100) NOT NULL
) ENGINE=InnoDB;

-- ============================================================
-- 6. SEATING TABLE
-- ============================================================
DROP TABLE IF EXISTS seating;
CREATE TABLE seating (
    seating_id   INT AUTO_INCREMENT PRIMARY KEY,
    exam_id      INT NOT NULL,
    student_id   INT NOT NULL,
    room_id      INT NOT NULL,
    seat_number  INT NOT NULL,
    row_number   INT NOT NULL,
    column_number INT NOT NULL,
    UNIQUE KEY unique_seat (exam_id, room_id, seat_number),
    UNIQUE KEY unique_student_exam (exam_id, student_id),
    FOREIGN KEY (exam_id)    REFERENCES exams(exam_id)    ON DELETE CASCADE,
    FOREIGN KEY (student_id) REFERENCES students(student_id) ON DELETE CASCADE,
    FOREIGN KEY (room_id)    REFERENCES classrooms(room_id) ON DELETE CASCADE
) ENGINE=InnoDB;

-- ============================================================
-- DEFAULT ADMIN (username: admin, password: admin123)
-- ============================================================
INSERT INTO admin (username, password)
VALUES ('admin', 'admin123');

-- ============================================================
-- SAMPLE DEPARTMENTS
-- ============================================================
INSERT INTO departments (department_name) VALUES
('Computer Science & Engineering'),
('Electronics & Communication'),
('Mechanical Engineering'),
('Civil Engineering'),
('Electrical Engineering'),
('Information Technology');

-- ============================================================
-- SAMPLE STUDENTS (6 per department, 36 total)
-- ============================================================
INSERT INTO students (roll_number, registration_number, full_name, department, semester, year, phone, email) VALUES
-- CSE
('CS1001','REG-CS-001','Aarav Sharma','Computer Science & Engineering',3,2,'9876543210','aarav.sharma@example.com'),
('CS1002','REG-CS-002','Priya Patel','Computer Science & Engineering',3,2,'9876543211','priya.patel@example.com'),
('CS1003','REG-CS-003','Rohit Verma','Computer Science & Engineering',3,2,'9876543212','rohit.verma@example.com'),
('CS1004','REG-CS-004','Sneha Gupta','Computer Science & Engineering',3,2,'9876543213','sneha.gupta@example.com'),
('CS1005','REG-CS-005','Arjun Singh','Computer Science & Engineering',3,2,'9876543214','arjun.singh@example.com'),
('CS1006','REG-CS-006','Neha Kapoor','Computer Science & Engineering',3,2,'9876543215','neha.kapoor@example.com'),
-- ECE
('EC1001','REG-EC-001','Rahul Yadav','Electronics & Communication',3,2,'9876543216','rahul.yadav@example.com'),
('EC1002','REG-EC-002','Simran Kaur','Electronics & Communication',3,2,'9876543217','simran.kaur@example.com'),
('EC1003','REG-EC-003','Vikram Joshi','Electronics & Communication',3,2,'9876543218','vikram.joshi@example.com'),
('EC1004','REG-EC-004','Ananya Reddy','Electronics & Communication',3,2,'9876543219','ananya.reddy@example.com'),
('EC1005','REG-EC-005','Karan Mehta','Electronics & Communication',3,2,'9876543220','karan.mehta@example.com'),
('EC1006','REG-EC-006','Isha Agarwal','Electronics & Communication',3,2,'9876543221','isha.agarwal@example.com'),
-- ME
('ME1001','REG-ME-001','Manish Kumar','Mechanical Engineering',5,3,'9876543222','manish.kumar@example.com'),
('ME1002','REG-ME-002','Pooja Desai','Mechanical Engineering',5,3,'9876543223','pooja.desai@example.com'),
('ME1003','REG-ME-003','Suresh Nair','Mechanical Engineering',5,3,'9876543224','suresh.nair@example.com'),
('ME1004','REG-ME-004','Divya Mishra','Mechanical Engineering',5,3,'9876543225','divya.mishra@example.com'),
('ME1005','REG-ME-005','Amit Saxena','Mechanical Engineering',5,3,'9876543226','amit.saxena@example.com'),
('ME1006','REG-ME-006','Kavita Jain','Mechanical Engineering',5,3,'9876543227','kavita.jain@example.com'),
-- CE
('CE1001','REG-CE-001','Ravi Tiwari','Civil Engineering',3,2,'9876543228','ravi.tiwari@example.com'),
('CE1002','REG-CE-002','Sonia Bhatia','Civil Engineering',3,2,'9876543229','sonia.bhatia@example.com'),
('CE1003','REG-CE-003','Deepak Chauhan','Civil Engineering',3,2,'9876543230','deepak.chauhan@example.com'),
('CE1004','REG-CE-004','Nisha Agarwal','Civil Engineering',3,2,'9876543231','nisha.agarwal@example.com'),
('CE1005','REG-CE-005','Pankaj Ghosh','Civil Engineering',3,2,'9876543232','pankaj.ghosh@example.com'),
('CE1006','REG-CE-006','Ritu Sharma','Civil Engineering',3,2,'9876543233','ritu.sharma@example.com'),
-- EE
('EE1001','REG-EE-001','Rajesh Pandey','Electrical Engineering',5,3,'9876543234','rajesh.pandey@example.com'),
('EE1002','REG-EE-002','Megha Singh','Electrical Engineering',5,3,'9876543235','megha.singh@example.com'),
('EE1003','REG-EE-003','Vivek Oberoi','Electrical Engineering',5,3,'9876543236','vivek.oberoi@example.com'),
('EE1004','REG-EE-004','Tanvi Shah','Electrical Engineering',5,3,'9876543237','tanvi.shah@example.com'),
('EE1005','REG-EE-005','Gaurav Malik','Electrical Engineering',5,3,'9876543238','gaurav.malik@example.com'),
('EE1006','REG-EE-006','Anjali Nair','Electrical Engineering',5,3,'9876543239','anjali.nair@example.com'),
-- IT
('IT1001','REG-IT-001','Akash Verma','Information Technology',3,2,'9876543240','akash.verma@example.com'),
('IT1002','REG-IT-002','Bhavna Das','Information Technology',3,2,'9876543241','bhavna.das@example.com'),
('IT1003','REG-IT-003','Chirag Shetty','Information Technology',3,2,'9876543242','chirag.shetty@example.com'),
('IT1004','REG-IT-004','Deepika Rao','Information Technology',3,2,'9876543243','deepika.rao@example.com'),
('IT1005','REG-IT-005','Esha Malhotra','Information Technology',3,2,'9876543244','esha.malhotra@example.com'),
('IT1006','REG-IT-006','Farhan Qureshi','Information Technology',3,2,'9876543245','farhan.qureshi@example.com');

-- ============================================================
-- SAMPLE CLASSROOMS
-- ============================================================
INSERT INTO classrooms (room_name, building, floor, capacity) VALUES
('LH-101','Lecture Hall Complex',1,60),
('LH-102','Lecture Hall Complex',1,60),
('LH-201','Lecture Hall Complex',2,50),
('LH-202','Lecture Hall Complex',2,50),
('CB-301','College Building',3,40),
('CB-302','College Building',3,40),
('CB-401','College Building',4,30),
('CB-402','College Building',4,30);

-- ============================================================
-- SAMPLE EXAMS
-- ============================================================
INSERT INTO exams (subject, exam_date, start_time, end_time, semester, department) VALUES
('Data Structures','2026-07-10','09:00:00','12:00:00',3,'Computer Science & Engineering'),
('Digital Electronics','2026-07-11','09:00:00','12:00:00',3,'Electronics & Communication'),
('Thermodynamics','2026-07-12','09:00:00','12:00:00',5,'Mechanical Engineering'),
('Database Management Systems','2026-07-10','14:00:00','17:00:00',3,'Information Technology');
