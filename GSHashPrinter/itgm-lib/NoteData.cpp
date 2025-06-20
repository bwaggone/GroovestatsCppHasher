#include <set>

#include "NoteData.h"
#include "ConstantsAndTypes.h"
#include "RageUtil.h"

void NoteData::Init() {
	m_TapNotes = std::vector<TrackMap>();	// ensure that the memory is freed
}

void NoteData::SetNumTracks(int num_tracks) {
	m_TapNotes.resize(num_tracks);
}

bool NoteData::IsComposite() const
{
	for (int track = 0; track < GetNumTracks(); ++track)
	{
		for (const std::pair<const int, TapNote>& tn : m_TapNotes[track])
			if (tn.second.pn != PlayerNumber_Invalid)
				return true;
	}
	return false;
}

void NoteData::ClearAll()
{
	for (int t = 0; t < GetNumTracks(); t++)
		m_TapNotes[t].clear();
}

bool NoteData::IsRowEmpty(int row) const
{
	for (int t = 0; t < GetNumTracks(); t++)
		if (GetTapNote(t, row).type != TapNoteType_Empty)
			return false;
	return true;
}

bool NoteData::IsHoldNoteAtRow(int iTrack, int iRow, int* pHeadRow) const
{
	int iDummy;
	if (pHeadRow == nullptr)
		pHeadRow = &iDummy;

	/* Starting at iRow, search upwards. If we find a TapNoteType_HoldHead, we're within
	 * a hold. If we find a tap, mine or attack, we're not--those never lie
	 * within hold notes. Ignore autoKeysound. */
	FOREACH_NONEMPTY_ROW_IN_TRACK_RANGE_REVERSE(*this, iTrack, r, 0, iRow)
	{
		const TapNote& tn = GetTapNote(iTrack, r);
		switch (tn.type)
		{
		case TapNoteType_HoldHead:
			if (tn.iDuration + r < iRow)
				return false;
			*pHeadRow = r;
			return true;

		case TapNoteType_Tap:
		case TapNoteType_Mine:
		case TapNoteType_Attack:
		case TapNoteType_Lift:
		case TapNoteType_Fake:
			return false;

		case TapNoteType_Empty:
		case TapNoteType_AutoKeysound:
			// ignore
			continue;
		}
	}

	return false;
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

void NoteData::SetTapNote(int track, int row, const TapNote& t)
{
	if (row < 0)
		return;

	// There's no point in inserting empty notes into the map.
	// Any blank space in the map is defined to be empty.
	// If we're trying to insert an empty at a spot where another note
	// already exists, then we're really deleting from the map.
	if (t == TapNote(TapNoteType_Empty, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1))
	{
		TrackMap& trackMap = m_TapNotes[track];
		// remove the element at this position (if any).
		// This will return either 0 or 1.
		trackMap.erase(row);
	}
	else
	{
		m_TapNotes[track][row] = t;
	}
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


void NoteData::GetTapNoteRange(int iTrack, int iStartRow, int iEndRow, TrackMap::const_iterator& lBegin, TrackMap::const_iterator& lEnd) const
{
	TrackMap::iterator const_begin, const_end;
	const_cast<NoteData*>(this)->GetTapNoteRange(iTrack, iStartRow, iEndRow, const_begin, const_end);
	lBegin = const_begin;
	lEnd = const_end;
}

void NoteData::GetTapNoteRangeInclusive(int iTrack, int iStartRow, int iEndRow, TrackMap::iterator& lBegin, TrackMap::iterator& lEnd, bool bIncludeAdjacent)
{
	GetTapNoteRange(iTrack, iStartRow, iEndRow, lBegin, lEnd);

	if (lBegin != this->begin(iTrack))
	{
		iterator prev = Decrement(lBegin);

		const TapNote& tn = prev->second;
		if (tn.type == TapNoteType_HoldHead)
		{
			int iHoldStartRow = prev->first;
			int iHoldEndRow = iHoldStartRow + tn.iDuration;
			if (bIncludeAdjacent)
				++iHoldEndRow;
			if (iHoldEndRow > iStartRow)
			{
				// The previous note is a hold.
				lBegin = prev;
			}
		}
	}

	if (bIncludeAdjacent && lEnd != this->end(iTrack))
	{
		// Include the next note if it's a hold and starts on iEndRow.
		const TapNote& tn = lEnd->second;
		int iHoldStartRow = lEnd->first;
		if (tn.type == TapNoteType_HoldHead && iHoldStartRow == iEndRow)
			++lEnd;
	}
}

void NoteData::GetTapNoteRangeInclusive(int iTrack, int iStartRow, int iEndRow, TrackMap::const_iterator& lBegin, TrackMap::const_iterator& lEnd, bool bIncludeAdjacent) const
{
	TrackMap::iterator const_begin, const_end;
	const_cast<NoteData*>(this)->GetTapNoteRangeInclusive(iTrack, iStartRow, iEndRow, const_begin, const_end, bIncludeAdjacent);
	lBegin = const_begin;
	lEnd = const_end;
}

bool NoteData::GetNextTapNoteRowForTrack(int track, int& rowInOut, bool ignoreAutoKeysounds = false) const
{
	const TrackMap& mapTrack = m_TapNotes[track];

	// lower_bound and upper_bound have the same effect here because duplicate
	// keys aren't allowed.

	// lower_bound "finds the first element whose key is not less than k" (>=);
	// upper_bound "finds the first element whose key greater than k".  They don't
	// have the same effect, but lower_bound(row+1) should equal upper_bound(row). -glenn
	TrackMap::const_iterator iter = mapTrack.lower_bound(rowInOut + 1);	// "find the first note for which row+1 < key == false"
	if (iter == mapTrack.end())
		return false;

	// If we want to ignore autokeysounds, keep going until we find a real note.
	if (ignoreAutoKeysounds) {
		while (iter->second.type == TapNoteType_AutoKeysound) {
			iter++;
			if (iter == mapTrack.end()) return false;
		}
	}
	rowInOut = iter->first;
	return true;
}

bool NoteData::GetNextTapNoteRowForAllTracks(int& rowInOut) const
{
	int iClosestNextRow = MAX_NOTE_ROW;
	bool bAnyHaveNextNote = false;
	for (int t = 0; t < GetNumTracks(); t++)
	{
		int iNewRowThisTrack = rowInOut;
		if (GetNextTapNoteRowForTrack(t, iNewRowThisTrack))
		{
			bAnyHaveNextNote = true;
			iClosestNextRow = std::min(iClosestNextRow, iNewRowThisTrack);
		}
	}

	if (bAnyHaveNextNote)
	{
		rowInOut = iClosestNextRow;
		return true;
	}
	else
	{
		return false;
	}
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN> NoteData::_all_tracks_iterator<ND, iter, TN>::operator++(int) // postincrement
{
	_all_tracks_iterator<ND, iter, TN> ret(*this);
	operator++();
	return ret;
}

template<typename ND, typename iter, typename TN>
void NoteData::_all_tracks_iterator<ND, iter, TN>::Revalidate(
	ND* notedata, std::vector<int> const& added_or_removed_tracks, bool added)
{
	m_pNoteData = notedata;
	if (!added_or_removed_tracks.empty())
	{
		if (added)
		{
			int avg_row = 0;
			for (size_t p = 0; p < m_PrevCurrentRows.size(); ++p)
			{
				avg_row += m_PrevCurrentRows[p];
			}
			avg_row /= m_PrevCurrentRows.size();
			for (size_t a = 0; a < added_or_removed_tracks.size(); ++a)
			{
				int track_id = added_or_removed_tracks[a];
				m_PrevCurrentRows.insert(m_PrevCurrentRows.begin() + track_id, avg_row);
			}
			m_vBeginIters.resize(m_pNoteData->GetNumTracks());
			m_vCurrentIters.resize(m_pNoteData->GetNumTracks());
			m_vEndIters.resize(m_pNoteData->GetNumTracks());
		}
		else
		{
			for (size_t a = 0; a < added_or_removed_tracks.size(); ++a)
			{
				int track_id = added_or_removed_tracks[a];
				m_PrevCurrentRows.erase(m_PrevCurrentRows.begin() + track_id);
			}
			m_vBeginIters.resize(m_pNoteData->GetNumTracks());
			m_vCurrentIters.resize(m_pNoteData->GetNumTracks());
			m_vEndIters.resize(m_pNoteData->GetNumTracks());
		}
	}
	for (int track = 0; track < m_pNoteData->GetNumTracks(); ++track)
	{
		iter begin, end;
		if (m_Inclusive)
		{
			m_pNoteData->GetTapNoteRangeInclusive(track, m_StartRow, m_EndRow, begin, end);
		}
		else
		{
			m_pNoteData->GetTapNoteRange(track, m_StartRow, m_EndRow, begin, end);
		}
		m_vBeginIters[track] = begin;
		m_vEndIters[track] = end;
		iter cur;
		if (m_bReverse)
		{
			cur = m_pNoteData->upper_bound(track, m_PrevCurrentRows[track]);
		}
		else
		{
			cur = m_pNoteData->lower_bound(track, m_PrevCurrentRows[track]);
		}
		m_vCurrentIters[track] = cur;
	}
	Find(m_bReverse);
}


void NoteData::RevalidateATIs(std::vector<int> const& added_or_removed_tracks, bool added)
{
	for (std::set<all_tracks_iterator*>::iterator cur = m_atis.begin();
		cur != m_atis.end(); ++cur)
	{
		(*cur)->Revalidate(this, added_or_removed_tracks, added);
	}
	for (std::set<all_tracks_const_iterator*>::iterator cur = m_const_atis.begin();
		cur != m_const_atis.end(); ++cur)
	{
		(*cur)->Revalidate(this, added_or_removed_tracks, added);
	}
}

template<typename ND, typename iter, typename TN>
void NoteData::_all_tracks_iterator<ND, iter, TN>::Find(bool bReverse)
{
	// If no notes can be found in the range, m_iTrack will stay -1 and IsAtEnd() will return true.
	m_iTrack = -1;
	if (bReverse)
	{
		int iMaxRow = INT_MIN;
		for (int iTrack = m_pNoteData->GetNumTracks() - 1; iTrack >= 0; --iTrack)
		{
			iter& i(m_vCurrentIters[iTrack]);
			const iter& end = m_vEndIters[iTrack];
			if (i != end && i->first > iMaxRow)
			{
				iMaxRow = i->first;
				m_iTrack = iTrack;
			}
		}
	}
	else
	{

		int iMinRow = INT_MAX;
		for (int iTrack = 0; iTrack < m_pNoteData->GetNumTracks(); ++iTrack)
		{
			iter& i = m_vCurrentIters[iTrack];
			const iter& end = m_vEndIters[iTrack];
			if (i != end && i->first < iMinRow)
			{
				iMinRow = i->first;
				m_iTrack = iTrack;
			}
		}
	}
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN>::_all_tracks_iterator(ND& nd, int iStartRow, int iEndRow, bool bReverse, bool bInclusive) :
	m_pNoteData(&nd), m_iTrack(0), m_bReverse(bReverse)
{
	ASSERT(m_pNoteData->GetNumTracks() > 0);

	m_StartRow = iStartRow;
	m_EndRow = iEndRow;

	for (int iTrack = 0; iTrack < m_pNoteData->GetNumTracks(); ++iTrack)
	{
		iter begin, end;
		m_Inclusive = bInclusive;
		if (bInclusive)
			m_pNoteData->GetTapNoteRangeInclusive(iTrack, iStartRow, iEndRow, begin, end);
		else
			m_pNoteData->GetTapNoteRange(iTrack, iStartRow, iEndRow, begin, end);

		m_vBeginIters.push_back(begin);
		m_vEndIters.push_back(end);
		m_PrevCurrentRows.push_back(0);

		iter cur;
		if (m_bReverse)
		{
			cur = end;
			if (cur != begin)
				cur--;
		}
		else
		{
			cur = begin;
		}
		m_vCurrentIters.push_back(cur);
	}
	m_pNoteData->AddATIToList(this);

	Find(bReverse);
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN>::_all_tracks_iterator(const _all_tracks_iterator& other) :
#define COPY_OTHER( x ) x( other.x )
	COPY_OTHER(m_pNoteData),
	COPY_OTHER(m_vBeginIters),
	COPY_OTHER(m_vCurrentIters),
	COPY_OTHER(m_vEndIters),
	COPY_OTHER(m_iTrack),
	COPY_OTHER(m_bReverse),
	COPY_OTHER(m_PrevCurrentRows),
	COPY_OTHER(m_StartRow),
	COPY_OTHER(m_EndRow)
#undef COPY_OTHER
{
	m_pNoteData->AddATIToList(this);
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN>::~_all_tracks_iterator()
{
	if (m_pNoteData != nullptr)
	{
		m_pNoteData->RemoveATIFromList(this);
	}
}