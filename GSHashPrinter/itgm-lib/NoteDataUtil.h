#ifndef NOTE_DATA_UTIL_H
#define NOTE_DATA_UTIL_H

#include <string>

#include "NoteData.h"

namespace NoteDataUtil {
	static const int BEATS_PER_MEASURE = 4;
	static const int ROWS_PER_MEASURE = ROWS_PER_BEAT * BEATS_PER_MEASURE;
	
	void LoadFromSMNoteDataString(NoteData& out, const std::string& sSMNoteData_, bool bComposite);
	void InsertHoldTails(NoteData& inout);
	void SplitCompositeNoteData(const NoteData& in, std::vector<NoteData>& out);
	void GetSMNoteDataString(const NoteData& in, std::string& notes_out);
	NoteType GetSmallestNoteTypeForMeasure(const NoteData& nd, int iMeasureIndex);
	NoteType GetSmallestNoteTypeInRange(const NoteData& n, int iStartIndex, int iEndIndex);
}


#endif