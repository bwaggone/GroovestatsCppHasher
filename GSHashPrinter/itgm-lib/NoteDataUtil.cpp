#include <algorithm>
#include <iostream>
#include <string>

#include "NoteData.h"
#include "NoteTypes.h"
#include "RageUtil.h"

namespace NoteDataUtil {

	static const int BEATS_PER_MEASURE = 4;
	static const int ROWS_PER_MEASURE = ROWS_PER_BEAT * BEATS_PER_MEASURE;

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

	static void LoadFromSMNoteDataStringWithPlayer(NoteData& out, const std::string& sSMNoteData, int start,
		int len, PlayerNumber pn, int iNumTracks)
	{
		/* Don't allocate memory for the entire string, nor per measure. Instead, use the in-place
		 * partial string split twice. By maintaining begin and end pointers to each measure line
		 * we can perform this without copying the string at all. */
		int size = -1;
		const int end = start + len;
		std::vector<std::pair<const char*, const char*> > aMeasureLines;
		for (unsigned m = 0; true; ++m)
		{
			/* XXX Ignoring empty seems wrong for measures. It means that ",,," is treated as
			 * "," where I would expect most people would want 2 empty measures. ",\n,\n,"
			 * would do as I would expect. */
			util::split(sSMNoteData, ",", start, size, end, true); // Ignore empty is important.
			if (start == end)
			{
				break;
			}
			// Partial string split.
			int measureLineStart = start, measureLineSize = -1;
			const int measureEnd = start + size;

			aMeasureLines.clear();
			for (;;)
			{
				// Ignore empty is clearly important here.
				util::split(sSMNoteData, "\n", measureLineStart, measureLineSize, measureEnd, true);
				if (measureLineStart == measureEnd)
				{
					break;
				}
				//RString &line = sSMNoteData.substr( measureLineStart, measureLineSize );
				const char* beginLine = sSMNoteData.data() + measureLineStart;
				const char* endLine = beginLine + measureLineSize;

				while (beginLine < endLine && strchr("\r\n\t ", *beginLine))
					++beginLine;
				while (endLine > beginLine && strchr("\r\n\t ", *(endLine - 1)))
					--endLine;
				if (beginLine < endLine) // nonempty
					aMeasureLines.push_back(std::pair<const char*, const char*>(beginLine, endLine));
			}

			for (unsigned l = 0; l < aMeasureLines.size(); l++)
			{
				const char* p = aMeasureLines[l].first;
				const char* const beginLine = p;
				const char* const endLine = aMeasureLines[l].second;

				const float fPercentIntoMeasure = l / (float)aMeasureLines.size();
				const float fBeat = (m + fPercentIntoMeasure) * BEATS_PER_MEASURE;
				const int iIndex = BeatToNoteRow(fBeat);

				int iTrack = 0;
				while (iTrack < iNumTracks && p < endLine)
				{
					TapNote tn;
					char ch = *p;

					switch (ch)
					{
					case '0': tn = TAP_EMPTY;				break;
					case '1': tn = TAP_ORIGINAL_TAP;			break;
					case '2':
					case '4':
						// case 'N': // minefield
						tn = ch == '2' ? TAP_ORIGINAL_HOLD_HEAD : TAP_ORIGINAL_ROLL_HEAD;
						/*
						// upcoming code for minefields -aj
						switch(ch)
						{
						case '2': tn = TAP_ORIGINAL_HOLD_HEAD; break;
						case '4': tn = TAP_ORIGINAL_ROLL_HEAD; break;
						case 'N': tn = TAP_ORIGINAL_MINE_HEAD; break;
						}
						*/

						/* Set the hold note to have infinite length. We'll clamp
						 * it when we hit the tail. */
						tn.iDuration = MAX_NOTE_ROW;
						break;
					case '3':
					{
						// This is the end of a hold. Search for the beginning.
						int iHeadRow;
						if (!out.IsHoldNoteAtRow(iTrack, iIndex, &iHeadRow))
						{
							int n = intptr_t(endLine) - intptr_t(beginLine);
							std::cout << "Unmatched 3 in" << n << " " << beginLine << "\n";
						}
						else
						{
							out.FindTapNote(iTrack, iHeadRow)->second.iDuration = iIndex - iHeadRow;
						}

						// This won't write tn, but keep parsing normally anyway.
						break;
					}
					//				case 'm':
					// Don't be loose with the definition.  Use only 'M' since
					// that's what we've been writing to disk.  -Chris
					case 'M': tn = TAP_ORIGINAL_MINE;			break;
						// case 'A': tn = TAP_ORIGINAL_ATTACK;			break;
					case 'K': tn = TAP_ORIGINAL_AUTO_KEYSOUND;		break;
					case 'L': tn = TAP_ORIGINAL_LIFT;			break;
					case 'F': tn = TAP_ORIGINAL_FAKE;			break;
						// case 'I': tn = TAP_ORIGINAL_ITEM;			break;
					default:
						/* Invalid data. We don't want to assert, since there might
						 * simply be invalid data in an .SM, and we don't want to die
						 * due to invalid data. We should probably check for this when
						 * we load SM data for the first time ... */
						 // FAIL_M("Invalid data in SM");
						tn = TAP_EMPTY;
						break;
					}

					p++;
					// We won't scan past the end of the line so these are safe to do.
#if 0
				// look for optional attack info (e.g. "{tipsy,50% drunk:15.2}")
					if (*p == '{')
					{
						p++;

						char szModifiers[256] = "";
						float fDurationSeconds = 0;
						if (sscanf(p, "%255[^:]:%f}", szModifiers, &fDurationSeconds) == 2)	// not fatal if this fails due to malformed data
						{
							tn.type = TapNoteType_Attack;
							tn.sAttackModifiers = szModifiers;
							tn.fAttackDurationSeconds = fDurationSeconds;
						}

						// skip past the '}'
						while (p < endLine)
						{
							if (*(p++) == '}')
								break;
						}
					}
#endif

					// look for optional keysound index (e.g. "[123]")
					if (*p == '[')
					{
						p++;
						int iKeysoundIndex = 0;
						if (1 == sscanf(p, "%d]", &iKeysoundIndex))	// not fatal if this fails due to malformed data
							tn.iKeysoundIndex = iKeysoundIndex;

						// skip past the ']'
						while (p < endLine)
						{
							if (*(p++) == ']')
								break;
						}
					}

#if 0
					// look for optional item name (e.g. "<potion>"),
					// where the name in the <> is a Lua function defined elsewhere
					// (Data/ItemTypes.lua, perhaps?) -aj
					if (*p == '<')
					{
						p++;

						// skip past the '>'
						while (p < endLine)
						{
							if (*(p++) == '>')
								break;
						}
					}
#endif

					/* Optimization: if we pass TAP_EMPTY, NoteData will do a search
					 * to remove anything in this position.  We know that there's nothing
					 * there, so avoid the search. */
					if (tn.type != TapNoteType_Empty && ch != '3')
					{
						tn.pn = pn;
						out.SetTapNote(iTrack, iIndex, tn);
					}

					iTrack++;
				}
			}
		}

		// Make sure we don't have any hold notes that didn't find a tail.
		for (int t = 0; t < out.GetNumTracks(); t++)
		{
			NoteData::iterator begin = out.begin(t);
			NoteData::iterator lEnd = out.end(t);
			while (begin != lEnd)
			{
				NoteData::iterator next = Increment(begin);
				const TapNote& tn = begin->second;
				if (tn.type == TapNoteType_HoldHead && tn.iDuration == MAX_NOTE_ROW)
				{
					int iRow = begin->first;
					std::cout << "While loading .sm/.ssc note data, there was an unmatched 2 at beat " << NoteRowToBeat(iRow) << "\n";
					out.RemoveTapNote(t, begin);
				}

				begin = next;
			}
		}
		out.RevalidateATIs(std::vector<int>(), false);
	}

	void SplitCompositeNoteData(const NoteData& in, std::vector<NoteData>& out)
	{
		if (!in.IsComposite())
		{
			out.push_back(in);
			return;
		}

		// TODO (maybe): The following is for routine charts only
		/*FOREACH_PlayerNumber(pn)
		{
			out.push_back(NoteData());
			out.back().SetNumTracks(in.GetNumTracks());
		}

		for (int t = 0; t < in.GetNumTracks(); ++t)
		{
			for (NoteData::const_iterator iter = in.begin(t); iter != in.end(t); ++iter)
			{
				int row = iter->first;
				TapNote tn = iter->second;
				const Style* curStyle = GAMESTATE->GetCurrentStyle(PLAYER_INVALID);
				if ((curStyle == nullptr || curStyle->m_StyleType == StyleType_TwoPlayersSharedSides)
					&& int(tn.pn) > NUM_PlayerNumber)
				{
					tn.pn = PLAYER_1;
				}
				unsigned index = int(tn.pn);

				ASSERT_M(index < NUM_PlayerNumber, ssprintf("We have a note not assigned to a player. The note in question is on beat %f, column %i.", NoteRowToBeat(row), t + 1));
				tn.pn = PLAYER_INVALID;
				out[index].SetTapNote(t, row, tn);
			}
		}*/
	}

	void InsertHoldTails(NoteData& inout)
	{
		for (int t = 0; t < inout.GetNumTracks(); t++)
		{
			NoteData::iterator begin = inout.begin(t), end = inout.end(t);

			for (; begin != end; ++begin)
			{
				int iRow = begin->first;
				const TapNote& tn = begin->second;
				if (tn.type != TapNoteType_HoldHead)
					continue;

				TapNote tail = tn;
				tail.type = TapNoteType_HoldTail;

				/* If iDuration is 0, we'd end up overwriting the head with the tail
				 * (and invalidating our iterator). Empty hold notes aren't valid. */
				if (tn.iDuration == 0) {
					std::cout << "Error! Invalidating NoteData Iterator!!\n";
				}

				inout.SetTapNote(t, iRow + tn.iDuration, tail);
			}
		}
	}

	void GetSMNoteDataString(const NoteData& in, std::string& notes_out) {
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
				notes_out.append("  // measure ");
				notes_out.append(std::to_string(m));
				notes_out.append("\n");

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
							notes_out += tn.sAttackModifiers;
							float rounded = std::floor(tn.fAttackDurationSeconds * 100 + 0.5) / 100;
							notes_out += std::to_string(rounded);
						}
						// hey maybe if we have TapNoteType_Item we can do things here.
						if (tn.iKeysoundIndex >= 0) {
							notes_out += std::to_string(tn.iKeysoundIndex);
						}
					}

					notes_out.append(1, '\n');
				}
			}
		}


	}

	void LoadFromSMNoteDataString(NoteData& out, const std::string& sSMNoteData_, bool bComposite)
	{
		// Load note data
		std::string sSMNoteData;
		std::string::size_type iIndexCommentStart = 0;
		std::string::size_type iIndexCommentEnd = 0;
		std::string::size_type origSize = sSMNoteData_.size();
		const char* p = sSMNoteData_.data();

		sSMNoteData.reserve(origSize);
		while ((iIndexCommentStart = sSMNoteData_.find("//", iIndexCommentEnd)) != std::string::npos)
		{
			sSMNoteData.append(p, iIndexCommentStart - iIndexCommentEnd);
			p += iIndexCommentStart - iIndexCommentEnd;
			iIndexCommentEnd = sSMNoteData_.find("\n", iIndexCommentStart);
			iIndexCommentEnd = (iIndexCommentEnd == std::string::npos ? origSize : iIndexCommentEnd + 1);
			p += iIndexCommentEnd - iIndexCommentStart;
		}
		sSMNoteData.append(p, origSize - iIndexCommentEnd);

		// Clear notes, but keep the same number of tracks.
		int iNumTracks = out.GetNumTracks();
		out.Init();
		out.SetNumTracks(iNumTracks);

		if (!bComposite)
		{
			LoadFromSMNoteDataStringWithPlayer(out, sSMNoteData, 0, sSMNoteData.size(),
				PlayerNumber_Invalid, iNumTracks);
			return;
		}

		// TODO: Routine Support here
		/*int start = 0, size = -1;

		std::vector<NoteData> vParts;
		FOREACH_PlayerNumber(pn)
		{
			// Split in place.
			split(sSMNoteData, "&", start, size, false);
			if (unsigned(start) == sSMNoteData.size())
				break;
			vParts.push_back(NoteData());
			NoteData& nd = vParts.back();

			nd.SetNumTracks(iNumTracks);
			LoadFromSMNoteDataStringWithPlayer(nd, sSMNoteData, start, size, pn, iNumTracks);
		}
		CombineCompositeNoteData(out, vParts);
		out.RevalidateATIs(std::vector<int>(), false);*/
	}
}