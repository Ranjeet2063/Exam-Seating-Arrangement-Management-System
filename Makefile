# ============================================================
# Exam Seating Arrangement Management System - Makefile
# ============================================================

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99
LDFLAGS = -lmysqlclient -lm

# MySQL Configuration - EDIT THESE PATHS
MYSQL_INC = "C:/Program Files/MySQL/MySQL Connector C 6.1/include"
MYSQL_LIB = "C:/Program Files/MySQL/MySQL Connector C 6.1/lib"

SRCDIR = src
EXPORTDIR = exports

SOURCES = \
    $(SRCDIR)/main.c \
    $(SRCDIR)/login.c \
    $(SRCDIR)/student.c \
    $(SRCDIR)/department.c \
    $(SRCDIR)/classroom.c \
    $(SRCDIR)/exam.c \
    $(SRCDIR)/seating.c \
    $(SRCDIR)/report.c \
    $(SRCDIR)/database.c \
    $(SRCDIR)/validation.c \
    $(SRCDIR)/utility.c \
    $(SRCDIR)/invigilator.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = esms.exe

.PHONY: all clean run directories

all: directories $(TARGET)

directories:
	@if not exist "$(EXPORTDIR)" mkdir "$(EXPORTDIR)"

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -I$(MYSQL_INC) -L$(MYSQL_LIB) $(LDFLAGS)
	@echo Build complete: $(TARGET)

$(SRCDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/header.h
	$(CC) $(CFLAGS) -I$(MYSQL_INC) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	@del /Q $(SRCDIR)\\*.o 2>NUL || echo No .o files to clean
	@del /Q $(TARGET) 2>NUL || echo No target to clean
	@echo Clean complete.
