#include <algorithm>
#include <iostream>
#include <string>

#include "NoteData.h"
#include "NoteTypes.h"

namespace NoteDataUtil {

	void GetSMNoteDataString(const NoteData& in, std::string& notes_out) {
		/*// Get note data
		std::vector<NoteData> parts;
		float fLastBeat = -1.0f;

		SplitCompositeNoteData(in, parts);

		for (NoteData& nd : parts)
		{
			InsertHoldTails(nd);
			fLastBeat = std::max(fLastBeat, nd.GetLastBeat());
		}

		int iLastMeasure = int(fLastBeat / BEATS_PER_MEASURE);

		notes_out = "";
		int partNum = 0;
		for (NoteData const& nd : parts)
		{
			if (partNum++ != 0)
				notes_out.append("&\n");
			for (int m = 0; m <= iLastMeasure; ++m) // foreach measure
			{
				if (m)
					notes_out.append(1, ',');
				notes_out += "  // measure " + m + '\n';

				NoteType nt = GetSmallestNoteTypeForMeasure(nd, m);
				int iRowSpacing;
				if (nt == NoteType_Invalid)
					iRowSpacing = 1;
				else
					iRowSpacing = std::lrint(NoteTypeToBeat(nt) * ROWS_PER_BEAT);
				// (verify first)
				// iRowSpacing = BeatToNoteRow( NoteTypeToBeat(nt) );

				const int iMeasureStartRow = m * ROWS_PER_MEASURE;
				const int iMeasureLastRow = (m + 1) * ROWS_PER_MEASURE - 1;

				for (int r = iMeasureStartRow; r <= iMeasureLastRow; r += iRowSpacing)
				{
					for (int t = 0; t < nd.GetNumTracks(); ++t)
					{
						const TapNote& tn = nd.GetTapNote(t, r);
						char c;
						switch (tn.type)
						{
						case TapNoteType_Empty:			c = '0'; break;
						case TapNoteType_Tap:			c = '1'; break;
						case TapNoteType_HoldHead:
							switch (tn.subType)
							{
							case TapNoteSubType_Hold:	c = '2'; break;
							case TapNoteSubType_Roll:	c = '4'; break;
								//case TapNoteSubType_Mine:	c = 'N'; break;
							default:
								std::cout << "Invalid tap note subtype: " << tn.subType;
							}
							break;
						case TapNoteType_HoldTail:		c = '3'; break;
						case TapNoteType_Mine:			c = 'M'; break;
						case TapNoteType_Attack:			c = 'A'; break;
						case TapNoteType_AutoKeysound:	c = 'K'; break;
						case TapNoteType_Lift:			c = 'L'; break;
						case TapNoteType_Fake:			c = 'F'; break;
						default:
							c = '\0';
							std::cout << "Invalid tap note type: " << tn.type;
						}
						notes_out.append(1, c);

						if (tn.type == TapNoteType_Attack)
						{
							notes_out.append(ssprintf("{%s:%.2f}", tn.sAttackModifiers.c_str(),
								tn.fAttackDurationSeconds));
						}
						// hey maybe if we have TapNoteType_Item we can do things here.
						if (tn.iKeysoundIndex >= 0)
							notes_out.append(ssprintf("[%d]", tn.iKeysoundIndex));
					}

					notes_out.append(1, '\n');
				}*/


	}
}