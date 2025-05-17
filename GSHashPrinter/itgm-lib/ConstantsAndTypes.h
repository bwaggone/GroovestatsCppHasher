#ifndef CONSTANTSANDTYPES_H
#define CONSTANTSANDTYPES_H

#include <unordered_map>

const int kMaxStepsDescriptionLength = 255;
const static float kStepFileVersionNumber = 0.83f;


enum DisplayBPM
{
	DISPLAY_BPM_ACTUAL, /**< Display the song's actual BPM. */
	DISPLAY_BPM_SPECIFIED, /**< Display a specified value or values. */
	DISPLAY_BPM_RANDOM, /**< Display a random selection of BPMs. */
	NUM_DisplayBPM,
	DisplayBPM_Invalid
};

enum StepsType
{
	StepsType_dance_single = 0,
	StepsType_dance_double,
	StepsType_dance_couple,
	StepsType_dance_solo,
	StepsType_dance_threepanel,
	StepsType_dance_routine,
	StepsType_pump_single,
	StepsType_pump_halfdouble,
	StepsType_pump_double,
	StepsType_pump_couple,
	StepsType_pump_routine,
	StepsType_kb7_single,
	StepsType_ez2_single,
	StepsType_ez2_double,
	StepsType_ez2_real,
	StepsType_para_single,
	StepsType_ds3ddx_single,
	StepsType_beat_single5,
	StepsType_beat_versus5,
	StepsType_beat_double5,
	StepsType_beat_single7,
	StepsType_beat_versus7,
	StepsType_beat_double7,
	StepsType_maniax_single,
	StepsType_maniax_double,
	StepsType_techno_single4,
	StepsType_techno_single5,
	StepsType_techno_single8,
	StepsType_techno_double4,
	StepsType_techno_double5,
	StepsType_techno_double8,
	StepsType_popn_five,
	StepsType_popn_nine,
	StepsType_lights_cabinet,
	StepsType_kickbox_human,
	StepsType_kickbox_quadarm,
	StepsType_kickbox_insect,
	StepsType_kickbox_arachnid,
	NUM_StepsType,		// leave this at the end
	StepsType_Invalid,
};


enum StepsTypeCategory
{
	StepsTypeCategory_Single, /**< One person plays on one side. */
	StepsTypeCategory_Double, /**< One person plays on both sides. */
	StepsTypeCategory_Couple, /**< Two players play on their own side. */
	StepsTypeCategory_Routine, /**< Two players share both sides together. */
};

struct StepsTypeInfo
{
	const char* szName;
	/** @brief The number of tracks, or columns, of this type. */
	int iNumTracks;
	/** @brief A flag to determine if we allow this type to be autogen'ed to other types. */
	bool bAllowAutogen;
	/** @brief The most basic StyleType that this StpesTypeInfo is used with. */
	StepsTypeCategory m_StepsTypeCategory;
};

static const StepsTypeInfo kStepTypeInfos[] = {
	// dance
	{ "dance-single",	4,	true,	StepsTypeCategory_Single },
	{ "dance-double",	8,	true,	StepsTypeCategory_Double },
	{ "dance-couple",	8,	true,	StepsTypeCategory_Couple },
	{ "dance-solo",		6,	true,	StepsTypeCategory_Single },
	{ "dance-threepanel",	3,	true,	StepsTypeCategory_Single }, // thanks to kurisu
	{ "dance-routine",	8,	false,	StepsTypeCategory_Routine },
};

static std::unordered_map<std::string, StepsType> kStringToStepsType = {
	{ "dance-single", StepsType_dance_single},
	{ "dance-double", StepsType_dance_double},
	{ "dance-couple", StepsType_dance_couple},
	{ "dance-solo", StepsType_dance_solo},
	{ "dance-threepanel", StepsType_dance_threepanel},
	{ "dance-routine", StepsType_dance_routine},
};
static std::unordered_map<StepsType, std::string> kStepsTypeToString = {
	{ StepsType_dance_single, "dance-single" },
	{ StepsType_dance_double, "dance-double" },
	{ StepsType_dance_couple, "dance-couple" },
	{ StepsType_dance_solo, "dance-solo" },
	{ StepsType_dance_threepanel, "dance-threepanel" },
	{ StepsType_dance_routine, "dance-routine" }
};


enum Difficulty
{
	Difficulty_Beginner,
	Difficulty_Easy,
	Difficulty_Medium,
	Difficulty_Hard,
	Difficulty_Challenge,
	Difficulty_Edit,
	NUM_Difficulty,
	Difficulty_Invalid
};

static std::unordered_map<std::string, Difficulty> kStringToDifficulty = {
{ "beginner", Difficulty_Beginner},
{ "easy", Difficulty_Easy},
{ "medium", Difficulty_Medium},
{ "hard", Difficulty_Hard},
{ "challenge", Difficulty_Challenge},
{ "edit", Difficulty_Edit},
};

static std::unordered_map<Difficulty, std::string> kDifficultyToString = {
{ Difficulty_Beginner, "beginner" },
{ Difficulty_Easy, "easy" },
{ Difficulty_Medium, "medium" },
{ Difficulty_Hard, "hard" },
{ Difficulty_Challenge, "challenge" },
{ Difficulty_Edit, "edit" }
};


enum RadarCategory
{
	RadarCategory_Stream = 0, /**< How much stream is in the song? */
	RadarCategory_Voltage, /**< How much voltage is in the song? */
	RadarCategory_Air, /**< How much air is in the song? */
	RadarCategory_Freeze, /**< How much freeze (holds) is in the song? */
	RadarCategory_Chaos, /**< How much chaos is in the song? */
	RadarCategory_Notes, /**< How many notes are in the song? */
	RadarCategory_TapsAndHolds, /**< How many taps and holds are in the song? */
	RadarCategory_Jumps, /**< How many jumps are in the song? */
	RadarCategory_Holds, /**< How many holds are in the song? */
	RadarCategory_Mines, /**< How many mines are in the song? */
	RadarCategory_Hands, /**< How many hands are in the song? */
	RadarCategory_Rolls, /**< How many rolls are in the song? */
	RadarCategory_Lifts, /**< How many lifts are in the song? */
	RadarCategory_Fakes, /**< How many fakes are in the song? */
	// If you add another radar category, make sure you update
	// NoteDataUtil::CalculateRadarValues to calculate it.
	// Also update NoteDataWithScoring::GetActualRadarValues. -Kyz
	NUM_RadarCategory,	/**< The number of radar categories. */
	RadarCategory_Invalid
};

enum PlayerNumber
{
	PLAYER_1 = 0,
	PLAYER_2,
	NUM_PlayerNumber,	// leave this at the end
	PlayerNumber_Invalid
};


#endif