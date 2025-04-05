#ifndef NOTE_TYPES_H
#define NOTE_TYPES_H

#include <cmath>
#include <string>

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

/**
 * @brief Convert the note row to note row (returns itself).
 * @param row the row to convert.
 */
static inline int ToNoteRow(int row) { return row; }


/**
 * @brief Convert the note row to beat.
 * @param row the row to convert.
 */
static inline float ToBeat(int row) { return NoteRowToBeat(row); }


const int MAX_NOTE_ROW = (1 << 30);

enum NoteType
{
	NOTE_TYPE_4TH,	/**< quarter note */
	NOTE_TYPE_8TH,	/**< eighth note */
	NOTE_TYPE_12TH,	/**< quarter note triplet */
	NOTE_TYPE_16TH,	/**< sixteenth note */
	NOTE_TYPE_24TH,	/**< eighth note triplet */
	NOTE_TYPE_32ND,	/**< thirty-second note */
	NOTE_TYPE_48TH, /**< sixteenth note triplet */
	NOTE_TYPE_64TH,	/**< sixty-fourth note */
	NOTE_TYPE_192ND,/**< sixty-fourth note triplet */
	NUM_NoteType,
	NoteType_Invalid
};

enum TapNoteType
{
	TapNoteType_Empty, 		/**< There is no note here. */
	TapNoteType_Tap,		/**< The player simply steps on this. */
	TapNoteType_HoldHead,	/**< This is graded like the Tap type, but should be held. */
	TapNoteType_HoldTail,	/**< In 2sand3s mode, holds are deleted and hold_tail is added. */
	TapNoteType_Mine,		/**< In most modes, it is suggested to not step on these mines. */
	TapNoteType_Lift,		/**< Lift your foot up when it crosses the target area. */
	TapNoteType_Attack,		/**< Hitting this note causes an attack to take place. */
	TapNoteType_AutoKeysound,	/**< A special sound is played when this note crosses the target area. */
	TapNoteType_Fake,		/**< This arrow can't be scored for or against the player. */
	NUM_TapNoteType,
	TapNoteType_Invalid
};


enum TapNoteSubType
{
	TapNoteSubType_Hold, /**< The start of a traditional hold note. */
	TapNoteSubType_Roll, /**< The start of a roll note that must be hit repeatedly. */
	//TapNoteSubType_Mine,
	NUM_TapNoteSubType,
	TapNoteSubType_Invalid
};


enum TapNoteSource
{
	TapNoteSource_Original,	/**< This note is part of the original NoteData. */
	TapNoteSource_Addition,	/**< This note is additional note added by a transform. */
	NUM_TapNoteSource,
	TapNoteSource_Invalid
};


struct TapNote {
	/** @brief The core note type that is about to cross the target area. */
	TapNoteType		type;
	/** @brief The sub type of the note. This is only used if the type is hold_head. */
	TapNoteSubType		subType;
	/** @brief The originating source of the TapNote. */
	TapNoteSource		source;

	// used only if Type == attack:
	std::string		sAttackModifiers;
	float		fAttackDurationSeconds;

	// Index into Song's vector of keysound files if nonnegative:
	int		iKeysoundIndex;

	// also used for hold_head only:
	int		iDuration;

	TapNote() : type(TapNoteType_Empty), subType(TapNoteSubType_Invalid),
		source(TapNoteSource_Original), sAttackModifiers(""),
		fAttackDurationSeconds(0), iKeysoundIndex(-1), iDuration(0) {}
	TapNote(
		TapNoteType type_,
		TapNoteSubType subType_,
		TapNoteSource source_,
		std::string sAttackModifiers_,
		float fAttackDurationSeconds_,
		int iKeysoundIndex_) :
		type(type_), subType(subType_), source(source_),
		sAttackModifiers(sAttackModifiers_),
		fAttackDurationSeconds(fAttackDurationSeconds_),
		iKeysoundIndex(iKeysoundIndex_), iDuration(0)
	{
		if (type_ > TapNoteType_Fake)
		{
			//LOG->Trace("Invalid tap note type %s (most likely) due to random vanish issues. Assume it doesn't need judging.", TapNoteTypeToString(type_).c_str());
			type = TapNoteType_Empty;
		}
	}
};


#endif