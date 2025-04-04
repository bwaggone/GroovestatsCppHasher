#include "BackgroundUtil.h"
#include "Song.h"
#include "RageUtil.h"

const std::string& InstrumentTrackToString(InstrumentTrack it) {
	return "";
};

InstrumentTrack StringToInstrumentTrack(const std::string& s) {
	static const std::unordered_map<std::string, InstrumentTrack> instrument_map = {
		{"InstrumentTrack_Guitar", InstrumentTrack_Guitar},
		{"InstrumentTrack_Rhythm", InstrumentTrack_Rhythm},
		{"InstrumentTrack_Bass", InstrumentTrack_Bass},
		{"InstrumentTrack_Invalid", InstrumentTrack_Invalid},
		{"NUM_InstrumentTrack", NUM_InstrumentTrack},
	};
	auto it = instrument_map.find(s);
	if (it != instrument_map.end()) {
		return it->second;
	}
	return InstrumentTrack_Invalid;
};

Steps* Song::CreateSteps() {
	Steps* steps = new Steps(this);
	InitSteps(steps);
	return steps;
}

void Song::InitSteps(Steps* steps) {
	steps->SetDisplayBPM(display_bpm_);
	steps->SetMinBPM(this->min_bpm_);
	steps->SetMaxBPM(this->max_bpm_);
}

void Song::AddSteps(Steps* steps)
{
	// Songs of unknown stepstype are saved as a forwards compatibility feature
	// so that editing a simfile made by a future version that has a new style
	// won't delete those steps. -Kyz
	if (steps->GetStepsTypeEnum() != enums::StepsType_Invalid)
	{
		m_vpSteps.push_back(steps);
		//ASSERT_M(pSteps->m_StepsType < NUM_StepsType, ssprintf("%i", pSteps->m_StepsType));
		//m_vpStepsByType[pSteps->m_StepsType].push_back(pSteps);
	}
	else
	{
		//m_UnknownStyleSteps.push_back(pSteps);
	}
}


void Song::SetGSHashes() {
	for (Steps& step : steps_) {
		step.CalculateAndSetGSHash(bpms_);
	}
}


void Song::SetBpms(const std::string& in) {
	// Trim bpms to three digits after the decimal.
	// ex. 0.00000=165.00000 -> 0.000=165.000
	std::vector<std::string> all_bpms;
	std::vector<std::string> bpms_out;
	std::istringstream ss(in);
	std::string bpm_string;

	// Split on any commas
	while (std::getline(ss, bpm_string, ',')) {
		all_bpms.push_back(bpm_string);
	}
	// Now split on the equals, and reassemble
	for (std::string& bpm : all_bpms) {
		auto equals_location = bpm.find('=');
		float bpm1 = std::stof(bpm.substr(0, equals_location));
		float bpm2 = std::stof(bpm.substr(equals_location + 1, bpm.length() - equals_location));
		std::stringstream output_bpm;
		output_bpm << std::fixed << std::setprecision(3) << bpm1 << "=" << bpm2;
		bpm = output_bpm.str();
	}

	bpms_ = util::join(",", all_bpms);
}

void Song::SetSpecifiedLastSecond(const float f)
{
	this->specified_last_second = f;
}

/*const std::vector<BackgroundChange>& Song::GetBackgroundChanges(BackgroundLayer bl) const
{
	return *(m_BackgroundChanges[bl]);
}

std::vector<BackgroundChange>& Song::GetBackgroundChanges(BackgroundLayer bl)
{
	return *(m_BackgroundChanges[bl]);
}

void Song::AddBackgroundChange(BackgroundLayer iLayer, BackgroundChange seg)
{
	// Delete old background change at this start beat, if any.
	auto& changes = GetBackgroundChanges(iLayer);
	for (std::vector<BackgroundChange>::iterator bgc = changes.begin(); bgc != changes.end(); ++bgc)
	{
		if (bgc->m_fStartBeat == seg.m_fStartBeat)
		{
			GetBackgroundChanges(iLayer).erase(bgc);
			break;
		}
	}

	// TODO bwaggone: This
	//BackgroundUtil::AddBackgroundChange(GetBackgroundChanges(iLayer), seg);
}
*/
