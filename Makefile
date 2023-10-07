# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/linuxbrew/.linuxbrew/Cellar/cmake/3.27.4/bin/cmake

# The command to remove a file.
RM = /home/linuxbrew/.linuxbrew/Cellar/cmake/3.27.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/daniel/Dev/opal

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/daniel/Dev/opal

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake cache editor..."
	/home/linuxbrew/.linuxbrew/Cellar/cmake/3.27.4/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake to regenerate build system..."
	/home/linuxbrew/.linuxbrew/Cellar/cmake/3.27.4/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/daniel/Dev/opal/CMakeFiles /home/daniel/Dev/opal//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/daniel/Dev/opal/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -P /home/daniel/Dev/opal/CMakeFiles/VerifyGlobs.cmake
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named opal

# Build rule for target.
opal: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 opal
.PHONY : opal

# fast build rule for target.
opal/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/build
.PHONY : opal/fast

src/interpreter.o: src/interpreter.cpp.o
.PHONY : src/interpreter.o

# target to build an object file
src/interpreter.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/interpreter.cpp.o
.PHONY : src/interpreter.cpp.o

src/interpreter.i: src/interpreter.cpp.i
.PHONY : src/interpreter.i

# target to preprocess a source file
src/interpreter.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/interpreter.cpp.i
.PHONY : src/interpreter.cpp.i

src/interpreter.s: src/interpreter.cpp.s
.PHONY : src/interpreter.s

# target to generate assembly for a file
src/interpreter.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/interpreter.cpp.s
.PHONY : src/interpreter.cpp.s

src/lexer.o: src/lexer.cpp.o
.PHONY : src/lexer.o

# target to build an object file
src/lexer.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/lexer.cpp.o
.PHONY : src/lexer.cpp.o

src/lexer.i: src/lexer.cpp.i
.PHONY : src/lexer.i

# target to preprocess a source file
src/lexer.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/lexer.cpp.i
.PHONY : src/lexer.cpp.i

src/lexer.s: src/lexer.cpp.s
.PHONY : src/lexer.s

# target to generate assembly for a file
src/lexer.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/lexer.cpp.s
.PHONY : src/lexer.cpp.s

src/main.o: src/main.cpp.o
.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i
.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s
.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

src/parser.o: src/parser.cpp.o
.PHONY : src/parser.o

# target to build an object file
src/parser.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/parser.cpp.o
.PHONY : src/parser.cpp.o

src/parser.i: src/parser.cpp.i
.PHONY : src/parser.i

# target to preprocess a source file
src/parser.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/parser.cpp.i
.PHONY : src/parser.cpp.i

src/parser.s: src/parser.cpp.s
.PHONY : src/parser.s

# target to generate assembly for a file
src/parser.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/opal.dir/build.make CMakeFiles/opal.dir/src/parser.cpp.s
.PHONY : src/parser.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... opal"
	@echo "... src/interpreter.o"
	@echo "... src/interpreter.i"
	@echo "... src/interpreter.s"
	@echo "... src/lexer.o"
	@echo "... src/lexer.i"
	@echo "... src/lexer.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/parser.o"
	@echo "... src/parser.i"
	@echo "... src/parser.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -P /home/daniel/Dev/opal/CMakeFiles/VerifyGlobs.cmake
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

