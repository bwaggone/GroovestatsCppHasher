#ifndef NOTE_DATA_H
#define NOTE_DATA_H

#include "NoteTypes.h"

#include <map>
#include <set>
#include <iterator>
#include <vector>


/** @brief Act on each non empty row in the specific track. */
#define FOREACH_NONEMPTY_ROW_IN_TRACK( nd, track, row ) \
	for( int row = -1; (nd).GetNextTapNoteRowForTrack(track,row); )
/** @brief Act on each non empty row in the specified track within the specified range. */
#define FOREACH_NONEMPTY_ROW_IN_TRACK_RANGE( nd, track, row, start, last ) \
	for( int row = start-1; (nd).GetNextTapNoteRowForTrack(track,row) && row < (last); )
/** @brief Act on each non empty row in the specified track within the specified range,
 going in reverse order. */
#define FOREACH_NONEMPTY_ROW_IN_TRACK_RANGE_REVERSE( nd, track, row, start, last ) \
	for( int row = last; (nd).GetPrevTapNoteRowForTrack(track,row) && row >= (start); )
/** @brief Act on each non empty row for all of the tracks. */
#define FOREACH_NONEMPTY_ROW_ALL_TRACKS( nd, row ) \
	for( int row = -1; (nd).GetNextTapNoteRowForAllTracks(row); )
/** @brief Act on each non empty row for all of the tracks within the specified range. */
#define FOREACH_NONEMPTY_ROW_ALL_TRACKS_RANGE( nd, row, start, last ) \
	for( int row = start-1; (nd).GetNextTapNoteRowForAllTracks(row) && row < (last); )

/** @brief Holds data about the notes that the player is supposed to hit. */
class NoteData
{
public:
	typedef std::map<int,TapNote> TrackMap;
	typedef std::map<int,TapNote>::iterator iterator;
	typedef std::map<int,TapNote>::const_iterator const_iterator;
	typedef std::map<int,TapNote>::reverse_iterator reverse_iterator;
	typedef std::map<int,TapNote>::const_reverse_iterator const_reverse_iterator;

	NoteData(): m_TapNotes() {}

	iterator begin( int iTrack )					{ return m_TapNotes[iTrack].begin(); }
	const_iterator begin( int iTrack ) const			{ return m_TapNotes[iTrack].begin(); }
	reverse_iterator rbegin( int iTrack )				{ return m_TapNotes[iTrack].rbegin(); }
	const_reverse_iterator rbegin( int iTrack ) const		{ return m_TapNotes[iTrack].rbegin(); }
	iterator end( int iTrack )					{ return m_TapNotes[iTrack].end(); }
	const_iterator end( int iTrack ) const				{ return m_TapNotes[iTrack].end(); }
	reverse_iterator rend( int iTrack )				{ return m_TapNotes[iTrack].rend(); }
	const_reverse_iterator rend( int iTrack ) const			{ return m_TapNotes[iTrack].rend(); }
	iterator lower_bound( int iTrack, int iRow )			{ return m_TapNotes[iTrack].lower_bound( iRow ); }
	const_iterator lower_bound( int iTrack, int iRow ) const	{ return m_TapNotes[iTrack].lower_bound( iRow ); }
	iterator upper_bound( int iTrack, int iRow )			{ return m_TapNotes[iTrack].upper_bound( iRow ); }
	const_iterator upper_bound( int iTrack, int iRow ) const	{ return m_TapNotes[iTrack].upper_bound( iRow ); }
	void swap( NoteData &nd )
	{
		m_TapNotes.swap(nd.m_TapNotes);
		m_atis.swap(nd.m_atis);
		m_const_atis.swap(nd.m_const_atis);
	}


	// This is ugly to make it templated but I don't want to have to write the same class twice.
	template<typename ND, typename iter, typename TN>
	class _all_tracks_iterator
	{
		ND		*m_pNoteData;
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

		void Find( bool bReverse );
	public:
		_all_tracks_iterator( ND &nd, int iStartRow, int iEndRow, bool bReverse, bool bInclusive );
		_all_tracks_iterator( const _all_tracks_iterator &other );
		~_all_tracks_iterator();
		_all_tracks_iterator &operator=( const _all_tracks_iterator &other );
		_all_tracks_iterator &operator++();		// preincrement
		_all_tracks_iterator operator++( int dummy );	// postincrement
		//_all_tracks_iterator &operator--();		// predecrement
		//_all_tracks_iterator operator--( int dummy );	// postdecrement
		inline int Track() const		{ return m_iTrack; }
		inline int Row() const			{ return m_vCurrentIters[m_iTrack]->first; }
		inline bool IsAtEnd() const		{ return m_iTrack == -1; }
		inline iter GetIter( int iTrack ) const	{ return m_vCurrentIters[iTrack]; }
		// Use when transforming the NoteData.
		void Revalidate(ND* notedata, std::vector<int> const& added_or_removed_tracks, bool added);
	};
	typedef _all_tracks_iterator<NoteData, NoteData::iterator, TapNote> 			all_tracks_iterator;
	typedef _all_tracks_iterator<const NoteData, NoteData::const_iterator, const TapNote>	all_tracks_const_iterator;
	typedef all_tracks_iterator								all_tracks_reverse_iterator;
	typedef all_tracks_const_iterator							all_tracks_const_reverse_iterator;
	friend class _all_tracks_iterator<NoteData, NoteData::iterator, TapNote>;
	friend class _all_tracks_iterator<const NoteData, NoteData::const_iterator, const TapNote>;
private:
	// There's no point in inserting empty notes into the map.
	// Any blank space in the map is defined to be empty.
	std::vector<TrackMap>	m_TapNotes;

	/**
	 * @brief Determine if the note in question should be counted as a tap.
	 * @param tn the note in question.
	 * @param row the row it lives in.
	 * @return true if it's a tap, false otherwise. */
	bool IsTap(const TapNote &tn, const int row) const;

	/**
	 * @brief Determine if the note in question should be counted as a mine.
	 * @param tn the note in question.
	 * @param row the row it lives in.
	 * @return true if it's a mine, false otherwise. */
	bool IsMine(const TapNote &tn, const int row) const;

	/**
	 * @brief Determine if the note in question should be counted as a lift.
	 * @param tn the note in question.
	 * @param row the row it lives in.
	 * @return true if it's a lift, false otherwise. */
	bool IsLift(const TapNote &tn, const int row) const;

	/**
	 * @brief Determine if the note in question should be counted as a fake.
	 * @param tn the note in question.
	 * @param row the row it lives in.
	 * @return true if it's a fake, false otherwise. */
	bool IsFake(const TapNote &tn, const int row) const;

	// These exist so that they can be revalidated when something that transforms
	// the NoteData occurs. -Kyz
	mutable std::set<all_tracks_iterator*> m_atis;
	mutable std::set<all_tracks_const_iterator*> m_const_atis;

	void AddATIToList(all_tracks_iterator* iter) const;
	void AddATIToList(all_tracks_const_iterator* iter) const;
	void RemoveATIFromList(all_tracks_iterator* iter) const;
	void RemoveATIFromList(all_tracks_const_iterator* iter) const;

	// Mina stuf (Used for chartkey hashing)
	std::vector<int> NonEmptyRowVector;

public:
	void Init();

	std::vector<int>& GetNonEmptyRowVector() { return NonEmptyRowVector; };

	int GetNumTracks() const { return m_TapNotes.size(); }
	void SetNumTracks( int iNewNumTracks );
	bool operator==( const NoteData &nd ) const			{ return m_TapNotes == nd.m_TapNotes; }
	bool operator!=( const NoteData &nd ) const			{ return m_TapNotes != nd.m_TapNotes; }

	/* Return the note at the given track and row.  Row may be out of
	 * range; pretend the song goes on with TAP_EMPTYs indefinitely. */
	inline const TapNote &GetTapNote( unsigned track, int row ) const
	{
		const TrackMap &mapTrack = m_TapNotes[track];
		TrackMap::const_iterator iter = mapTrack.find( row );
		if( iter != mapTrack.end() )
			return iter->second;
		return TAP_EMPTY;
	}
	bool GetPrevTapNoteRowForTrack(int track, int& rowInOut) const;
	int GetLastRow() const;	 // return the beat number of the last note
	float GetLastBeat() const { return NoteRowToBeat(GetLastRow()); }
	bool GetNextTapNoteRowForTrack(int track, int& rowInOut, bool ignoreKeySounds = false) const;
	bool GetNextTapNoteRowForAllTracks(int& rowInOut) const;
	bool IsRowEmpty(int row) const;

	
	void GetTapNoteRange( int iTrack, int iStartRow, int iEndRow, TrackMap::iterator &begin, TrackMap::iterator &end );
	void GetTapNoteRange(int iTrack, int iStartRow, int iEndRow,
		TrackMap::const_iterator& begin, TrackMap::const_iterator& end) const;
	void GetTapNoteRangeInclusive(int iTrack, int iStartRow, int iEndRow,
		TrackMap::const_iterator& begin, TrackMap::const_iterator& end, bool bIncludeAdjacent = false) const;
	void GetTapNoteRangeInclusive(int iTrack, int iStartRow, int iEndRow,
		TrackMap::iterator& begin, TrackMap::iterator& end, bool bIncludeAdjacent = false);
};

/** @brief Allow a quick way to swap notedata. */
namespace std
{
	template<> inline void swap<NoteData>( NoteData &nd1, NoteData &nd2 ) noexcept
	{
		nd1.swap( nd2 );
	}
}

#endif
