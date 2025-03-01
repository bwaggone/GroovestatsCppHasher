#ifndef TIMING_SEGMENT_H
#define TIMING_SEGMENT_H

#include "NoteTypes.h"

enum TimingSegmentType
{
	SEGMENT_BPM,
	SEGMENT_STOP,
	SEGMENT_DELAY,
	SEGMENT_TIME_SIG,
	SEGMENT_WARP,
	SEGMENT_LABEL,
	SEGMENT_TICKCOUNT,
	SEGMENT_COMBO,
	SEGMENT_SPEED,
	SEGMENT_SCROLL,
	SEGMENT_FAKE,
	NUM_TimingSegmentType,
	TimingSegmentType_Invalid,
};

// XXX: dumb names
enum SegmentEffectType
{
	SegmentEffectType_Row,		// takes effect on a single row
	SegmentEffectType_Range,	// takes effect for a definite amount of rows
	SegmentEffectType_Indefinite,	// takes effect until the next segment of its type
	NUM_SegmentEffectType,
	SegmentEffectType_Invalid,
};


const int ROW_INVALID = -1;

#define COMPARE(x) if( this->x!=other.x ) return false
#define COMPARE_FLOAT(x) if( std::abs(this->x - other.x) > EPSILON ) return false

struct TimingSegment {
	virtual TimingSegmentType GetType() const { return TimingSegmentType_Invalid; }
	virtual SegmentEffectType GetEffectType() const { return SegmentEffectType_Invalid; }
	virtual TimingSegment* Copy() const = 0;

	virtual bool IsNotable() const = 0;
	//virtual void DebugPrint() const;

	// don't allow base TimingSegments to be instantiated directly
	TimingSegment(int iRow = ROW_INVALID) : m_iStartRow(iRow) { }
	TimingSegment(float fBeat) : m_iStartRow(ToNoteRow(fBeat)) { }

	TimingSegment(const TimingSegment& other) :
		m_iStartRow(other.GetRow()) { }

	// for our purposes, two floats within this level of error are equal
	static constexpr double EPSILON = 1e-6;

	virtual ~TimingSegment() { }

	/**
	 * @brief Scales itself.
	 * @param start Starting row
	 * @param length Length in rows
	 * @param newLength The new length in rows
	 */
	//virtual void Scale(int start, int length, int newLength);

	int GetRow() const { return m_iStartRow; }
	void SetRow(int iRow) { m_iStartRow = iRow; }

	float GetBeat() const { return NoteRowToBeat(m_iStartRow); }
	void SetBeat(float fBeat) { SetRow(BeatToNoteRow(fBeat)); }

	virtual std::string ToString(int /* dec */) const
	{
		return std::to_string(GetBeat());
	}

	virtual std::vector<float> GetValues() const
	{
		return std::vector<float>(0);
	}

	bool operator<(const TimingSegment& other) const
	{
		return GetRow() < other.GetRow();
	}

	// overloads should not call this base version; derived classes
	// should only compare contents, and this compares position.
	virtual bool operator==(const TimingSegment& other) const
	{
		return GetRow() == other.GetRow();
	}

	virtual bool operator!=(const TimingSegment& other) const
	{
		return !this->operator==(other);
	}

private:
	/** @brief The row in which this segment activates. */
	int m_iStartRow;

};

struct BPMSegment : public TimingSegment
{
	TimingSegmentType GetType() const { return SEGMENT_BPM; }
	SegmentEffectType GetEffectType() const { return SegmentEffectType_Indefinite; }

	bool IsNotable() const { return true; } // indefinite segments are always true
	//void DebugPrint() const;

	TimingSegment* Copy() const { return new BPMSegment(*this); }

	// note that this takes a BPM, not a BPS (compatibility)
	BPMSegment(int iStartRow = ROW_INVALID, float fBPM = 0.0f) :
		TimingSegment(iStartRow) {
		SetBPM(fBPM);
	}

	BPMSegment(const BPMSegment& other) :
		TimingSegment(other.GetRow()),
		m_fBPS(other.GetBPS()) { }

	float GetBPS() const { return m_fBPS; }
	float GetBPM() const { return m_fBPS * 60.0f; }

	void SetBPS(float fBPS) { m_fBPS = fBPS; }
	void SetBPM(float fBPM) { m_fBPS = fBPM / 60.0f; }

	//std::string ToString(int dec) const;
	std::vector<float> GetValues() const { return std::vector<float>(1, GetBPM()); }

	bool operator==(const BPMSegment& other) const
	{
		COMPARE_FLOAT(m_fBPS);
		return true;
	}

	bool operator==(const TimingSegment& other) const
	{
		if (GetType() != other.GetType())
			return false;

		return operator==(static_cast<const BPMSegment&>(other));
	}

private:
	/** @brief The number of beats per second within this BPMSegment. */
	float m_fBPS;
};

#endif