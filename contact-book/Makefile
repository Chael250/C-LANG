# Makefile for Contact Book System
# Author: Contact Book System
# Version: 1.0

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
LDFLAGS = -lmysqlclient -lbcrypt

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

# Source files
SOURCES = main.c database.c auth.c utils.c
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/contact_book

# MySQL configuration
MYSQL_CFLAGS = $(shell mysql_config --cflags)
MYSQL_LDFLAGS = $(shell mysql_config --libs)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p logs

# Build the main executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(MYSQL_LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -c $< -o $@

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BIN_DIR)
	@echo "Clean complete"

# Clean all (including logs)
clean-all: clean
	@echo "Cleaning logs..."
	@rm -f *.log
	@rm -rf logs
	@echo "Clean all complete"

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	@sudo apt-get update
	@sudo apt-get install -y build-essential
	@sudo apt-get install -y libmysqlclient-dev
	@sudo apt-get install -y libbcrypt-dev
	@sudo apt-get install -y mysql-server
	@echo "Dependencies installed"

# Install dependencies (CentOS/RHEL/Fedora)
install-deps-rpm:
	@echo "Installing dependencies..."
	@sudo yum install -y gcc gcc-c++ make
	@sudo yum install -y mysql-devel
	@sudo yum install -y bcrypt-devel
	@sudo yum install -y mysql-server
	@echo "Dependencies installed"

# Database setup
setup-db:
	@echo "Setting up database..."
	@mysql -u root -p -e "CREATE DATABASE IF NOT EXISTS contact_book;"
	@mysql -u root -p -e "CREATE USER IF NOT EXISTS 'contact_user'@'localhost' IDENTIFIED BY 'contact_pass';"
	@mysql -u root -p -e "GRANT ALL PRIVILEGES ON contact_book.* TO 'contact_user'@'localhost';"
	@mysql -u root -p -e "FLUSH PRIVILEGES;"
	@echo "Database setup complete"

# Run the application
run: $(TARGET)
	@echo "Running Contact Book System..."
	@./$(TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -g3
debug: clean $(TARGET)

# Release build
release: CFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

# Install the application
install: $(TARGET)
	@echo "Installing Contact Book System..."
	@sudo cp $(TARGET) /usr/local/bin/
	@sudo chmod +x /usr/local/bin/contact_book
	@echo "Installation complete"

# Uninstall the application
uninstall:
	@echo "Uninstalling Contact Book System..."
	@sudo rm -f /usr/local/bin/contact_book
	@echo "Uninstallation complete"

# Run tests
test: $(TARGET)
	@echo "Running tests..."
	@./scripts/run_tests.sh

# Generate documentation
docs:
	@echo "Generating documentation..."
	@doxygen Doxyfile

# Check for memory leaks
memcheck: $(TARGET)
	@echo "Running memory check..."
	@valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Static analysis
analyze:
	@echo "Running static analysis..."
	@cppcheck --enable=all --std=c99 *.c

# Format code
format:
	@echo "Formatting code..."
	@clang-format -i *.c *.h

# Package for distribution
package: clean
	@echo "Creating package..."
	@tar -czf contact_book_$(shell date +%Y%m%d).tar.gz *.c *.h Makefile README.md LICENSE

# Show help
help:
	@echo "Contact Book System - Makefile Help"
	@echo "===================================="
	@echo "Available targets:"
	@echo "  all          - Build the application (default)"
	@echo "  clean        - Clean build files"
	@echo "  clean-all    - Clean build files and logs"
	@echo "  install-deps - Install dependencies (Ubuntu/Debian)"
	@echo "  install-deps-rpm - Install dependencies (CentOS/RHEL/Fedora)"
	@echo "  setup-db     - Setup MySQL database"
	@echo "  run          - Build and run the application"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  install      - Install to system"
	@echo "  uninstall    - Remove from system"
	@echo "  test         - Run tests"
	@echo "  docs         - Generate documentation"
	@echo "  memcheck     - Run memory leak check"
	@echo "  analyze      - Run static analysis"
	@echo "  format       - Format source code"
	@echo "  package      - Create distribution package"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all clean clean-all install-deps install-deps-rpm setup-db run debug release install uninstall test docs memcheck analyze format package help directories

# Dependencies
$(BUILD_DIR)/main.o: main.c main.h
$(BUILD_DIR)/database.o: database.c main.h
$(BUILD_DIR)/auth.o: auth.c main.h
$(BUILD_DIR)/utils.o: utils.c main.h	