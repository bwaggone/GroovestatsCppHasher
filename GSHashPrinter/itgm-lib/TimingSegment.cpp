#include <vector>

#include "TimingSegment.h"

std::vector<float> TimeSignatureSegment::GetValues() const
{
	std::vector<float> ret;
	ret.push_back(GetNum());
	ret.push_back(GetDen());
	return ret;
}

std::vector<float> SpeedSegment::GetValues() const
{
	std::vector<float> ret;
	ret.push_back(GetRatio());
	ret.push_back(GetDelay());
	ret.push_back(GetUnit());
	return ret;
}