#include <cmath>
#include <string>

/** @brief What is the TapNote's core type? */
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

/** @brief The list of a TapNote's sub types. */
enum TapNoteSubType
{
	TapNoteSubType_Hold, /**< The start of a traditional hold note. */
	TapNoteSubType_Roll, /**< The start of a roll note that must be hit repeatedly. */
	//TapNoteSubType_Mine,
	NUM_TapNoteSubType,
	TapNoteSubType_Invalid
};

/** @brief The different places a TapNote could come from. */
enum TapNoteSource
{
	TapNoteSource_Original,	/**< This note is part of the original NoteData. */
	TapNoteSource_Addition,	/**< This note is additional note added by a transform. */
	NUM_TapNoteSource,
	TapNoteSource_Invalid
};

/** @brief The various properties of a tap note. */
struct TapNote
{
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
	void Init()
	{
		type = TapNoteType_Empty;
		subType = TapNoteSubType_Invalid;
		source = TapNoteSource_Original;
		fAttackDurationSeconds = 0.f;
		iKeysoundIndex = -1;
		iDuration = 0;
	}
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
	}

	/**
	 * @brief Determine if the two TapNotes are equal to each other.
	 * @param other the other TapNote we're checking.
	 * @return true if the two TapNotes are equal, or false otherwise. */
	bool operator==(const TapNote& other) const
	{
#define COMPARE(x)	if(x!=other.x) return false
		COMPARE(type);
		COMPARE(subType);
		COMPARE(source);
		COMPARE(sAttackModifiers);
		COMPARE(fAttackDurationSeconds);
		COMPARE(iKeysoundIndex);
		COMPARE(iDuration);
#undef COMPARE
		return true;
	}
	/**
	 * @brief Determine if the two TapNotes are not equal to each other.
	 * @param other the other TapNote we're checking.
	 * @return true if the two TapNotes are not equal, or false otherwise. */
	bool operator!=(const TapNote& other) const { return !operator==(other); }
};

extern TapNote TAP_EMPTY;			// '0'
extern TapNote TAP_ORIGINAL_TAP;		// '1'
extern TapNote TAP_ORIGINAL_HOLD_HEAD;		// '2'
extern TapNote TAP_ORIGINAL_ROLL_HEAD;		// '4'
extern TapNote TAP_ORIGINAL_MINE;		// 'M'
extern TapNote TAP_ORIGINAL_LIFT;		// 'L'
extern TapNote TAP_ORIGINAL_ATTACK;		// 'A'
extern TapNote TAP_ORIGINAL_AUTO_KEYSOUND;	// 'K'
extern TapNote TAP_ORIGINAL_FAKE;		// 'F'
//extern TapNote TAP_ORIGINAL_MINE_HEAD;	// 'N' (tentative, we'll see when iDance gets ripped.)
extern TapNote TAP_ADDITION_TAP;
extern TapNote TAP_ADDITION_MINE;

/**
 * @brief The number of tracks allowed.
 *
 * TODO: Don't have a hard-coded track limit.
 */
const int MAX_NOTE_TRACKS = 16;

/**
 * @brief The number of rows per beat.
 *
 * This is a divisor for our "fixed-point" time/beat representation. It must be
 * evenly divisible by 2, 3, and 4, to exactly represent 8th, 12th and 16th notes.
 *
 * XXX: Some other forks try to keep this flexible by putting this in the simfile.
 * Is this a recommended course of action? -Wolfman2000 */
const int ROWS_PER_BEAT = 48;

/** @brief The max number of rows allowed for a Steps pattern. */
const int MAX_NOTE_ROW = (1 << 30);

/** @brief The list of quantized note types allowed at present. */
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

inline float NoteRowToBeat(int iRow) { return iRow / (float)ROWS_PER_BEAT; }

float NoteTypeToBeat(NoteType nt);
