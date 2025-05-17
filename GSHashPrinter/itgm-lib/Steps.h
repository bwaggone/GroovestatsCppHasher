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
	void SetDisplayBPM(DisplayBPM in) { display_bpm_ = in; }
	void SetMinBPM(float in) { min_bpm_ = in; }
	void SetMaxBPM(float in) { max_bpm_ = in; }
	void SetChartName(const std::string cn) { chart_name_ = cn; }
	void SetStepsTypeStr(const std::string steps_type) { steps_type_str_ = steps_type; }
	void SetStepsType(const std::string steps_type);
	void SetChartStyle(std::string chart_style);
	void DeAutogen(bool copy_note_data = true);
	bool Steps::MakeValidEditDescription(std::string& preferred_description);
	void SetDifficulty(Difficulty dc) { SetDifficultyAndDescription(dc, GetDescription()); }
	void SetDescription(std::string desc) { SetDifficultyAndDescription(this->GetDifficulty(), desc); }
	void Steps::SetDifficultyAndDescription(Difficulty difficulty, std::string desc);
	Difficulty GetDifficulty() { return difficulty_; }
	std::string GetDescription() { return description_; }
	void SetMeter(int meter);
	void CalculateGrooveStatsHash();
	int GetMeter() const { return meter_; }
	float Steps::PredictMeter() const;
	void Steps::Decompress();
	void Steps::Decompress() const;
	void GetNoteData(NoteData& noteDataOut) const;
	void GetSMNoteData(std::string& notes_comp_out) const;


	std::string GetGSHash() { return groovestats_hash_; }

	// Minimizes the chart string into part of the hash that's used for GrooveStats.
	std::string MinimizedChartString();
	void SetSMNoteData(const std::string& notes_comp_);
	void TidyUpData();
	void SetFilename(std::string in) { filename_ = in; }
	StepsType GetStepsTypeEnum() { return steps_type_enum_; }
	std::string GetStepsTypeString() { return steps_type_str_; }

	const TimingData* GetTimingData() const;
	TimingData* GetTimingData() { return const_cast<TimingData*>(static_cast<const Steps*>(this)->GetTimingData()); };
	NoteData note_data_;
	TimingData timing_data_;

private:

	// New, closer to ITGm
	Song* song_;
	DisplayBPM display_bpm_;
	float min_bpm_;
	float max_bpm_;
	mutable bool m_bNoteDataIsFilled;
	mutable std::string m_sNoteDataCompressed;
	StepsType steps_type_enum_;
	std::string filename_;
	std::string chart_name_;
	std::string steps_type_str_;
	StepsType steps_type_;
	std::string chart_style_;
	Difficulty difficulty_;
	std::string description_;
	int meter_;
	std::string groovestats_hash_;
	int groovestats_version_;

	// TODO: Attacks
	// std::vector<std::string> attack_string_;
	// AttackArray attack_array_;
};


#endif