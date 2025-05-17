#ifndef NOTE_DATA_H
#define NOTE_DATA_H

#include <map>
#include <set>
#include <vector>

#include "NoteTypes.h"


#define FOREACH_NONEMPTY_ROW_ALL_TRACKS_RANGE( nd, row, start, last ) \
	for( int row = start-1; (nd).GetNextTapNoteRowForAllTracks(row) && row < (last); )
#define FOREACH_NONEMPTY_ROW_IN_TRACK_RANGE_REVERSE( nd, track, row, start, last ) \
	for( int row = last; (nd).GetPrevTapNoteRowForTrack(track,row) && row >= (start); )

class NoteData
{
public:
	typedef std::map<int, TapNote> TrackMap;
	typedef std::map<int, TapNote>::iterator iterator;
	typedef std::map<int, TapNote>::const_iterator const_iterator;
	typedef std::map<int, TapNote>::reverse_iterator reverse_iterator;
	typedef std::map<int, TapNote>::const_reverse_iterator const_reverse_iterator;



	template<typename ND, typename iter, typename TN>
	class _all_tracks_iterator
	{
		ND* m_pNoteData;
		std::vector<iter>	m_vBeginIters;

		/* There isn't a "past the beginning" iterator so this is hard to make a true bidirectional iterator.
		* Use the "past the end" iterator in place of the "past the beginning" iterator when in reverse. */
		std::vector<iter>	m_vCurrentIters;

		std::vector<iter>	m_vEndIters;
		int		m_iTrack;
		bool		m_bReverse;

		// These exist so that the iterator can be revalidated if the NoteData is
		// transformed during this iterator's lifetime.
		std::vector<int> m_PrevCurrentRows;
		bool m_Inclusive;
		int m_StartRow;
		int m_EndRow;

		void Find(bool bReverse);
	public:
		_all_tracks_iterator(ND& nd, int iStartRow, int iEndRow, bool bReverse, bool bInclusive);
		_all_tracks_iterator(const _all_tracks_iterator& other);
		~_all_tracks_iterator();
		_all_tracks_iterator& operator=(const _all_tracks_iterator& other);
		_all_tracks_iterator& operator++();		// preincrement
		_all_tracks_iterator operator++(int dummy);	// postincrement
		//_all_tracks_iterator &operator--();		// predecrement
		//_all_tracks_iterator operator--( int dummy );	// postdecrement
		inline int Track() const { return m_iTrack; }
		inline int Row() const { return m_vCurrentIters[m_iTrack]->first; }
		inline bool IsAtEnd() const { return m_iTrack == -1; }
		inline iter GetIter(int iTrack) const { return m_vCurrentIters[iTrack]; }
		inline TN& operator*() { DEBUG_ASSERT(!IsAtEnd()); return m_vCurrentIters[m_iTrack]->second; }
		inline TN* operator->() { DEBUG_ASSERT(!IsAtEnd()); return &m_vCurrentIters[m_iTrack]->second; }
		inline const TN& operator*() const { DEBUG_ASSERT(!IsAtEnd()); return m_vCurrentIters[m_iTrack]->second; }
		inline const TN* operator->() const { DEBUG_ASSERT(!IsAtEnd()); return &m_vCurrentIters[m_iTrack]->second; }
		// Use when transforming the NoteData.
		void Revalidate(ND* notedata, std::vector<int> const& added_or_removed_tracks, bool added);
	};
	typedef _all_tracks_iterator<NoteData, NoteData::iterator, TapNote> 			all_tracks_iterator;
	typedef _all_tracks_iterator<const NoteData, NoteData::const_iterator, const TapNote>	all_tracks_const_iterator;
	typedef all_tracks_iterator								all_tracks_reverse_iterator;
	typedef all_tracks_const_iterator							all_tracks_const_reverse_iterator;
	friend class _all_tracks_iterator<NoteData, NoteData::iterator, TapNote>;
	friend class _all_tracks_iterator<const NoteData, NoteData::const_iterator, const TapNote>;

	iterator begin(int iTrack) { return m_TapNotes[iTrack].begin(); }
	const_iterator begin(int iTrack) const { return m_TapNotes[iTrack].begin(); }
	reverse_iterator rbegin(int iTrack) { return m_TapNotes[iTrack].rbegin(); }
	const_reverse_iterator rbegin(int iTrack) const { return m_TapNotes[iTrack].rbegin(); }
	iterator end(int iTrack) { return m_TapNotes[iTrack].end(); }
	const_iterator end(int iTrack) const { return m_TapNotes[iTrack].end(); }
	reverse_iterator rend(int iTrack) { return m_TapNotes[iTrack].rend(); }
	const_reverse_iterator rend(int iTrack) const { return m_TapNotes[iTrack].rend(); }
	iterator lower_bound(int iTrack, int iRow) { return m_TapNotes[iTrack].lower_bound(iRow); }
	const_iterator lower_bound(int iTrack, int iRow) const { return m_TapNotes[iTrack].lower_bound(iRow); }
	iterator upper_bound(int iTrack, int iRow) { return m_TapNotes[iTrack].upper_bound(iRow); }
	const_iterator upper_bound(int iTrack, int iRow) const { return m_TapNotes[iTrack].upper_bound(iRow); }

	inline iterator FindTapNote(unsigned iTrack, int iRow) { return m_TapNotes[iTrack].find(iRow); }
	inline const_iterator FindTapNote(unsigned iTrack, int iRow) const { return m_TapNotes[iTrack].find(iRow); }
	void RemoveTapNote(unsigned iTrack, iterator it) { m_TapNotes[iTrack].erase(it); }


	void Init();
	void SetNumTracks(int num_tracks);
	int GetNumTracks() const { return m_TapNotes.size(); }
	void ClearAll();
	int GetLastRow() const;
	float GetLastBeat() const { return NoteRowToBeat(GetLastRow()); }
	bool GetPrevTapNoteRowForTrack(int track, int& rowInOut) const;
	bool IsComposite() const;
	void NoteData::SetTapNote(int track, int row, const TapNote& t); 
	bool NoteData::IsRowEmpty(int row) const;
	bool NoteData::GetNextTapNoteRowForAllTracks(int& rowInOut) const;
	bool NoteData::GetNextTapNoteRowForTrack(int track, int& rowInOut, bool ignoreAutoKeysounds) const;
	bool NoteData::IsHoldNoteAtRow(int iTrack, int iRow, int* pHeadRow) const;
	void RevalidateATIs(std::vector<int> const& added_or_removed_tracks, bool added);
	void GetTapNoteRange(int iTrack, int iStartRow, int iEndRow,
		TrackMap::const_iterator& begin, TrackMap::const_iterator& end) const;
	void GetTapNoteRangeInclusive(int iTrack, int iStartRow, int iEndRow,
		TrackMap::const_iterator& begin, TrackMap::const_iterator& end, bool bIncludeAdjacent = false) const;
	void GetTapNoteRangeInclusive(int iTrack, int iStartRow, int iEndRow,
		TrackMap::iterator& begin, TrackMap::iterator& end, bool bIncludeAdjacent = false);


	inline const TapNote& GetTapNote(unsigned track, int row) const
	{
		const TrackMap& mapTrack = m_TapNotes[track];
		TrackMap::const_iterator iter = mapTrack.find(row);
		if (iter != mapTrack.end())
			return iter->second;
		else
			return TapNote(TapNoteType_Empty, TapNoteSubType_Invalid, TapNoteSource_Original, "", 0, -1);
	}

	mutable std::set<all_tracks_iterator*> m_atis;
	mutable std::set<all_tracks_const_iterator*> m_const_atis;

private:
	std::vector<TrackMap>	m_TapNotes;
};

#endif