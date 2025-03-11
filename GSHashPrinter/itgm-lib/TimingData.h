#ifndef TIMING_DATA_H
#define TIMING_DATA_H

#include <array>
#include <string>
#include <vector>

#include "RageUtil.h"
#include "TimingSegment.h"

/* convenience functions to handle static casting */
template<class T>
inline T ToDerived(const TimingSegment* t, TimingSegmentType tst)
{
	ASSERT_M(t && tst == t->GetType(),
		ssprintf("type mismatch (expected %s, got %s)",
			TimingSegmentTypeToString(tst).c_str(),
			TimingSegmentTypeToString(t->GetType()).c_str()));

	return static_cast<T>(t);
}

#define TimingSegmentToXWithName(Seg, SegName, SegType) \
	inline const Seg* To##SegName( const TimingSegment *t ) \
	{ \
		return static_cast<const Seg*>( t ); \
	} \
	inline Seg* To##SegName( TimingSegment *t ) \
	{ \
		return static_cast<Seg*>( t ); \
	}

#define TimingSegmentToX(Seg, SegType) \
	TimingSegmentToXWithName(Seg##Segment, Seg, SEGMENT_##SegType)

/* ToBPM(TimingSegment*), ToTimeSignature(TimingSegment*), etc. */
TimingSegmentToX(BPM, BPM);
TimingSegmentToX(Stop, STOP);
TimingSegmentToX(Delay, DELAY);
TimingSegmentToX(TimeSignature, TIME_SIG);
TimingSegmentToX(Warp, WARP);
TimingSegmentToX(Label, LABEL);
TimingSegmentToX(Tickcount, TICKCOUNT);
TimingSegmentToX(Combo, COMBO);
TimingSegmentToX(Speed, SPEED);
TimingSegmentToX(Scroll, SCROLL);
TimingSegmentToX(Fake, FAKE);

#undef TimingSegmentToXWithName
#undef TimingSegmentToX

class TimingData {
public:
	TimingData(float fOffset = 0) : beat0OffsetInSeconds_(fOffset) {};
	~TimingData();

	void Copy(const TimingData& other);
	void Clear();
	bool IsSafeFullTiming();

	TimingData(const TimingData& cpy) { Copy(cpy); }
	TimingData& operator=(const TimingData& cpy) { Copy(cpy); return *this; }


	const TimingSegment* GetSegmentAtRow(int iNoteRow, TimingSegmentType tst) const;
	TimingSegment* GetSegmentAtRow(int iNoteRow, TimingSegmentType tst);
	int GetSegmentIndexAtRow(TimingSegmentType tst, int iRow) const;

	// GetBeatArgs, GetBeatStarts, m_beat_start_lookup, m_time_start_lookup,
	// PrepareLookup, and ReleaseLookup form a system for speeding up finding
	// the current beat and bps from the time, or finding the time from the
	// current beat.
	// The lookup tables contain indices for the beat and time finding
	// functions to start at so they don't have to walk through all the timing
	// segments.
	// PrepareLookup should be called before gameplay starts, so that the lookup
	// tables are populated.  ReleaseLookup should be called after gameplay
	// finishes so that memory isn't wasted.
	// -Kyz
	struct GetBeatArgs
	{
		float elapsed_time;
		float beat;
		float bps_out;
		float warp_dest_out;
		int warp_begin_out;
		bool freeze_out;
		bool delay_out;
		GetBeatArgs() :elapsed_time(0), beat(0), bps_out(0), warp_dest_out(0),
			warp_begin_out(-1), freeze_out(false), delay_out(false) {}
	};
	struct GetBeatStarts
	{
		unsigned int bpm;
		unsigned int warp;
		unsigned int stop;
		unsigned int delay;
		int last_row;
		float last_time;
		float warp_destination;
		bool is_warping;
		GetBeatStarts() :bpm(0), warp(0), stop(0), delay(0), last_row(0),
			last_time(0), warp_destination(0), is_warping(false) {}
	};

	const std::vector<TimingSegment*>& GetTimingSegments(TimingSegmentType tst) const
	{
		return const_cast<TimingData*>(this)->GetTimingSegments(tst);
	}
	std::vector<TimingSegment*>& GetTimingSegments(TimingSegmentType tst)
	{
		return m_avpTimingSegments[tst];
	}

#define DefineSegmentWithName(Seg, SegName, SegType) \
		const Seg* Get##Seg##AtRow( int iNoteRow ) const \
		{ \
			const TimingSegment *t = GetSegmentAtRow( iNoteRow, SegType ); \
			return To##SegName( t ); \
		} \
		Seg* Get##Seg##AtRow( int iNoteRow ) \
		{ \
			return const_cast<Seg*> (((const TimingData*)this)->Get##Seg##AtRow(iNoteRow) ); \
		} \
		const Seg* Get##Seg##AtBeat( float fBeat ) const \
		{ \
			return Get##Seg##AtRow( BeatToNoteRow(fBeat) ); \
		} \
		Seg* Get##Seg##AtBeat( float fBeat ) \
		{ \
			return const_cast<Seg*> (((const TimingData*)this)->Get##Seg##AtBeat(fBeat) ); \
		} \
		void AddSegment( const Seg &seg ) \
		{ \
			AddSegment( &seg ); \
		}

	// (TimeSignature,TIME_SIG) -> (TimeSignatureSegment,SEGMENT_TIME_SIG)
#define DefineSegment(Seg, SegType ) \
		DefineSegmentWithName( Seg##Segment, Seg, SEGMENT_##SegType )

	DefineSegment(BPM, BPM);
	DefineSegment(Stop, STOP);
	DefineSegment(Delay, DELAY);
	DefineSegment(Warp, WARP);
	DefineSegment(Label, LABEL);
	DefineSegment(Tickcount, TICKCOUNT);
	DefineSegment(Combo, COMBO);
	DefineSegment(Speed, SPEED);
	DefineSegment(Scroll, SCROLL);
	DefineSegment(Fake, FAKE);
	DefineSegment(TimeSignature, TIME_SIG);

#undef DefineSegmentWithName
#undef DefineSegment

	float beat0OffsetInSeconds_;
private:
	std::string filename_;

protected:
	// don't call this directly; use the derived-type overloads.
	void AddSegment(const TimingSegment* seg);

	// All of the following vectors must be sorted before gameplay.
	std::array<std::vector<TimingSegment*>, NUM_TimingSegmentType> m_avpTimingSegments;
};

#endif