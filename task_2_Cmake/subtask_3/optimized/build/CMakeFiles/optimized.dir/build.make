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
CMAKE_SOURCE_DIR = /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/build

# Include any dependencies generated for this target.
include CMakeFiles/optimized.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/optimized.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/optimized.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/optimized.dir/flags.make

CMakeFiles/optimized.dir/optimized.cpp.o: CMakeFiles/optimized.dir/flags.make
CMakeFiles/optimized.dir/optimized.cpp.o: ../optimized.cpp
CMakeFiles/optimized.dir/optimized.cpp.o: CMakeFiles/optimized.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/optimized.dir/optimized.cpp.o"
	/opt/nvidia/hpc_sdk/Linux_x86_64/23.11/compilers/bin/nvc++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/optimized.dir/optimized.cpp.o -MF CMakeFiles/optimized.dir/optimized.cpp.o.d -o CMakeFiles/optimized.dir/optimized.cpp.o -c /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/optimized.cpp

CMakeFiles/optimized.dir/optimized.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/optimized.dir/optimized.cpp.i"
	/opt/nvidia/hpc_sdk/Linux_x86_64/23.11/compilers/bin/nvc++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/optimized.cpp > CMakeFiles/optimized.dir/optimized.cpp.i

CMakeFiles/optimized.dir/optimized.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/optimized.dir/optimized.cpp.s"
	/opt/nvidia/hpc_sdk/Linux_x86_64/23.11/compilers/bin/nvc++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/optimized.cpp -o CMakeFiles/optimized.dir/optimized.cpp.s

# Object files for target optimized
optimized_OBJECTS = \
"CMakeFiles/optimized.dir/optimized.cpp.o"

# External object files for target optimized
optimized_EXTERNAL_OBJECTS =

optimized: CMakeFiles/optimized.dir/optimized.cpp.o
optimized: CMakeFiles/optimized.dir/build.make
optimized: CMakeFiles/optimized.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable optimized"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/optimized.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/optimized.dir/build: optimized
.PHONY : CMakeFiles/optimized.dir/build

CMakeFiles/optimized.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/optimized.dir/cmake_clean.cmake
.PHONY : CMakeFiles/optimized.dir/clean

CMakeFiles/optimized.dir/depend:
	cd /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/build /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/build /home/g.zhigalov/Parallels-NSU/task_2_Cmake/subtask_3/optimized/build/CMakeFiles/optimized.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/optimized.dir/depend

