# CMakeList.txt : CMake project for GSHashPrinter, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.16)

# Add source to this project's executable.
add_subdirectory(abseil-cpp)
add_executable (GSHashPrinter "GSHashPrinter.cpp" "GSHashPrinter.h" "itgm-lib/NoteTypes.h" "itgm-lib/NoteTypes.cpp" "itgm-lib/NoteData.cpp" "itgm-lib/NoteData.h")
target_link_libraries(GSHashPrinter absl::strings absl::flags)

set(CMAKE_CXX_STANDARD 17)

#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET GSHashPrinter PROPERTY CXX_STANDARD 20)
#endif()

# TODO: Add tests and install targets if needed.
