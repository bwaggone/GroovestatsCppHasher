#ifndef NOTE_DATA_UTIL_H
#define NOTE_DATA_UTIL_H

#include <string>

#include "NoteData.h"

namespace NoteDataUtil {
	static const int BEATS_PER_MEASURE = 4;
	static const int ROWS_PER_MEASURE = ROWS_PER_BEAT * BEATS_PER_MEASURE;

	void GetSMNoteDataString(const NoteData& in, std::string& notes_out);
}


#endif