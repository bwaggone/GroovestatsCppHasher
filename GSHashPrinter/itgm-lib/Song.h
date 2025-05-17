#ifndef SONG_H
#define SONG_H

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "Attack.h"
#include "Steps.h"
#include "TimingData.h"

enum BackgroundLayer
{
	BACKGROUND_LAYER_1,
	BACKGROUND_LAYER_2,
	//BACKGROUND_LAYER_3, // StepCollection get
	NUM_BackgroundLayer,
	BACKGROUND_LAYER_Invalid
};

enum InstrumentTrack
{
	InstrumentTrack_Guitar,
	InstrumentTrack_Rhythm,
	InstrumentTrack_Bass,
	NUM_InstrumentTrack,
	InstrumentTrack_Invalid
};

const std::string& InstrumentTrackToString(InstrumentTrack it);

InstrumentTrack StringToInstrumentTrack(const std::string& s);

class Song {
public:
	Song(std::string path) : song_path_(path) {};

	Steps* CreateSteps();
	void InitSteps(Steps* steps);

	void SetDisplayBPM(DisplayBPM in) { display_bpm_ = in; }
	void SetMinBPM(float in) { min_bpm_ = in; }
	void SetMaxBPM(float in) { max_bpm_ = in; }

	float GetMinBPM() { return min_bpm_; }
	float GetMaxBPM() { return max_bpm_; }

	void AddSteps(Steps* steps);

	//void AddSteps(const std::string& in, std::string diff, std::string steps_type) {
	//	steps_.push_back(Steps(in, diff, steps_type));
	//}
	std::vector<Steps*> GetSteps() { return steps_; }
	std::string GetBpms() { return bpms_; }
	std::string GetPath() { return song_path_; }
	void Song::SetBpms(const std::string& in);
	void SetSpecifiedLastSecond(const float f);
	void SetGSHashes();
	void SetFirstSecond(const float f) { firstSecond = f; }
	void SetLastSecond(const float f) { lastSecond = f; }
	void SetFileName(const std::string fn) { filename = fn; }
	//void AddBackgroundChange(BackgroundLayer iLayer, BackgroundChange seg);
	//const std::vector<BackgroundChange>& GetBackgroundChanges(BackgroundLayer bl) const;
	//std::vector<BackgroundChange>& GetBackgroundChanges(BackgroundLayer bl);

	std::string filename;
	TimingData timing_data_;
	float version;
	std::string main_title;
	std::string subtitle;
	std::string artist;
	std::string main_title_transliteration;
	std::string subtitle_transliteration;
	std::string artist_transliteration;
	std::string genre;
	std::string origin;
	std::string credit;
	std::string banner_file;
	std::string background_file;
	std::string preview_video_file;
	std::string jacket_file;
	std::string cd_file;
	std::string disc_file;
	std::string lyrics_file;
	std::string cd_title_file;
	std::string music_file;
	std::string preview_file;
	float music_length_seconds;
	std::vector<std::string> instrument_track_files;
	float music_sample_start_seconds;
	float music_sample_length_seconds;
	float firstSecond;
	float lastSecond;
	bool has_music;
	bool has_banner;
	enum SelectionDisplay
	{
		SHOW_ALWAYS,	/**< always show on the wheel. */
		SHOW_NEVER	/**< never show on the wheel (unless song hiding is turned off). */
	} selection_display;
	std::vector<std::string> keysound_files;
	AttackArray m_Attacks;
	std::vector<std::string>	m_sAttackString;


private:
	DisplayBPM display_bpm_;
	float min_bpm_;
	float max_bpm_;
	float specified_last_second;
	std::vector<Steps*> steps_;
	//std::vector<BackgroundChange>*	m_BackgroundChanges[NUM_BackgroundLayer];

	std::string song_path_;
	std::string bpms_;
};

#endif