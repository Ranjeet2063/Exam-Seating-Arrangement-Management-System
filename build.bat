@echo off
REM ============================================================
REM Exam Seating Arrangement Management System - Build Script
REM ============================================================

REM ---- MySQL Configuration - EDIT THESE PATHS ----
set MYSQL_INC="C:\Program Files\MySQL\MySQL Connector C 6.1\include"
set MYSQL_LIB="C:\Program Files\MySQL\MySQL Connector C 6.1\lib"
REM ------------------------------------------------

set CC=gcc
set CFLAGS=-Wall -Wextra -Wpedantic -std=c99
set LDFLAGS=-lmysqlclient -lm
set SRCDIR=src
set TARGET=esms.exe

echo.
echo Building Exam Seating Arrangement Management System...
echo.

if not exist exports mkdir exports

%CC% %CFLAGS% -o %TARGET% ^
    %SRCDIR%\main.c ^
    %SRCDIR%\login.c ^
    %SRCDIR%\student.c ^
    %SRCDIR%\department.c ^
    %SRCDIR%\classroom.c ^
    %SRCDIR%\exam.c ^
    %SRCDIR%\seating.c ^
    %SRCDIR%\report.c ^
    %SRCDIR%\database.c ^
    %SRCDIR%\validation.c ^
    %SRCDIR%\utility.c ^
    -I%MYSQL_INC% -L%MYSQL_LIB% %LDFLAGS%

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful! Output: %TARGET%
    echo.
) else (
    echo.
    echo Build failed with error code %ERRORLEVEL%
    echo.
)

pause
