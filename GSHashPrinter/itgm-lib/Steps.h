#include <string>

class Steps {
public:
	Steps(std::string raw_chart, std::string difficulty, std::string steps_type) : raw_chart_(raw_chart), difficulty_(difficulty), steps_type_(steps_type) {};
	std::string GetRawChart() { return raw_chart_; }
	std::string GetDifficulty() { return difficulty_; }
	std::string GetStepsType() { return steps_type_; }

	void SetGSHash(std::string hash) { gsHash_ = hash; }
	std::string GetGSHash() { return gsHash_; }
	void CalculateAndSetGSHash(std::string bpm_string);

	// Minimizes the chart string into part of the hash that's used for GrooveStats.
	std::string MinimizeChartString();

private:
	std::string raw_chart_;
	std::string difficulty_;
	std::string steps_type_;
	std::string gsHash_ = "";
};