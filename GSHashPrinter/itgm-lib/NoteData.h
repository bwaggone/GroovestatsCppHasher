#ifndef NOTE_DATA_H
#define NOTE_DATA_H

#include <map>
#include <vector>

#include "NoteTypes.h"

class NoteData
{
public:
	typedef std::map<int, TapNote> TrackMap;

	void Init();
	void SetNumTracks(int num_tracks);
	int GetNumTracks() const { return m_TapNotes.size(); }
	void ClearAll();
	int GetLastRow() const;
	float GetLastBeat() const { return NoteRowToBeat(GetLastRow()); }
	bool NoteData::GetPrevTapNoteRowForTrack(int track, int& rowInOut) const;


	inline const TapNote& GetTapNote(unsigned track, int row) const
	{
		const TrackMap& mapTrack = m_TapNotes[track];
		TrackMap::const_iterator iter = mapTrack.find(row);
		if (iter != mapTrack.end())
			return iter->second;
		else
			return TapNote(TapNoteType_Empty, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	}

private:
	std::vector<TrackMap>	m_TapNotes;
};

#endif