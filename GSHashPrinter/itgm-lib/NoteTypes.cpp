#include <iostream>

#include "NoteTypes.h"

//namespace note_type {

	TapNote TAP_EMPTY(TapNoteType_Empty, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	TapNote TAP_ORIGINAL_TAP(TapNoteType_Tap, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	TapNote TAP_ORIGINAL_LIFT(TapNoteType_Lift, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	TapNote TAP_ORIGINAL_HOLD_HEAD(TapNoteType_HoldHead, TapNoteSubType_Hold, TapNoteSource_Original, "", 0, -1);
	TapNote TAP_ORIGINAL_ROLL_HEAD(TapNoteType_HoldHead, TapNoteSubType_Roll, TapNoteSource_Original, "", 0, -1);
	TapNote TAP_ORIGINAL_MINE(TapNoteType_Mine, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	TapNote TAP_ORIGINAL_ATTACK(TapNoteType_Attack, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	TapNote TAP_ORIGINAL_AUTO_KEYSOUND(TapNoteType_AutoKeysound, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	TapNote TAP_ORIGINAL_FAKE(TapNoteType_Fake, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	//TapNote TAP_ORIGINAL_MINE_HEAD ( TapNoteType_HoldHead, TapNoteSubType_Mine, TapNoteSource_Original, "", 0, -1 );
	TapNote TAP_ADDITION_TAP(TapNoteType_Tap, TapNoteSubType_Invalid, TapNoteSource_Addition, "", 0, -1);
	TapNote TAP_ADDITION_MINE(TapNoteType_Mine, TapNoteSubType_Invalid, TapNoteSource_Addition, "", 0, -1);


	float NoteTypeToBeat(NoteType nt)
	{
		switch (nt)
		{
		case NOTE_TYPE_4TH:	return 1.0f;	// quarter notes
		case NOTE_TYPE_8TH:	return 1.0f / 2;	// eighth notes
		case NOTE_TYPE_12TH:	return 1.0f / 3;	// quarter note triplets
		case NOTE_TYPE_16TH:	return 1.0f / 4;	// sixteenth notes
		case NOTE_TYPE_24TH:	return 1.0f / 6;	// eighth note triplets
		case NOTE_TYPE_32ND:	return 1.0f / 8;	// thirty-second notes
		case NOTE_TYPE_48TH:	return 1.0f / 12; // sixteenth note triplets
		case NOTE_TYPE_64TH:	return 1.0f / 16; // sixty-fourth notes
		case NOTE_TYPE_192ND:	return 1.0f / 48; // sixty-fourth note triplets
		case NoteType_Invalid:	return 1.0f / 48;
		default:
			std::cout << "Bad note type!!\n";
			return -1;
		}
	}
//}