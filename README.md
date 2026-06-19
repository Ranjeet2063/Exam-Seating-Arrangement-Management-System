# Exam Seating Arrangement Management System

A complete C-based console application for managing exam seating arrangements in universities. Features automatic seat allocation with anti-cheating patterns, student/classroom/exam management, and comprehensive reporting.

## Features

- **Secure Login** with 3-attempt password protection
- **Student Management** - Add, update, delete, search students with roll/registration numbers
- **Department Management** - Organize departments
- **Classroom Management** - Rooms with building, floor, and capacity tracking
- **Examination Management** - Schedule exams with date/time, link to departments
- **Automatic Seating Allocation** - Algorithm assigns seats respecting:
  - Room capacity (fill Room A before Room B)
  - Sequential seat numbering
  - Anti-cheating row/column alternation
  - No duplicate student per exam
  - No duplicate seat per room
- **View & Export Seating Plans** - On-screen and TXT file export
- **Print Seat Cards** - Individual student seat cards
- **Comprehensive Reports** - Dashboard, attendance sheets, room occupancy, empty seats, capacity utilization
- **Database Backup/Restore** via mysqldump
- **Change Password** functionality

## Technology Stack

- **Language**: C (C99)
- **Compiler**: GCC/MinGW
- **Database**: MySQL with Connector/C
- **Platform**: Windows (Console)
- **Build**: Makefile / build.bat

## Project Structure

```
ExamSeatingSystem/
├── src/
│   ├── header.h        # All structures, constants, prototypes
│   ├── main.c          # Entry point, main menu (15 options)
│   ├── login.c         # Authentication with 3-attempt limit
│   ├── student.c       # Student CRUD operations
│   ├── department.c    # Department management
│   ├── classroom.c     # Classroom management
│   ├── exam.c          # Exam scheduling
│   ├── seating.c       # Automatic allocation algorithm
│   ├── report.c        # Reports and dashboard
│   ├── database.c      # MySQL connection layer
│   ├── validation.c    # Input validation
│   └── utility.c       # Console UI, colors, animations
├── sql/
│   └── exam_seating_system.sql   # Schema + sample data
├── exports/            # Generated TXT files
├── Makefile
├── build.bat
└── README.md
```

## Database Setup

1. Install MySQL Server and MySQL Connector/C
2. Import the database schema:
   ```
   mysql -u root < sql/exam_seating_system.sql
   ```
3. Default admin credentials:
   - Username: `admin`
   - Password: `admin123`

## Compilation

### Using Makefile (MinGW/MSYS2):
```bash
cd ExamSeatingSystem
make
```

### Using build.bat (Windows):
Edit `build.bat` to set your MySQL include/lib paths, then:
```cmd
build
```

### Manual compilation:
```bash
gcc -o esms.exe src/main.c src/login.c src/student.c src/department.c \
    src/classroom.c src/exam.c src/seating.c src/report.c \
    src/database.c src/validation.c src/utility.c \
    -I"C:\Program Files\MySQL\MySQL Connector C 6.1\include" \
    -L"C:\Program Files\MySQL\MySQL Connector C 6.1\lib" \
    -lmysqlclient
```

## Usage

1. Run `esms.exe`
2. Login with `admin` / `admin123`
3. Use the main menu (options 1-15):
   - Start with **Department Management** to add departments
   - Add **Classrooms** with capacity
   - Add **Students** with roll numbers
   - Create **Exams** for specific departments/semesters
   - Run **Automatic Seating Allocation**
   - **View/Export** seating plans

## Seating Algorithm

The auto-allocation algorithm:
1. Selects an exam and finds eligible students (same department + semester)
2. Retrieves available classrooms sorted by capacity (ascending)
3. Fills each room completely before moving to the next
4. Assigns seat numbers sequentially within each room
5. Calculates rows/columns for anti-cheating patterns
6. Enforces no duplicate student per exam and no duplicate seat per room
7. Respects foreign key constraints

## Sample Data

The SQL script includes:
- 1 admin user
- 6 departments
- 36 students (6 per department)
- 8 classrooms (30-60 capacity each)
- 4 sample exams

## License

This project is for educational purposes. Free to use and modify.
