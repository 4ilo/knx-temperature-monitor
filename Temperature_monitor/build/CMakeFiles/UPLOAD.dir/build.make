# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor/build

# Utility rule file for UPLOAD.

# Include the progress variables for this target.
include CMakeFiles/UPLOAD.dir/progress.make

CMakeFiles/UPLOAD:
	arm-none-eabi-gdb -iex target\ remote\ tcp:127.0.0.1:3333 -iex monitor\ program\ /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor/build/Temperature_monitor.elf -iex monitor\ reset\ init -iex disconnect -iex quit

UPLOAD: CMakeFiles/UPLOAD
UPLOAD: CMakeFiles/UPLOAD.dir/build.make

.PHONY : UPLOAD

# Rule to build all files generated by this target.
CMakeFiles/UPLOAD.dir/build: UPLOAD

.PHONY : CMakeFiles/UPLOAD.dir/build

CMakeFiles/UPLOAD.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/UPLOAD.dir/cmake_clean.cmake
.PHONY : CMakeFiles/UPLOAD.dir/clean

CMakeFiles/UPLOAD.dir/depend:
	cd /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor/build /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor/build /home/olivier/code/projects/knx_temperature_monitor/Temperature_monitor/build/CMakeFiles/UPLOAD.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/UPLOAD.dir/depend

