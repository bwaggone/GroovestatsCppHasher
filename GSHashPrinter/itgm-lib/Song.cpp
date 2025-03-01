#include "Song.h"
#include "RageUtil.h"


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