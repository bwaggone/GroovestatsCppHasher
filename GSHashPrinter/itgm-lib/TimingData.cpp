#include <vector>

#include "TimingData.h"

static const int INVALID_INDEX = -1;

static const TimingSegment* DummySegments[NUM_TimingSegmentType] =
{
	nullptr, // BPMSegment
	nullptr, //new StopSegment,
	nullptr, //new DelaySegment,
	nullptr, // TimeSignatureSegment
	nullptr, //new WarpSegment,
	nullptr, // LabelSegment
	nullptr, // TickcountSegment
	nullptr, // ComboSegment
	nullptr, // SpeedSegment
	nullptr, // ScrollSegment
	nullptr, //new FakeSegment
};

void TimingData::Copy(const TimingData& cpy)
{
	/* de-allocate any old pointers we had */
	Clear();

	beat0OffsetInSeconds_ = cpy.beat0OffsetInSeconds_;
	filename_ = cpy.filename_;

	/*FOREACH_TimingSegmentType(tst)
	{
		const std::vector<TimingSegment*>& vpSegs = cpy.m_avpTimingSegments[tst];

		for (unsigned i = 0; i < vpSegs.size(); ++i)
			AddSegment(vpSegs[i]);
	}*/
}

bool TimingData::empty() const
{
	FOREACH_TimingSegmentType(tst)
		if (!GetTimingSegments(tst).empty())
			return false;

	return true;
}

TimingData::~TimingData()
{
	Clear();
}


int TimingData::GetSegmentIndexAtRow(TimingSegmentType tst, int iRow) const
{
	const std::vector<TimingSegment*>& vSegs = GetTimingSegments(tst);

	if (vSegs.empty())
		return INVALID_INDEX;

	int min = 0, max = vSegs.size() - 1;
	int l = min, r = max;
	while (l <= r)
	{
		int m = (l + r) / 2;
		if ((m == min || vSegs[m]->GetRow() <= iRow) && (m == max || iRow < vSegs[m + 1]->GetRow()))
		{
			return m;
		}
		else if (vSegs[m]->GetRow() <= iRow)
		{
			l = m + 1;
		}
		else
		{
			r = m - 1;
		}
	}

	// iRow is before the first segment of type tst
	return INVALID_INDEX;
}

const TimingSegment* TimingData::GetSegmentAtRow(int iNoteRow, TimingSegmentType tst) const
{
	const std::vector<TimingSegment*>& vSegments = GetTimingSegments(tst);

	if (vSegments.empty())
		return DummySegments[tst];

	int index = GetSegmentIndexAtRow(tst, iNoteRow);
	const TimingSegment* seg = vSegments[index];

	switch (seg->GetEffectType())
	{
	case SegmentEffectType_Indefinite:
	{
		// this segment is in effect at this row
		return seg;
	}
	default:
	{
		// if the returned segment isn't exactly on this row,
		// we don't want it, return a dummy instead
		if (seg->GetRow() == iNoteRow)
			return seg;
		else
			return DummySegments[tst];
	}
	}
}

TimingSegment* TimingData::GetSegmentAtRow(int iNoteRow, TimingSegmentType tst)
{
	return const_cast<TimingSegment*>(static_cast<const TimingData*>(this)->GetSegmentAtRow(iNoteRow, tst));
}


void TimingData::Clear()
{
	/* Delete all pointers owned by this TimingData. */
	/*FOREACH_TimingSegmentType(tst)
	{
		std::vector<TimingSegment*>& vSegs = m_avpTimingSegments[tst];
		for (unsigned i = 0; i < vSegs.size(); ++i)
		{
			RageUtil::SafeDelete(vSegs[i]);
		}

		vSegs.clear();
	}*/
}

/*bool TimingData::IsSafeFullTiming()
{
	static std::vector<TimingSegmentType> needed_segments;
	if (needed_segments.empty())
	{
		needed_segments.push_back(SEGMENT_BPM);
		needed_segments.push_back(SEGMENT_TIME_SIG);
		needed_segments.push_back(SEGMENT_TICKCOUNT);
		needed_segments.push_back(SEGMENT_COMBO);
		needed_segments.push_back(SEGMENT_LABEL);
		needed_segments.push_back(SEGMENT_SPEED);
		needed_segments.push_back(SEGMENT_SCROLL);
	}
	for (size_t s = 0; s < needed_segments.size(); ++s)
	{
		if (m_avpTimingSegments[needed_segments[s]].empty())
		{
			return false;
		}
	}
	return true;
}*/

struct ts_less
{
	bool operator() (const TimingSegment* x, const TimingSegment* y) const
	{
		return (*x) < (*y);
	}
};

static void EraseSegment(std::vector<TimingSegment*>& vSegs, int index, TimingSegment* cur)
{
	vSegs.erase(vSegs.begin() + index);
	RageUtil::SafeDelete(cur);
}

void TimingData::AddSegment(const TimingSegment* seg)
{

	TimingSegmentType tst = seg->GetType();
	std::vector<TimingSegment*>& vSegs = m_avpTimingSegments[tst];

	// OPTIMIZATION: if this is our first segment, push and return.
	if (vSegs.empty())
	{
		vSegs.push_back(seg->Copy());
		return;
	}

	int index = GetSegmentIndexAtRow(tst, seg->GetRow());
	TimingSegment* cur = vSegs[index];

	bool bIsNotable = seg->IsNotable();
	bool bOnSameRow = seg->GetRow() == cur->GetRow();

	// ignore changes that are zero and don't overwrite an existing segment
	if (!bIsNotable && !bOnSameRow)
		return;

	switch (seg->GetEffectType())
	{
	case SegmentEffectType_Row:
	case SegmentEffectType_Range:
	{
		// if we're overwriting a change with a non-notable
		// one, take it to mean deleting the existing segment
		if (bOnSameRow && !bIsNotable)
		{
			EraseSegment(vSegs, index, cur);
			return;
		}

		break;
	}
	case SegmentEffectType_Indefinite:
	{
		TimingSegment* prev = cur;

		// get the segment before last; if we're on the same
		// row, get the segment in effect before 'cur'
		if (bOnSameRow && index > 0)
		{
			prev = vSegs[index - 1];
		}

		// If there is another segment after this one, it might become
		// redundant when this one is inserted.
		// If the next segment is redundant, we want to move its starting row
		// to the row the new segment is being added at instead of erasing it
		// and adding the new segment.
		// If the new segment is also redundant, erase the next segment because
		// that effectively moves it back to the prev segment. -Kyz
		if (static_cast<size_t>(index) < vSegs.size() - 1)
		{
			TimingSegment* next = vSegs[index + 1];
			if ((*seg) == (*next))
			{
				// The segment after this new one is redundant.
				if ((*seg) == (*prev))
				{
					// This new segment is redundant.  Erase the next segment and
					// ignore this new one.
					EraseSegment(vSegs, index + 1, next);
					if (prev != cur)
					{
						EraseSegment(vSegs, index, cur);
					}
					return;
				}
				else
				{
					// Move the next segment's start back to this row.
					next->SetRow(seg->GetRow());
					if (prev != cur)
					{
						EraseSegment(vSegs, index, cur);
					}
					return;
				}
			}
			else
			{
				// if true, this is redundant segment change
				if ((*prev) == (*seg))
				{
					if (prev != cur)
					{
						EraseSegment(vSegs, index, cur);
					}
					return;
				}
			}
		}
		else
		{
			// if true, this is redundant segment change
			if ((*prev) == (*seg))
			{
				if (prev != cur)
				{
					EraseSegment(vSegs, index, cur);
				}
				return;
			}
		}
		break;
	}
	default:
		break;
	}

	// the segment at or before this row is equal to the new one; ignore it
	if (bOnSameRow && (*cur) == (*seg))
	{
		return;
	}

	// Copy() the segment (which allocates a new segment), assign it
	// to the position of the old one, then delete the old pointer.
	TimingSegment* cpy = seg->Copy();

	if (bOnSameRow)
	{
		// delete the existing pointer and replace it
		RageUtil::SafeDelete(cur);
		vSegs[index] = cpy;
	}
	else
	{
		// copy and insert a new segment
		std::vector<TimingSegment*>::iterator it;
		it = upper_bound(vSegs.begin(), vSegs.end(), cpy, ts_less());
		vSegs.insert(it, cpy);
	}
}