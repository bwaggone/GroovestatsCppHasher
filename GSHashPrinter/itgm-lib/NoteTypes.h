#ifndef NOTE_TYPES_H
#define NOTE_TYPES_H

#include <cmath>

const int ROWS_PER_BEAT = 48;

/**
 * @brief Convert the beat into a note row.
 * @param fBeatNum the beat to convert.
 * @return the note row. */
inline int   BeatToNoteRow(float fBeatNum) { return std::lrint(fBeatNum * ROWS_PER_BEAT); }	// round


/**
 * @brief Convert the note row to a beat.
 * @param iRow the row to convert.
 * @return the beat. */
inline float NoteRowToBeat(int iRow) { return iRow / (float)ROWS_PER_BEAT; }

/**
 * @brief Convert the beat to note row.
 * @param beat the beat to convert.
 */
static inline int ToNoteRow(float beat) { return BeatToNoteRow(beat); }

#endif