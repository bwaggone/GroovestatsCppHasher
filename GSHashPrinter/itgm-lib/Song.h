#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include "Steps.h"

// A VERY trimmed down song class. It's usually populated by various SM/SSC type
// loaders. It typically contains a vector of "Steps" representing the song contents.
// Instead, we represent it as a vector of the raw data for this program.
class Song {
public:
	Song(std::string path) : song_path_(path) {};
	void AddSteps(const std::string& in, std::string diff, std::string steps_type) {
		steps_.push_back(Steps(in, diff, steps_type));
	}
	std::vector<Steps> GetSteps() { return steps_;  }
	std::string GetBpms() { return bpms_; }
	std::string GetPath() { return song_path_; }
	void Song::SetBpms(const std::string& in);
	void SetGSHashes();

private:
	std::string song_path_;
	std::vector<Steps> steps_;
	std::string bpms_;
};