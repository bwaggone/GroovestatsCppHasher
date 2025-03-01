#ifndef SONG_H
#define SONG_H

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include "Steps.h"
#include "TimingData.h"

// A VERY trimmed down song class. It's usually populated by various SM/SSC type
// loaders. It typically contains a vector of "Steps" representing the song contents.
// Instead, we represent it as a vector of the raw data for this program.
class Song {
public:
	Song(std::string path) : song_path_(path) {};

	// Closer to ITGm
	Steps* CreateSteps();
	void InitSteps(Steps* steps);

	void SetDisplayBPM(enums::DisplayBPM in) { display_bpm_ = in; }
	void SetMinBPM(float in) { min_bpm_ = in; }
	void SetMaxBPM(float in) { max_bpm_ = in; }

	float GetMinBPM() { return min_bpm_; }
	float GetMaxBPM() { return max_bpm_; }


	// Not faithful to ITGm
	void AddSteps(const std::string& in, std::string diff, std::string steps_type) {
		steps_.push_back(Steps(in, diff, steps_type));
	}
	std::vector<Steps> GetSteps() { return steps_; }
	std::string GetBpms() { return bpms_; }
	std::string GetPath() { return song_path_; }
	void Song::SetBpms(const std::string& in);
	void SetGSHashes();


	TimingData timing_data_;

private:
	// Closer to ITGm
	enums::DisplayBPM display_bpm_;
	float min_bpm_;
	float max_bpm_;

	// Not faithful to ITGm
	std::string song_path_;
	std::vector<Steps> steps_;
	std::string bpms_;
};

#endif