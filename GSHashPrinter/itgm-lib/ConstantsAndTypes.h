#ifndef CONSTANTSANDTYPES_H
#define CONSTANTSANDTYPES_H

namespace enums {
	enum DisplayBPM
	{
		DISPLAY_BPM_ACTUAL, /**< Display the song's actual BPM. */
		DISPLAY_BPM_SPECIFIED, /**< Display a specified value or values. */
		DISPLAY_BPM_RANDOM, /**< Display a random selection of BPMs. */
		NUM_DisplayBPM,
		DisplayBPM_Invalid
	};
}

#endif