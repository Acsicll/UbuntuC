# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /data/share/cmakelist/1.1.3/3.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/share/cmakelist/1.1.3/3.1/build

# Include any dependencies generated for this target.
include src/CMakeFiles/MyProject.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/MyProject.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/MyProject.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/MyProject.dir/flags.make

src/CMakeFiles/MyProject.dir/main.c.o: src/CMakeFiles/MyProject.dir/flags.make
src/CMakeFiles/MyProject.dir/main.c.o: ../src/main.c
src/CMakeFiles/MyProject.dir/main.c.o: src/CMakeFiles/MyProject.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/share/cmakelist/1.1.3/3.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/MyProject.dir/main.c.o"
	cd /data/share/cmakelist/1.1.3/3.1/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/CMakeFiles/MyProject.dir/main.c.o -MF CMakeFiles/MyProject.dir/main.c.o.d -o CMakeFiles/MyProject.dir/main.c.o -c /data/share/cmakelist/1.1.3/3.1/src/main.c

src/CMakeFiles/MyProject.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MyProject.dir/main.c.i"
	cd /data/share/cmakelist/1.1.3/3.1/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data/share/cmakelist/1.1.3/3.1/src/main.c > CMakeFiles/MyProject.dir/main.c.i

src/CMakeFiles/MyProject.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MyProject.dir/main.c.s"
	cd /data/share/cmakelist/1.1.3/3.1/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data/share/cmakelist/1.1.3/3.1/src/main.c -o CMakeFiles/MyProject.dir/main.c.s

# Object files for target MyProject
MyProject_OBJECTS = \
"CMakeFiles/MyProject.dir/main.c.o"

# External object files for target MyProject
MyProject_EXTERNAL_OBJECTS =

src/MyProject: src/CMakeFiles/MyProject.dir/main.c.o
src/MyProject: src/CMakeFiles/MyProject.dir/build.make
src/MyProject: src/dir1/libdir1.so
src/MyProject: src/dir2/libdir2.so
src/MyProject: src/CMakeFiles/MyProject.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/share/cmakelist/1.1.3/3.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable MyProject"
	cd /data/share/cmakelist/1.1.3/3.1/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MyProject.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/MyProject.dir/build: src/MyProject
.PHONY : src/CMakeFiles/MyProject.dir/build

src/CMakeFiles/MyProject.dir/clean:
	cd /data/share/cmakelist/1.1.3/3.1/build/src && $(CMAKE_COMMAND) -P CMakeFiles/MyProject.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/MyProject.dir/clean

src/CMakeFiles/MyProject.dir/depend:
	cd /data/share/cmakelist/1.1.3/3.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/share/cmakelist/1.1.3/3.1 /data/share/cmakelist/1.1.3/3.1/src /data/share/cmakelist/1.1.3/3.1/build /data/share/cmakelist/1.1.3/3.1/build/src /data/share/cmakelist/1.1.3/3.1/build/src/CMakeFiles/MyProject.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/MyProject.dir/depend

