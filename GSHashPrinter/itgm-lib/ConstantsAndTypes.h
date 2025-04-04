#ifndef CONSTANTSANDTYPES_H
#define CONSTANTSANDTYPES_H

#include <unordered_map>

const int kMaxStepsDescriptionLength = 255;
namespace enums {
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

	static std::unordered_map<std::string, StepsType> kStringToStepsType = {
		{ "dance-single", StepsType_dance_single},
	    { "dance-double", StepsType_dance_double},
		{ "dance-couple", StepsType_dance_couple},
		{ "dance-solo", StepsType_dance_solo},
		{ "dance-threepanel", StepsType_dance_threepanel},
		{ "dance-routine", StepsType_dance_routine},
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

}

#endif