#ifndef STEPS_H
#define STEPS_H

#include <string>

#include "ConstantsAndTypes.h"
#include "NoteData.h"
#include "TimingData.h"

class Song;

class Steps {
public:
	// Closer to ITGm
	Steps(Song* song);
	void SetDisplayBPM(enums::DisplayBPM in) { display_bpm_ = in; }
	void SetMinBPM(float in) { min_bpm_ = in; }
	void SetMaxBPM(float in) { max_bpm_ = in; }
	void SetChartName(const std::string cn) { chart_name_ = cn; }
	void SetStepsTypeStr(const std::string steps_type) { steps_type_str_ = steps_type; }
	void SetStepsType(const std::string steps_type);
	void SetChartStyle(std::string chart_style);
	void DeAutogen(bool copy_note_data = true);
	bool Steps::MakeValidEditDescription(std::string& preferred_description);
	void SetDifficulty(enums::Difficulty dc) { SetDifficultyAndDescription(dc, GetDescription()); }
	void SetDescription(std::string desc) { SetDifficultyAndDescription(this->GetDifficulty(), desc); }
	void Steps::SetDifficultyAndDescription(enums::Difficulty difficulty, std::string desc);
	enums::Difficulty GetDifficulty() { return difficulty_; }
	std::string GetDescription() { return description_; }
	void SetMeter(int meter);
	void CalculateGrooveStatsHash();
	int GetMeter() const { return meter_; }
	float Steps::PredictMeter() const;
	void Steps::Decompress();
	void Steps::Decompress() const;
	void GetNoteData(NoteData& noteDataOut) const;
	void GetSMNoteData(std::string& notes_comp_out) const;

	// Not faithful to ITGm
	//Steps(std::string raw_chart, std::string difficulty, std::string steps_type) : raw_chart_(raw_chart), difficulty_(difficulty), steps_type_(steps_type) {};
	std::string GetRawChart() { return raw_chart_; }

	void SetGSHash(std::string hash) { gsHash_ = hash; }
	std::string GetGSHash() { return gsHash_; }
	void CalculateAndSetGSHash(std::string bpm_string);

	// Minimizes the chart string into part of the hash that's used for GrooveStats.
	std::string MinimizedChartString();
	void SetSMNoteData(const std::string& notes_comp_);
	void TidyUpData();
	void SetFilename(std::string in) { filename_ = in; }
	enums::StepsType GetStepsTypeEnum() { return steps_type_enum_; }

	const TimingData* GetTimingData() const;
	TimingData* GetTimingData() { return const_cast<TimingData*>(static_cast<const Steps*>(this)->GetTimingData()); };
	NoteData note_data_;
	TimingData timing_data_;

private:

	// New, closer to ITGm
	Song* song_;
	enums::DisplayBPM display_bpm_;
	float min_bpm_;
	float max_bpm_;
	mutable bool m_bNoteDataIsFilled;
	mutable std::string m_sNoteDataCompressed;
	enums::StepsType steps_type_enum_;
	std::string filename_;
	std::string chart_name_;
	std::string steps_type_str_;
	enums::StepsType steps_type_;
	std::string chart_style_;
	enums::Difficulty difficulty_;
	std::string description_;
	int meter_;
	std::string groovestats_hash_;

	// TODO: Attacks
	// std::vector<std::string> attack_string_;
	// AttackArray attack_array_;

	// Old, not faithful to ITGm
	std::string raw_chart_;
	std::string gsHash_ = "";
};


#endif