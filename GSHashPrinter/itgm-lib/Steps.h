#ifndef STEPS_H
#define STEPS_H

#include <string>
#include "ConstantsAndTypes.h"
#include "TimingData.h"

class Song;

class Steps {
public:
	// Closer to ITGm
	Steps(Song* song);
	void SetDisplayBPM(enums::DisplayBPM in) { display_bpm_ = in; }
	void SetMinBPM(float in) { min_bpm_ = in; }
	void SetMaxBPM(float in) { max_bpm_ = in; }

	// Not faithful to ITGm
	Steps(std::string raw_chart, std::string difficulty, std::string steps_type) : raw_chart_(raw_chart), difficulty_(difficulty), steps_type_(steps_type) {};
	std::string GetRawChart() { return raw_chart_; }
	std::string GetDifficulty() { return difficulty_; }
	std::string GetStepsType() { return steps_type_; }

	void SetGSHash(std::string hash) { gsHash_ = hash; }
	std::string GetGSHash() { return gsHash_; }
	void CalculateAndSetGSHash(std::string bpm_string);

	// Minimizes the chart string into part of the hash that's used for GrooveStats.
	std::string MinimizeChartString();
	void SetSMNoteData(const std::string& notes_comp_);

	TimingData timing_data_;

private:

	// New, closer to ITGm
	Song* song_;
	enums::DisplayBPM display_bpm_;
	float min_bpm_;
	float max_bpm_;

	// TODO: Attacks
	// std::vector<std::string> attack_string_;
	// AttackArray attack_array_;

	// Old, not faithful to ITGm
	std::string raw_chart_;
	std::string difficulty_;
	std::string steps_type_;
	std::string gsHash_ = "";
};


#endif