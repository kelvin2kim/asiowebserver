# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.29.2/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.29.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kelvin/Documents/asiowebserver/devices/volunteer1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kelvin/Documents/asiowebserver/devices/volunteer1/volunteer1exec

# Include any dependencies generated for this target.
include CMakeFiles/volunteer1.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/volunteer1.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/volunteer1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/volunteer1.dir/flags.make

CMakeFiles/volunteer1.dir/main.cpp.o: CMakeFiles/volunteer1.dir/flags.make
CMakeFiles/volunteer1.dir/main.cpp.o: /Users/kelvin/Documents/asiowebserver/devices/volunteer1/main.cpp
CMakeFiles/volunteer1.dir/main.cpp.o: CMakeFiles/volunteer1.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/kelvin/Documents/asiowebserver/devices/volunteer1/volunteer1exec/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/volunteer1.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/volunteer1.dir/main.cpp.o -MF CMakeFiles/volunteer1.dir/main.cpp.o.d -o CMakeFiles/volunteer1.dir/main.cpp.o -c /Users/kelvin/Documents/asiowebserver/devices/volunteer1/main.cpp

CMakeFiles/volunteer1.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/volunteer1.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/kelvin/Documents/asiowebserver/devices/volunteer1/main.cpp > CMakeFiles/volunteer1.dir/main.cpp.i

CMakeFiles/volunteer1.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/volunteer1.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/kelvin/Documents/asiowebserver/devices/volunteer1/main.cpp -o CMakeFiles/volunteer1.dir/main.cpp.s

# Object files for target volunteer1
volunteer1_OBJECTS = \
"CMakeFiles/volunteer1.dir/main.cpp.o"

# External object files for target volunteer1
volunteer1_EXTERNAL_OBJECTS =

volunteer1: CMakeFiles/volunteer1.dir/main.cpp.o
volunteer1: CMakeFiles/volunteer1.dir/build.make
volunteer1: /opt/homebrew/lib/libboost_thread-mt.dylib
volunteer1: /opt/homebrew/lib/libboost_json-mt.dylib
volunteer1: /opt/homebrew/lib/libboost_container-mt.dylib
volunteer1: /opt/homebrew/Cellar/libpqxx/7.9.0/lib/libpqxx.dylib
volunteer1: CMakeFiles/volunteer1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/kelvin/Documents/asiowebserver/devices/volunteer1/volunteer1exec/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable volunteer1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/volunteer1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/volunteer1.dir/build: volunteer1
.PHONY : CMakeFiles/volunteer1.dir/build

CMakeFiles/volunteer1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/volunteer1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/volunteer1.dir/clean

CMakeFiles/volunteer1.dir/depend:
	cd /Users/kelvin/Documents/asiowebserver/devices/volunteer1/volunteer1exec && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kelvin/Documents/asiowebserver/devices/volunteer1 /Users/kelvin/Documents/asiowebserver/devices/volunteer1 /Users/kelvin/Documents/asiowebserver/devices/volunteer1/volunteer1exec /Users/kelvin/Documents/asiowebserver/devices/volunteer1/volunteer1exec /Users/kelvin/Documents/asiowebserver/devices/volunteer1/volunteer1exec/CMakeFiles/volunteer1.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/volunteer1.dir/depend

