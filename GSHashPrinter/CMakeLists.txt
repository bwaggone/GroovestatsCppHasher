# CMakeList.txt : CMake project for GSHashPrinter, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.16)
include("CMakeProject-tomcrypt.cmake")
include("CMakeProject-tommath.cmake")

# Add source to this project's executable.
add_executable (GSHashPrinter
"GSHashPrinter.cpp"
    "itgm-lib/NotesLoader.cpp" "itgm-lib/NotesLoader.h" "itgm-lib/MsdFile.cpp" "itgm-lib/MsdFile.h"  "itgm-lib/Song.h" "itgm-lib/Song.cpp" "itgm-lib/Steps.h" "itgm-lib/Steps.cpp" "itgm-lib/RageUtil.h" "itgm-lib/RageUtil.cpp" "itgm-lib/TimingSegment.h" "itgm-lib/TimingSegment.cpp" "itgm-lib/TimingData.h" "itgm-lib/TimingData.cpp" "itgm-lib/ConstantsAndTypes.h" "itgm-lib/NotesLoaderSSC.h" "itgm-lib/NotesLoaderSSC.cpp" "itgm-lib/NotesLoaderSM.h" "itgm-lib/NotesLoaderSM.cpp" "itgm-lib/NoteTypes.h" "itgm-lib/BackgroundUtil.h" "itgm-lib/Attack.h" "itgm-lib/NoteData.h" "itgm-lib/NoteData.cpp" "itgm-lib/NoteDataUtil.h" "itgm-lib/NoteDataUtil.cpp" "itgm-lib/NoteTypes.cpp")
target_compile_features(GSHashPrinter PUBLIC cxx_std_17)
target_link_libraries(GSHashPrinter PUBLIC tommath tomcrypt)


 target_compile_definitions(GSHashPrinter PRIVATE LTC_NO_ASM)

