// GSHashPrinter.cpp : Defines the entry point for the application.
//

#include <cmath>
#include <cstdio>
#include <stdio.h>
#include <vector>

#include "GSHashPrinter.h"

#include "itgm-lib/NoteData.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_join.h"

using namespace std;

ABSL_FLAG(std::string, songs_path, "", "The path to the songs folder, which will be recursively iterated through");
ABSL_FLAG(std::string, output_path, "/output", "The output path for the hash analysis");
ABSL_FLAG(std::string, delimiter, ",", "The delimiter for the hash and chart information. Defaults to comma.");
ABSL_FLAG(std::string, entry_delimeter, "\n", "The delimiter between whole entries. Defaults to newline.");

// Ripped from NoteDataUtil.cpp
static const int BEATS_PER_MEASURE = 4;
//static const int ROWS_PER_BEAT = 48;
static const int ROWS_PER_MEASURE = ROWS_PER_BEAT * BEATS_PER_MEASURE;

// Some jank enum code.
template<typename T>
static inline void enum_add(T& val, int iAmt)
{
	val = static_cast<T>(val + iAmt);
}
#define FOREACH_ENUM( e, var )	for( e var=(e)0; var<NUM_##e; enum_add<e>( var, +1 ) )

void SplitCompositeNoteData(NoteData in, std::vector<NoteData> in2) {
	return;
}

void InsertHoldTails(NoteData in) {
	return;
}

NoteType GetSmallestNoteTypeInRange(const NoteData& n, int iStartIndex, int iEndIndex)
{
	// probe to find the smallest note type
	FOREACH_ENUM(NoteType, nt)
	{
		float fBeatSpacing = NoteTypeToBeat(nt);
		int iRowSpacing = std::lrint(fBeatSpacing * ROWS_PER_BEAT);

		bool bFoundSmallerNote = false;
		// for each index in this measure
		FOREACH_NONEMPTY_ROW_ALL_TRACKS_RANGE(n, i, iStartIndex, iEndIndex)
		{
			if (i % iRowSpacing == 0)
				continue;	// skip

			if (!n.IsRowEmpty(i))
			{
				bFoundSmallerNote = true;
				break;
			}
		}

		if (bFoundSmallerNote)
			continue;	// searching the next NoteType
		else
			return nt;	// stop searching. We found the smallest NoteType
	}
	return NoteType_Invalid;	// well-formed notes created in the editor should never get here
}

NoteType GetSmallestNoteTypeForMeasure(const NoteData& nd, int iMeasureIndex)
{
	const int iMeasureStartIndex = iMeasureIndex * ROWS_PER_MEASURE;
	const int iMeasureEndIndex = (iMeasureIndex + 1) * ROWS_PER_MEASURE;

	return GetSmallestNoteTypeInRange(nd, iMeasureStartIndex, iMeasureEndIndex);
}

void GetSMNoteDataString(const NoteData& in, std::string& sRet)
{
	// Get note data
	std::vector<NoteData> parts;
	float fLastBeat = -1.0f;

	SplitCompositeNoteData(in, parts);

	for (NoteData& nd : parts)
	{
		InsertHoldTails(nd);
		fLastBeat = std::max(fLastBeat, nd.GetLastBeat());
	}

	int iLastMeasure = int(fLastBeat / BEATS_PER_MEASURE);

	sRet = "";
	int partNum = 0;
	for (NoteData const& nd : parts)
	{
		if (partNum++ != 0)
			sRet.append("&\n");
		for (int m = 0; m <= iLastMeasure; ++m) // foreach measure
		{
			if (m)
				sRet.append(1, ',');
			sRet.append("  // measure ");
			sRet.append(std::to_string(m));
			sRet.append("\n");

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
							std::cout << "Invalid tap note subtype: " << tn.subType << "\n";
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
						std::cout << "Invalid tap note type: " << tn.type << "\n";
					}
					sRet.append(1, c);

					if (tn.type == TapNoteType_Attack)
					{
						sRet.append(ssprintf("{%s:%.2f}", tn.sAttackModifiers.c_str(),
							tn.fAttackDurationSeconds));
					}
					// hey maybe if we have TapNoteType_Item we can do things here.
					if (tn.iKeysoundIndex >= 0)
						sRet.append(ssprintf("[%d]", tn.iKeysoundIndex));
				}

				sRet.append(1, '\n');
			}
		}
	}
}

// Ripped from Steps.cpp
std::string MinimizedChartString()
{
	// We can potentially minimize the chart to get the most compressed
	// form of the actual chart data.
	// NOTE(teejusb): This can be more compressed than the data actually
	// generated by StepMania. This is okay because the charts would still
	// be considered equivalent.
	// E.g. 0000                      0000
	// 		0000  -- minimized to -->
	// 		0000
	// 		0000
	// 		StepMania will always generate the former since quarter notes are
	// 		the smallest quantization.

	std::string smNoteData = "";

	this->GetSMNoteData(smNoteData);
	if (smNoteData == "")
	{
		return "";
	}
	std::string minimizedNoteData = "";

	std::vector<std::string> measures;
	Regex anyNote("[^0]");

	split(smNoteData, ",", measures, true);
	for (unsigned m = 0; m < measures.size(); m++)
	{

		Trim(measures[m]);
		bool isEmpty = true;
		bool allZeroes = true;
		bool minimal = false;
		std::vector<std::string> lines;
		split(measures[m], "\n", lines, true);
		while (!minimal && lines.size() % 2 == 0)
		{
			// If every other line is all 0s, we can minimize the measure
			for (unsigned i = 1; i < lines.size(); i += 2)
			{
				Trim(lines[i]);
				if (anyNote.Compare(lines[i]) == true)
				{
					allZeroes = false;
					break;
				}
			}

			if (allZeroes)
			{
				// Iterate through lines, removing every other item.
				// Note that we're removing the odd indices, so we
				// call `++it;` and then erase the following line
				auto it = lines.begin();
				while (it != lines.end())
				{
					++it;
					if (it != lines.end())
					{
						it = lines.erase(it);
					}
				}
			}
			else
			{
				minimal = true;
			}
		}

		// Once the measure has been minimized, make sure all of the lines are
		// actually trimmed.
		// (for some reason, the chart returned by GetSMNoteData() have a lot
		//  of extra newlines)
		for (unsigned l = 0; l < lines.size(); l++)
		{
			Trim(lines[l]);
		}

		// Then, rejoin the lines together to make a measure,
		// and add it to minimizedNoteData.
		minimizedNoteData += join("\n", lines);
		if (m < measures.size() - 1)
		{
			minimizedNoteData += "\n,\n";
		}
	}
	return minimizedNoteData;
}

int main()
{
	std::vector<std::string> tmp = { "foo", "bar" };
	cout << "Hello CMake." << absl::StrJoin(tmp, ",") << absl::GetFlag(FLAGS_songs_path);
	return 0;
}
