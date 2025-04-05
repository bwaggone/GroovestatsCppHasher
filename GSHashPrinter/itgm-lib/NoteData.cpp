#include "NoteData.h"

void NoteData::Init() {

}

void NoteData::SetNumTracks(int num_tracks) {
	// This usually resizes a "TrackMap" vector. But we stripped out that functionality for
	// this script.
	m_TapNotes.resize(num_tracks);
}

void NoteData::ClearAll()
{
	for (int t = 0; t < GetNumTracks(); t++)
		m_TapNotes[t].clear();
}

bool NoteData::GetPrevTapNoteRowForTrack(int track, int& rowInOut) const
{
	const TrackMap& mapTrack = m_TapNotes[track];

	// Find the first note >= rowInOut.
	TrackMap::const_iterator iter = mapTrack.lower_bound(rowInOut);

	// If we're at the beginning, we can't move back any more.
	if (iter == mapTrack.begin())
		return false;

	// Move back by one.
	--iter;
	rowInOut = iter->first;
	return true;
}


int NoteData::GetLastRow() const
{
	int iOldestRowFoundSoFar = 0;

	for (int t = 0; t < GetNumTracks(); t++)
	{
		int iRow = MAX_NOTE_ROW;
		if (!GetPrevTapNoteRowForTrack(t, iRow))
			continue;

		/* XXX: We might have a hold note near the end with autoplay sounds
		 * after it.  Do something else with autoplay sounds ... */
		const TapNote& tn = GetTapNote(t, iRow);
		if (tn.type == TapNoteType_HoldHead)
			iRow += tn.iDuration;

		iOldestRowFoundSoFar = std::max(iOldestRowFoundSoFar, iRow);
	}

	return iOldestRowFoundSoFar;
}