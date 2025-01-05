/*
 * NoteData is organized by:
 *  track - corresponds to different columns of notes on the screen
 *  row/index - corresponds to subdivisions of beats
 */

#include "NoteData.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>


void NoteData::Init()
{
	m_TapNotes = std::vector<TrackMap>();	// ensure that the memory is freed
}

void NoteData::SetNumTracks( int iNewNumTracks )
{

	m_TapNotes.resize( iNewNumTracks );
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


bool NoteData::GetNextTapNoteRowForTrack(int track, int& rowInOut, bool ignoreAutoKeysounds) const
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

bool NoteData::IsRowEmpty(int row) const
{
	for (int t = 0; t < GetNumTracks(); t++)
		if (GetTapNote(t, row).type != TapNoteType_Empty)
			return false;
	return true;
}

bool NoteData::IsTap(const TapNote &tn, const int row) const
{
	return (tn.type != TapNoteType_Empty && tn.type != TapNoteType_Mine
		&& tn.type != TapNoteType_Lift && tn.type != TapNoteType_Fake
		&& tn.type != TapNoteType_AutoKeysound);
			//&& GAMESTATE->GetProcessedTimingData()->IsJudgableAtRow(row));
}

bool NoteData::IsMine(const TapNote &tn, const int row) const
{
	return (tn.type == TapNoteType_Mine);
			//&& GAMESTATE->GetProcessedTimingData()->IsJudgableAtRow(row));
}

bool NoteData::IsLift(const TapNote &tn, const int row) const
{
	return (tn.type == TapNoteType_Lift);
			//&& GAMESTATE->GetProcessedTimingData()->IsJudgableAtRow(row));
}

bool NoteData::IsFake(const TapNote &tn, const int row) const
{
	return (tn.type == TapNoteType_Fake);
			//|| !GAMESTATE->GetProcessedTimingData()->IsJudgableAtRow(row));
}


void NoteData::AddATIToList(all_tracks_iterator* iter) const
{
	m_atis.insert(iter);
}

void NoteData::AddATIToList(all_tracks_const_iterator* iter) const
{
	m_const_atis.insert(iter);
}

void NoteData::RemoveATIFromList(all_tracks_iterator* iter) const
{
	std::set<all_tracks_iterator*>::iterator pos= m_atis.find(iter);
	if(pos != m_atis.end())
	{
		m_atis.erase(pos);
	}
}

void NoteData::RemoveATIFromList(all_tracks_const_iterator* iter) const
{
	std::set<all_tracks_const_iterator*>::iterator pos= m_const_atis.find(iter);
	if(pos != m_const_atis.end())
	{
		m_const_atis.erase(pos);
	}
}

void NoteData::GetTapNoteRange(int iTrack, int iStartRow, int iEndRow, TrackMap::iterator& lBegin, TrackMap::iterator& lEnd)
{
	TrackMap& mapTrack = m_TapNotes[iTrack];

	if (iStartRow > iEndRow)
	{
		lBegin = lEnd = mapTrack.end();
		return;
	}

	if (iStartRow <= 0)
		lBegin = mapTrack.begin(); // optimization
	else if (iStartRow >= MAX_NOTE_ROW)
		lBegin = mapTrack.end(); // optimization
	else
		lBegin = mapTrack.lower_bound(iStartRow);

	if (iEndRow <= 0)
		lEnd = mapTrack.begin(); // optimization
	else if (iEndRow >= MAX_NOTE_ROW)
		lEnd = mapTrack.end(); // optimization
	else
		lEnd = mapTrack.lower_bound(iEndRow);
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
		iterator prev = lBegin--;

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

template<typename ND, typename iter, typename TN>
void NoteData::_all_tracks_iterator<ND, iter, TN>::Find( bool bReverse )
{
	// If no notes can be found in the range, m_iTrack will stay -1 and IsAtEnd() will return true.
	m_iTrack = -1;
	if( bReverse )
	{
		int iMaxRow = INT_MIN;
		for( int iTrack = m_pNoteData->GetNumTracks() - 1; iTrack >= 0; --iTrack )
		{
			iter &i( m_vCurrentIters[iTrack] );
			const iter &end = m_vEndIters[iTrack];
			if( i != end  &&  i->first > iMaxRow )
			{
				iMaxRow = i->first;
				m_iTrack = iTrack;
			}
		}
	}
	else
	{

		int iMinRow = INT_MAX;
		for( int iTrack = 0; iTrack < m_pNoteData->GetNumTracks(); ++iTrack )
		{
			iter &i = m_vCurrentIters[iTrack];
			const iter &end = m_vEndIters[iTrack];
			if( i != end  &&  i->first < iMinRow )
			{
				iMinRow = i->first;
				m_iTrack = iTrack;
			}
		}
	}
}

template<typename ND, typename iter, typename TN>
	NoteData::_all_tracks_iterator<ND, iter, TN>::_all_tracks_iterator( ND &nd, int iStartRow, int iEndRow, bool bReverse, bool bInclusive ) :
	m_pNoteData(&nd), m_iTrack(0), m_bReverse(bReverse)
{

	m_StartRow= iStartRow;
	m_EndRow= iEndRow;

	for( int iTrack = 0; iTrack < m_pNoteData->GetNumTracks(); ++iTrack )
	{
		iter begin, end;
		m_Inclusive= bInclusive;
		if( bInclusive )
			m_pNoteData->GetTapNoteRangeInclusive( iTrack, iStartRow, iEndRow, begin, end );
		else
			m_pNoteData->GetTapNoteRange( iTrack, iStartRow, iEndRow, begin, end );

		m_vBeginIters.push_back( begin );
		m_vEndIters.push_back( end );
		m_PrevCurrentRows.push_back(0);

		iter cur;
		if( m_bReverse )
		{
			cur = end;
			if( cur != begin )
				cur--;
		}
		else
		{
			cur = begin;
		}
		m_vCurrentIters.push_back( cur );
	}
	m_pNoteData->AddATIToList(this);

	Find( bReverse );
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN>::_all_tracks_iterator( const _all_tracks_iterator &other ) :
#define COPY_OTHER( x ) x( other.x )
	COPY_OTHER( m_pNoteData ),
	COPY_OTHER( m_vBeginIters ),
	COPY_OTHER( m_vCurrentIters ),
	COPY_OTHER( m_vEndIters ),
	COPY_OTHER( m_iTrack ),
	COPY_OTHER( m_bReverse ),
	COPY_OTHER( m_PrevCurrentRows ),
	COPY_OTHER( m_StartRow ),
	COPY_OTHER( m_EndRow )
#undef COPY_OTHER
{
	m_pNoteData->AddATIToList(this);
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN>::~_all_tracks_iterator()
{
	if(m_pNoteData != nullptr)
	{
		m_pNoteData->RemoveATIFromList(this);
	}
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN> &NoteData::_all_tracks_iterator<ND, iter, TN>::operator=( const _all_tracks_iterator &other )
{
	_all_tracks_iterator tmp (other);

#define SWAP_OTHER( x ) std::swap( x, tmp.x )
	SWAP_OTHER( m_pNoteData );
	SWAP_OTHER( m_vBeginIters );
	SWAP_OTHER( m_vCurrentIters );
	SWAP_OTHER( m_vEndIters );
	SWAP_OTHER( m_iTrack );
	SWAP_OTHER( m_bReverse );
	SWAP_OTHER( m_PrevCurrentRows );
	SWAP_OTHER( m_StartRow );
	SWAP_OTHER( m_EndRow );
#undef SWAP_OTHER
	m_pNoteData->AddATIToList(this);

	return *this;
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN> &NoteData::_all_tracks_iterator<ND, iter, TN>::operator++() // preincrement
{
	m_PrevCurrentRows[m_iTrack]= Row();
	if( m_bReverse )
	{
		if( m_vCurrentIters[m_iTrack] == m_vBeginIters[m_iTrack] )
			m_vCurrentIters[m_iTrack] = m_vEndIters[m_iTrack];
		else
			--m_vCurrentIters[m_iTrack];
	}
	else
	{
		++m_vCurrentIters[m_iTrack];
	}
	Find( m_bReverse );
	return *this;
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN> NoteData::_all_tracks_iterator<ND, iter, TN>::operator++( int ) // postincrement
{
	_all_tracks_iterator<ND, iter, TN> ret( *this );
	operator++();
	return ret;
}

template<typename ND, typename iter, typename TN>
	void NoteData::_all_tracks_iterator<ND, iter, TN>::Revalidate(
		ND* notedata, std::vector<int> const& added_or_removed_tracks, bool added)
{
	m_pNoteData= notedata;
	if(!added_or_removed_tracks.empty())
	{
		if(added)
		{
			int avg_row= 0;
			for(size_t p= 0; p < m_PrevCurrentRows.size(); ++p)
			{
				avg_row+= m_PrevCurrentRows[p];
			}
			avg_row/= m_PrevCurrentRows.size();
			for(size_t a= 0; a < added_or_removed_tracks.size(); ++a)
			{
				int track_id= added_or_removed_tracks[a];
				m_PrevCurrentRows.insert(m_PrevCurrentRows.begin()+track_id, avg_row);
			}
			m_vBeginIters.resize(m_pNoteData->GetNumTracks());
			m_vCurrentIters.resize(m_pNoteData->GetNumTracks());
			m_vEndIters.resize(m_pNoteData->GetNumTracks());
		}
		else
		{
			for(size_t a= 0; a < added_or_removed_tracks.size(); ++a)
			{
				int track_id= added_or_removed_tracks[a];
				m_PrevCurrentRows.erase(m_PrevCurrentRows.begin()+track_id);
			}
			m_vBeginIters.resize(m_pNoteData->GetNumTracks());
			m_vCurrentIters.resize(m_pNoteData->GetNumTracks());
			m_vEndIters.resize(m_pNoteData->GetNumTracks());
		}
	}
	for(int track= 0; track < m_pNoteData->GetNumTracks(); ++track)
	{
		iter begin, end;
		if(m_Inclusive)
		{
			m_pNoteData->GetTapNoteRangeInclusive(track, m_StartRow, m_EndRow, begin, end);
		}
		else
		{
			m_pNoteData->GetTapNoteRange(track, m_StartRow, m_EndRow, begin, end);
		}
		m_vBeginIters[track]= begin;
		m_vEndIters[track]= end;
		iter cur;
		if(m_bReverse)
		{
			cur= m_pNoteData->upper_bound(track, m_PrevCurrentRows[track]);
		}
		else
		{
			cur= m_pNoteData->lower_bound(track, m_PrevCurrentRows[track]);
		}
		m_vCurrentIters[track]= cur;
	}
	Find(m_bReverse);
}

/* XXX: This doesn't satisfy the requirements that ++iter; --iter; is a no-op so it cannot be bidirectional for now. */
#if 0
template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN> &NoteData::_all_tracks_iterator<ND, iter, TN>::operator--() // predecrement
{
	if( m_bReverse )
	{
		++m_vCurrentIters[m_iTrack];
	}
	else
	{
		if( m_vCurrentIters[m_iTrack] == m_vEndIters[m_iTrack] )
			m_vCurrentIters[m_iTrack] = m_vEndIters[m_iTrack];
		else
			--m_vCurrentIters[m_iTrack];
	}
	Find( !m_bReverse );
	return *this;
}

template<typename ND, typename iter, typename TN>
NoteData::_all_tracks_iterator<ND, iter, TN> NoteData::_all_tracks_iterator<ND, iter, TN>::operator--( int dummy ) // postdecrement
{
	_all_tracks_iterator<ND, iter, TN> ret( *this );
	operator--();
	return ret;
}
#endif

// Explicit instantiation.
template class NoteData::_all_tracks_iterator<NoteData, NoteData::iterator, TapNote>;
template class NoteData::_all_tracks_iterator<const NoteData, NoteData::const_iterator, const TapNote>;

/*
 * (c) 2001-2004 Chris Danford, Glenn Maynard
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
