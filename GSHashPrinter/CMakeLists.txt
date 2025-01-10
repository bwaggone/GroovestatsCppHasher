# CMakeList.txt : CMake project for GSHashPrinter, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.16)
include("CMakeProject-tomcrypt.cmake")
include("CMakeProject-tommath.cmake")

# Add source to this project's executable.
add_executable (GSHashPrinter
"GSHashPrinter.cpp"
"GSHashPrinter.h"
"itgm-lib/NoteTypes.h" "itgm-lib/NoteTypes.cpp"   "itgm-lib/NotesLoader.cpp" "itgm-lib/NotesLoader.h" "itgm-lib/MsdFile.cpp" "itgm-lib/MsdFile.h"  "itgm-lib/Song.h" "itgm-lib/Song.cpp" "itgm-lib/Steps.h" "itgm-lib/Steps.cpp" "itgm-lib/RageUtil.h" "itgm-lib/RageUtil.cpp")
#set(CMAKE_CXX_STANDARD 17)
target_compile_features(GSHashPrinter PUBLIC cxx_std_17)
target_link_libraries(GSHashPrinter PUBLIC tommath tomcrypt)


  target_compile_definitions(GSHashPrinter PRIVATE LTC_NO_ASM)
#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET GSHashPrinter PROPERTY CXX_STANDARD 20)
#endif()

# TODO: Add tests and install targets if needed.
