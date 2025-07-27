#include <algorithm>
#include <cstddef>
#include <map>
#include <vector>
#include <iostream>

#include "BackgroundUtil.h"
#include "NotesLoaderSSC.h"
#include "RageUtil.h"
#include "Song.h"

struct StepsTagInfo
{
	SSCLoader* loader;
	Song* song;
	Steps* steps;
	TimingData* timing;
	const MsdFile::value_t* params;
	const std::string& path;
	bool has_own_timing;
	bool ssc_format;
	bool for_load_edit = false;
	StepsTagInfo(SSCLoader* l, Song* s, const std::string& p)
		: loader(l), song(s), path(p), has_own_timing(false)
	{}
};
struct SongTagInfo
{
	SSCLoader* loader;
	Song* song;
	const MsdFile::value_t* params;
	const std::string& path;
	SongTagInfo(SSCLoader* l, Song* s, const std::string& p)
		: loader(l), song(s), path(p)
	{}
};

// LoadNoteDataFromSimfile uses LoadNoteDataTagIDs because its parts operate
// on state variables internal to the function.
enum LoadNoteDataTagIDs
{
	LNDID_version,
	LNDID_stepstype,
	LNDID_chartname,
	LNDID_description,
	LNDID_difficulty,
	LNDID_meter,
	LNDID_credit,
	LNDID_notes,
	LNDID_notes2,
	LNDID_notedata
};

typedef void (*steps_tag_func_t)(StepsTagInfo& info);
typedef void (*song_tag_func_t)(SongTagInfo& info);

void SetVersion(SongTagInfo& info)
{
	info.song->version = std::stof((*info.params)[1]);
}
void SetTitle(SongTagInfo& info)
{
	info.song->main_title = (*info.params)[1];
	info.loader->SetSongTitle((*info.params)[1]);
}
void SetSubtitle(SongTagInfo& info)
{
	info.song->subtitle = (*info.params)[1];
}
void SetArtist(SongTagInfo& info)
{
	info.song->artist = (*info.params)[1];
}
void SetMainTitleTranslit(SongTagInfo& info)
{
	info.song->main_title_transliteration = (*info.params)[1];
}
void SetSubtitleTranslit(SongTagInfo& info)
{
	info.song->subtitle_transliteration = (*info.params)[1];
}
void SetArtistTranslit(SongTagInfo& info)
{
	info.song->artist_transliteration = (*info.params)[1];
}
void SetGenre(SongTagInfo& info)
{
	info.song->genre = (*info.params)[1];
}
void SetOrigin(SongTagInfo& info)
{
	info.song->origin = (*info.params)[1];
}
void SetCredit(SongTagInfo& info)
{
	info.song->credit = (*info.params)[1];
	util::Trim(info.song->credit);
}
void SetBanner(SongTagInfo& info)
{
	info.song->banner_file = (*info.params)[1];
}
void SetBackground(SongTagInfo& info)
{
	info.song->background_file = (*info.params)[1];
}
void SetPreviewVid(SongTagInfo& info)
{
	info.song->preview_video_file = (*info.params)[1];
}
void SetJacket(SongTagInfo& info)
{
	info.song->jacket_file = (*info.params)[1];
}
void SetCDImage(SongTagInfo& info)
{
	info.song->cd_file = (*info.params)[1];
}
void SetDiscImage(SongTagInfo& info)
{
	info.song->disc_file = (*info.params)[1];
}
void SetLyricsPath(SongTagInfo& info)
{
	info.song->lyrics_file = (*info.params)[1];
}
void SetCDTitle(SongTagInfo& info)
{
	info.song->cd_title_file = (*info.params)[1];
}
void SetMusic(SongTagInfo& info)
{
	info.song->music_file = (*info.params)[1];
}
void SetPreview(SongTagInfo& info)
{
	info.song->preview_file = (*info.params)[1];
}
void SetInstrumentTrack(SongTagInfo& info)
{
	info.loader->ProcessInstrumentTracks(*info.song, (*info.params)[1]);
}
void SetMusicLength(SongTagInfo& info)
{
	info.song->music_length_seconds = std::stof((*info.params)[1]);
}
void SetLastSecondHint(SongTagInfo& info)
{
	info.song->SetSpecifiedLastSecond(std::stof((*info.params)[1]));
}
void SetSampleStart(SongTagInfo& info)
{
	info.song->music_sample_start_seconds = util::HHMMSSToSeconds((*info.params)[1]);
}
void SetSampleLength(SongTagInfo& info)
{
	info.song->music_sample_length_seconds = util::HHMMSSToSeconds((*info.params)[1]);
}
void SetSelectable(SongTagInfo& info)
{
	std::string tag = util::upper((*info.params)[1]);
	if (tag == "YES")
	{
		info.song->selection_display = info.song->SHOW_ALWAYS;
	}
	else if (tag == "NO")
	{
		info.song->selection_display = info.song->SHOW_NEVER;
	}
	// ROULETTE from 3.9 is no longer in use.
	else if (tag == "ROULETTE")
	{
		info.song->selection_display = info.song->SHOW_ALWAYS;
	}
	/* The following two cases are just fixes to make sure simfiles that
	 * used 3.9+ features are not excluded here */
	else if (tag == "ES" || tag == "OMES")
	{
		info.song->selection_display = info.song->SHOW_ALWAYS;
	}
	else if (std::stof(tag) > 0)
	{
		info.song->selection_display = info.song->SHOW_ALWAYS;
	}
	else
	{
		std::cerr << "Song file " << info.path << " has an unknown #SELECTABLE value, \"%s\"; ignored." << (*info.params)[1].c_str() << "\n";
	}
}
void SetBGChanges(SongTagInfo& info)
{
	info.loader->ProcessBGChanges(*info.song, (*info.params)[0], info.path, (*info.params)[1]);
}
void SetFGChanges(SongTagInfo& info)
{
	std::vector<std::vector<std::string> > aFGChanges;
	info.loader->ParseBGChangesString((*info.params)[1], aFGChanges, info.song->GetPath());

	for (const auto& b : aFGChanges)
	{
		BackgroundChange change;
		// TODO bwaggone: This
		//if (info.loader->LoadFromBGChangesVector(change, b))
		//	info.song->AddForegroundChange(change);
	}
}
void SetKeysounds(SongTagInfo& info)
{
	std::string keysounds = (*info.params)[1];
	if (keysounds.length() >= 2 && keysounds.substr(0, 2) == "\\#")
	{
		keysounds = keysounds.substr(1);
	}
	util::split(keysounds, ",", info.song->keysound_files);
}
void SetAttacks(SongTagInfo& info)
{
	info.loader->ProcessAttackString(info.song->m_sAttackString, (*info.params));
	info.loader->ProcessAttacks(info.song->m_Attacks, (*info.params));
}
void SetOffset(SongTagInfo& info)
{
	info.song->timing_data_.beat0OffsetInSeconds_ = std::stof((*info.params)[1]);
}
void SetSongStops(SongTagInfo& info)
{
	info.loader->ProcessStops(info.song->timing_data_, (*info.params)[1]);
}
void SetSongDelays(SongTagInfo& info)
{
	info.loader->ProcessDelays(info.song->timing_data_, (*info.params)[1]);
}
void SetSongBPMs(SongTagInfo& info)
{
	info.loader->ProcessBPMs(info.song->timing_data_, (*info.params)[1]);
}
void SetSongWarps(SongTagInfo& info)
{
	info.loader->ProcessWarps(info.song->timing_data_, (*info.params)[1], info.song->version);
}
void SetSongLabels(SongTagInfo& info)
{
	info.loader->ProcessLabels(info.song->timing_data_, (*info.params)[1]);
}
void SetSongTimeSignatures(SongTagInfo& info)
{
	info.loader->ProcessTimeSignatures(info.song->timing_data_, (*info.params)[1]);
}
void SetSongTickCounts(SongTagInfo& info)
{
	info.loader->ProcessTickcounts(info.song->timing_data_, (*info.params)[1]);
}
void SetSongCombos(SongTagInfo& info)
{
	info.loader->ProcessCombos(info.song->timing_data_, (*info.params)[1]);
}
void SetSongSpeeds(SongTagInfo& info)
{
	info.loader->ProcessSpeeds(info.song->timing_data_, (*info.params)[1]);
}
void SetSongScrolls(SongTagInfo& info)
{
	info.loader->ProcessScrolls(info.song->timing_data_, (*info.params)[1]);
}
void SetSongFakes(SongTagInfo& info)
{
	info.loader->ProcessFakes(info.song->timing_data_, (*info.params)[1]);
}
void SetFirstSecond(SongTagInfo& info)
{
		info.song->SetFirstSecond(std::stof((*info.params)[1]));
}
void SetLastSecond(SongTagInfo& info)
{
		info.song->SetLastSecond(std::stof((*info.params)[1]));
}
void SetSongFilename(SongTagInfo& info)
{
		info.song->SetFileName((*info.params)[1]);
}
void SetHasMusic(SongTagInfo& info)
{
	info.song->has_music = std::stoi((*info.params)[1]) != 0;
}
void SetHasBanner(SongTagInfo& info)
{
	info.song->has_banner = std::stoi((*info.params)[1]) != 0;
}

// Functions for steps tags go below this line. -Kyz
/****************************************************************/
void SetStepsVersion(StepsTagInfo& info)
{
	info.song->version = std::stof((*info.params)[1]);
}
void SetChartName(StepsTagInfo& info)
{
	std::string name = (*info.params)[1];
	util::Trim(name);
	info.steps->SetChartName(name);
}
void SetStepsType(StepsTagInfo& info)
{
	info.steps->SetStepsType((*info.params)[1]);
	info.steps->SetStepsTypeStr((*info.params)[1]);
	info.ssc_format = true;
}
void SetChartStyle(StepsTagInfo& info)
{
	info.steps->SetChartStyle((*info.params)[1]);
	info.ssc_format = true;
}
void SetDescription(StepsTagInfo& info)
{
	std::string name = (*info.params)[1];
	util::Trim(name);
	if (info.song->version < VERSION_CHART_NAME_TAG && !info.for_load_edit)
	{
		info.steps->SetChartName(name);
	}
	else
	{
		info.steps->SetDescription(name);
	}
	info.ssc_format = true;
}
void SetDifficulty(StepsTagInfo& info)
{
	info.ssc_format = true;
	std::string diff = (*info.params)[1];
	for (char& c : diff) {
		c = std::tolower(c);
	}

	if (kStringToDifficulty.find(diff) == kStringToDifficulty.end()) {
		info.steps->SetDifficulty(Difficulty_Invalid);
		return;
	}
	info.steps->SetDifficulty(kStringToDifficulty[diff]);
}
void SetMeter(StepsTagInfo& info)
{
	info.steps->SetMeter(std::stoi((*info.params)[1]));
	info.ssc_format = true;
}
/*void SetRadarValues(StepsTagInfo& info)
{
	if (info.from_cache || info.for_load_edit)
	{
		std::vector<std::string> values;
		split((*info.params)[1], ",", values, true);
		// Instead of trying to use the version to figure out how many
		// categories to expect, look at the number of values and split them
		// evenly. -Kyz
		size_t cats_per_player = values.size() / NUM_PlayerNumber;
		RadarValues v[NUM_PLAYERS];
		FOREACH_PlayerNumber(pn)
		{
			for (size_t i = 0; i < cats_per_player; ++i)
			{
				v[pn][i] = std::stof(values[pn * cats_per_player + i]);
			}
		}
		info.steps->SetCachedRadarValues(v);
	}
	else
	{
		// just recalc at time.
	}
	info.ssc_format = true;
}

void SetTechCounts(StepsTagInfo& info)
{
	if (info.from_cache || info.for_load_edit)
	{
		std::vector<std::string> values;
		split((*info.params)[1], ",", values, true);
		std::size_t cats_per_player = values.size() / NUM_PlayerNumber;
		TechCounts v[NUM_PLAYERS];
		FOREACH_PlayerNumber(pn)
		{
			for (std::size_t i = 0; i < cats_per_player; ++i)
			{
				v[pn][i] = std::stof(values[pn * cats_per_player + i]);
			}
		}
		info.steps->SetCachedTechCounts(v);
	}
	else
	{
		// just recalc at time.
	}
	info.ssc_format = true;
}

void SetMeasureInfo(StepsTagInfo& info)
{
	if (info.from_cache || info.for_load_edit)
	{
		std::vector<std::string> values;
		split((*info.params)[1], "|", values, true);

		MeasureInfo v[NUM_PLAYERS];
		FOREACH_PlayerNumber(pn)
		{
			v[pn].FromString(values[pn]);
		}
		info.steps->SetCachedMeasureInfo(v);
	}
	else
	{
		// just recalc at time.
	}
	info.ssc_format = true;
}*/

void SetDisplayBPM(SongTagInfo& info)
{
	// #DISPLAYBPM:[xxx][xxx:xxx]|[*];
	if ((*info.params)[1] == "*")
	{
		info.song->SetDisplayBPM(DISPLAY_BPM_RANDOM);
	}
	else
	{
		info.song->SetDisplayBPM(DISPLAY_BPM_SPECIFIED);
		info.song->SetMinBPM(std::stof((*info.params)[1]));
		if ((*info.params)[2].empty())
		{
			info.song->SetMaxBPM(info.song->GetMinBPM());
		}
		else
		{
			info.song->SetMaxBPM(std::stof((*info.params)[2]));
		}
	}
}

void SetStepsDisplayBPM(StepsTagInfo& info)
{
	// #DISPLAYBPM:[xxx][xxx:xxx]|[*];
	if ((*info.params)[1] == "*")
	{
		info.steps->SetDisplayBPM(DISPLAY_BPM_RANDOM);
	}
	else if ((*info.params)[1] != "")
	{
		info.steps->SetDisplayBPM(DISPLAY_BPM_SPECIFIED);
		float min = std::stof((*info.params)[1]);
		info.steps->SetMinBPM(min);
		if ((*info.params)[2].empty())
		{
			info.steps->SetMaxBPM(min);
		}
		else
		{
			info.steps->SetMaxBPM(std::stof((*info.params)[2]));
		}
	}
}

typedef std::map<std::string, steps_tag_func_t> steps_handler_map_t;
typedef std::map<std::string, song_tag_func_t> song_handler_map_t;
typedef std::map<std::string, LoadNoteDataTagIDs> load_note_data_handler_map_t;

struct ssc_parser_helper_t
{
	steps_handler_map_t steps_tag_handlers;
	song_handler_map_t song_tag_handlers;
	load_note_data_handler_map_t load_note_data_handlers;
	// Unless signed, the comments in this tag list are not by me.  They were
	// moved here when converting from the else if chain. -Kyz
	ssc_parser_helper_t()
	{
		song_tag_handlers["VERSION"] = &SetVersion;
		song_tag_handlers["TITLE"] = &SetTitle;
		song_tag_handlers["SUBTITLE"] = &SetSubtitle;
		song_tag_handlers["ARTIST"] = &SetArtist;
		song_tag_handlers["TITLETRANSLIT"] = &SetMainTitleTranslit;
		song_tag_handlers["SUBTITLETRANSLIT"] = &SetSubtitleTranslit;
		song_tag_handlers["ARTISTTRANSLIT"] = &SetArtistTranslit;
		song_tag_handlers["GENRE"] = &SetGenre;
		song_tag_handlers["ORIGIN"] = &SetOrigin;
		song_tag_handlers["CREDIT"] = &SetCredit;
		song_tag_handlers["BANNER"] = &SetBanner;
		song_tag_handlers["BACKGROUND"] = &SetBackground;
		song_tag_handlers["PREVIEWVID"] = &SetPreviewVid;
		song_tag_handlers["JACKET"] = &SetJacket;
		song_tag_handlers["CDIMAGE"] = &SetCDImage;
		song_tag_handlers["DISCIMAGE"] = &SetDiscImage;
		song_tag_handlers["LYRICSPATH"] = &SetLyricsPath;
		song_tag_handlers["CDTITLE"] = &SetCDTitle;
		song_tag_handlers["MUSIC"] = &SetMusic;
		song_tag_handlers["PREVIEW"] = &SetPreview;
		song_tag_handlers["INSTRUMENTTRACK"] = &SetInstrumentTrack;
		song_tag_handlers["MUSICLENGTH"] = &SetMusicLength;
		song_tag_handlers["LASTSECONDHINT"] = &SetLastSecondHint;
		song_tag_handlers["SAMPLESTART"] = &SetSampleStart;
		song_tag_handlers["SAMPLELENGTH"] = &SetSampleLength;
		song_tag_handlers["DISPLAYBPM"] = &SetDisplayBPM;
		song_tag_handlers["SELECTABLE"] = &SetSelectable;
		// It's a bit odd to have the tag that exists for backwards compatibility
		// in this list and not the replacement, but the BGCHANGES tag has a
		// number on the end, allowing up to NUM_BackgroundLayer tags, so it
		// can't fit in the map. -Kyz
		song_tag_handlers["ANIMATIONS"] = &SetBGChanges;
		song_tag_handlers["FGCHANGES"] = &SetFGChanges;
		song_tag_handlers["KEYSOUNDS"] = &SetKeysounds;
		song_tag_handlers["ATTACKS"] = &SetAttacks;
		song_tag_handlers["OFFSET"] = &SetOffset;
		/* Below are the song based timings that should only be used
		 * if the steps do not have their own timing. */
		song_tag_handlers["STOPS"] = &SetSongStops;
		song_tag_handlers["DELAYS"] = &SetSongDelays;
		song_tag_handlers["BPMS"] = &SetSongBPMs;
		song_tag_handlers["WARPS"] = &SetSongWarps;
		song_tag_handlers["LABELS"] = &SetSongLabels;
		song_tag_handlers["TIMESIGNATURES"] = &SetSongTimeSignatures;
		song_tag_handlers["TICKCOUNTS"] = &SetSongTickCounts;
		song_tag_handlers["COMBOS"] = &SetSongCombos;
		song_tag_handlers["SPEEDS"] = &SetSongSpeeds;
		song_tag_handlers["SCROLLS"] = &SetSongScrolls;
		song_tag_handlers["FAKES"] = &SetSongFakes;
		/* The following are cache tags. Never fill their values
		 * directly: only from the cached version. */
		song_tag_handlers["FIRSTSECOND"] = &SetFirstSecond;
		song_tag_handlers["LASTSECOND"] = &SetLastSecond;
		song_tag_handlers["SONGFILENAME"] = &SetSongFilename;
		song_tag_handlers["HASMUSIC"] = &SetHasMusic;
		song_tag_handlers["HASBANNER"] = &SetHasBanner;
		/* Tags that no longer exist, listed for posterity.  May their names
		 * never be forgotten for their service to Stepmania. -Kyz
		 * LASTBEATHINT: // unable to parse due to tag position. Ignore.
		 * MUSICBYTES: // ignore
		 * FIRSTBEAT: // no longer used.
		 * LASTBEAT: // no longer used.
		 */

		steps_tag_handlers["VERSION"] = &SetStepsVersion;
		steps_tag_handlers["CHARTNAME"] = &SetChartName;
		steps_tag_handlers["STEPSTYPE"] = &SetStepsType;
		steps_tag_handlers["CHARTSTYLE"] = &SetChartStyle;
		steps_tag_handlers["DESCRIPTION"] = &SetDescription;
		steps_tag_handlers["DIFFICULTY"] = &SetDifficulty;
		steps_tag_handlers["METER"] = &SetMeter;
		//steps_tag_handlers["RADARVALUES"] = &SetRadarValues;
		//steps_tag_handlers["CREDIT"] = &SetCredit;
		//steps_tag_handlers["MUSIC"] = &SetStepsMusic;
		//steps_tag_handlers["BPMS"] = &SetStepsBPMs;
		//steps_tag_handlers["STOPS"] = &SetStepsStops;
		//steps_tag_handlers["DELAYS"] = &SetStepsDelays;
		//steps_tag_handlers["TIMESIGNATURES"] = &SetStepsTimeSignatures;
		//steps_tag_handlers["TICKCOUNTS"] = &SetStepsTickCounts;
		//steps_tag_handlers["COMBOS"] = &SetStepsCombos;
		//steps_tag_handlers["WARPS"] = &SetStepsWarps;
		//steps_tag_handlers["SPEEDS"] = &SetStepsSpeeds;
		//steps_tag_handlers["SCROLLS"] = &SetStepsScrolls;
		//steps_tag_handlers["FAKES"] = &SetStepsFakes;
		//steps_tag_handlers["LABELS"] = &SetStepsLabels;
		//steps_tag_handlers["TECHCOUNTS"] = &SetTechCounts;
		//steps_tag_handlers["MEASUREINFO"] = &SetMeasureInfo;
		//steps_tag_handlers["GROOVESTATSHASH"] = &SetGrooveStatsHash;
		//steps_tag_handlers["GROOVESTATSHASHVERSION"] = &SetGrooveStatsHashVersion;

		/* If this is called, the chart does not use the same attacks
		 * as the Song's timing. No other changes are required. */
		//steps_tag_handlers["ATTACKS"] = &SetStepsAttacks;
		//steps_tag_handlers["OFFSET"] = &SetStepsOffset;
		steps_tag_handlers["DISPLAYBPM"] = &SetStepsDisplayBPM;

		//load_note_data_handlers["VERSION"] = LNDID_version;
		//load_note_data_handlers["STEPSTYPE"] = LNDID_stepstype;
		//load_note_data_handlers["CHARTNAME"] = LNDID_chartname;
		//load_note_data_handlers["DESCRIPTION"] = LNDID_description;
		//load_note_data_handlers["DIFFICULTY"] = LNDID_difficulty;
		//load_note_data_handlers["METER"] = LNDID_meter;
		//load_note_data_handlers["CREDIT"] = LNDID_credit;
		//load_note_data_handlers["NOTES"] = LNDID_notes;
		//load_note_data_handlers["NOTES2"] = LNDID_notes2;
		//load_note_data_handlers["NOTEDATA"] = LNDID_notedata;
	}
};

ssc_parser_helper_t parser_helper;

void SSCLoader::ProcessBPMs(TimingData& out, const std::string sParam) {
	std::vector<std::string> arrayBPMExpressions;
	util::split(sParam, ",", arrayBPMExpressions);

	for (unsigned b = 0; b < arrayBPMExpressions.size(); b++)
	{
		std::vector<std::string> arrayBPMValues;
		util::split(arrayBPMExpressions[b], "=", arrayBPMValues);
		if (arrayBPMValues.size() != 2)
		{
			// Invalid BPM string
			continue;
		}

		const float fBeat = std::stof(arrayBPMValues[0]);
		const float fNewBPM = std::stof(arrayBPMValues[1]);
		if (fBeat >= 0 && fNewBPM > 0)
		{
			out.AddSegment(BPMSegment(BeatToNoteRow(fBeat), fNewBPM));
		}
		else
		{
			// Invalid BPM
			std::cout << "Song file" <<
				this->GetSongTitle() <<
				"has an invalid BPM at beat %f, BPM %f." <<
				fBeat, fNewBPM;
		}
	}

}

void SSCLoader::ProcessStops(TimingData& out, const std::string sParam)
{
	std::vector<std::string> arrayStopExpressions;
	util::split(sParam, ",", arrayStopExpressions);

	for (unsigned b = 0; b < arrayStopExpressions.size(); b++)
	{
		std::vector<std::string> arrayStopValues;
		util::split(arrayStopExpressions[b], "=", arrayStopValues);
		if (arrayStopValues.size() != 2)
		{
			// Invalid stops
			continue;
		}

		const float fBeat = std::stof(arrayStopValues[0]);
		const float fNewStop = std::stof(arrayStopValues[1]);
		if (fBeat >= 0 && fNewStop > 0)
			out.AddSegment(StopSegment(BeatToNoteRow(fBeat), fNewStop));
		else
		{
			// Invalid stop beat
		}
	}
}

void SSCLoader::ProcessWarps(TimingData& out, const std::string sParam, const float fVersion)
{
	std::vector<std::string> arrayWarpExpressions;
	util::split(sParam, ",", arrayWarpExpressions);

	for (unsigned b = 0; b < arrayWarpExpressions.size(); b++)
	{
		std::vector<std::string> arrayWarpValues;
		util::split(arrayWarpExpressions[b], "=", arrayWarpValues);
		if (arrayWarpValues.size() != 2)
		{
			std::cout << "Song file", this->GetSongTitle(), "has an invalid #WARPS value.\n";
			continue;
		}

		const float fBeat = std::stof(arrayWarpValues[0]);
		const float fNewBeat = std::stof(arrayWarpValues[1]);
		// Early versions were absolute in beats. They should be relative.
		if ((fVersion < VERSION_SPLIT_TIMING && fNewBeat > fBeat))
		{
			out.AddSegment(WarpSegment(BeatToNoteRow(fBeat), fNewBeat - fBeat));
		}
		else if (fNewBeat > 0)
			out.AddSegment(WarpSegment(BeatToNoteRow(fBeat), fNewBeat));
		else
		{
			std::cout << "Song file" << this->GetSongTitle() << "has an invalid #WARPS value at beat " << fBeat << "\n";
		}
	}
}

void SSCLoader::ProcessLabels(TimingData& out, const std::string sParam)
{
	std::vector<std::string> arrayLabelExpressions;
	util::split(sParam, ",", arrayLabelExpressions);

	for (unsigned b = 0; b < arrayLabelExpressions.size(); b++)
	{
		std::vector<std::string> arrayLabelValues;
		util::split(arrayLabelExpressions[b], "=", arrayLabelValues);
		if (arrayLabelValues.size() != 2)
		{
			std::cout << "Song file", this->GetSongTitle(), "has an invalid #LABELS value.\n";
			continue;
		}

		const float fBeat = std::stof(arrayLabelValues[0]);
		std::string sLabel = arrayLabelValues[1];
		util::TrimRight(sLabel);
		if (fBeat >= 0.0f)
			out.AddSegment(LabelSegment(BeatToNoteRow(fBeat), sLabel));
		else
		{
			std::cout << "Song file" << this->GetSongTitle() << "has an invalid #LABELS value at beat " << fBeat << "\n";
		}

	}
}

void SSCLoader::ProcessCombos(TimingData& out, const std::string line, const int rowsPerBeat)
{
	std::vector<std::string> arrayComboExpressions;
	util::split(line, ",", arrayComboExpressions);

	for (unsigned f = 0; f < arrayComboExpressions.size(); f++)
	{
		std::vector<std::string> arrayComboValues;
		util::split(arrayComboExpressions[f], "=", arrayComboValues);
		unsigned size = arrayComboValues.size();
		if (size < 2)
		{
			std::cout << "Song file" <<
				this->GetSongTitle()
				<< " has an invalid #COMBOS value \"%s\" (must have at least one '='), ignored." <<
				arrayComboExpressions[f].c_str();
			continue;
		}
		const float fComboBeat = std::stof(arrayComboValues[0]);
		const int iCombos = std::stoi(arrayComboValues[1]);
		const int iMisses = (size == 2 ? iCombos : std::stoi(arrayComboValues[2]));
		out.AddSegment(ComboSegment(BeatToNoteRow(fComboBeat), iCombos, iMisses));
	}
}

void SSCLoader::ProcessScrolls(TimingData& out, const std::string sParam)
{
	std::vector<std::string> vs1;
	util::split(sParam, ",", vs1);

	for (std::string const& s1 : vs1)
	{
		std::vector<std::string> vs2;
		util::split(s1, "=", vs2);

		if (vs2.size() < 2)
		{
			std::cout << "Song file" <<
				this->GetSongTitle() <<
				"has an scroll change with %i values." <<
				static_cast<int>(vs2.size());
			continue;
		}

		const float fBeat = std::stof(vs2[0]);
		const float fRatio = std::stof(vs2[1]);

		if (fBeat < 0)
		{
			std::cout << "Song file" <<
				this->GetSongTitle() <<
				"has an scroll change with beat %f." <<
				fBeat;
			continue;
		}

		out.AddSegment(ScrollSegment(BeatToNoteRow(fBeat), fRatio));
	}
}

bool SSCLoader::LoadFromSimfile(const std::string& sPath, Song& out) {
	MsdFile msd;
	if (!msd.ReadFile(sPath, true))
	{
		return false;
	}

	out.filename = sPath;

	int state = GETTING_SONG_INFO;
	const unsigned values = msd.GetNumValues();
	Steps* pNewNotes = nullptr;
	TimingData stepsTiming;

	SongTagInfo song_tag(&*this, &out, sPath);
	StepsTagInfo step_tag(&*this, &out, sPath);

	for (unsigned i = 0; i < values; i++)
	{
		const MsdFile::value_t& params = msd.GetValue(i);
		std::string valueName = params[0];
		std::transform(valueName.begin(), valueName.end(), valueName.begin(), ::toupper);

		switch (state) {
		case GETTING_SONG_INFO:
		{
			song_tag.params = &params;
			song_handler_map_t::iterator handler =
				parser_helper.song_tag_handlers.find(valueName);
			if (handler != parser_helper.song_tag_handlers.end())
			{
				handler->second(song_tag);
			}
			// TODO: BGChanges needs more work if desired
			//else if (sValueName.Left(strlen("BGCHANGES")) == "BGCHANGES")
			//{
			//	SetBGChanges(reused_song_info);
			//}

			// This tag will get us to the next section.
			if (valueName == "NOTEDATA")
			{
				state = GETTING_STEP_INFO;
				pNewNotes = out.CreateSteps();
				stepsTiming = TimingData(out.timing_data_.beat0OffsetInSeconds_);
				step_tag.has_own_timing = false;
				step_tag.steps = pNewNotes;
				step_tag.timing = &stepsTiming;
			}
			else
			{
				// Silently ignore unrecognized tags, as was done before. -Kyz
			}
			break;
		}
		case GETTING_STEP_INFO:
		{

			step_tag.params = &params;
			steps_handler_map_t::iterator handler =
				parser_helper.steps_tag_handlers.find(valueName);
			if (handler != parser_helper.steps_tag_handlers.end())
			{
				handler->second(step_tag);
			}
			else if (valueName == "NOTES" || valueName == "NOTES2")
			{
				state = GETTING_SONG_INFO;
				if (step_tag.has_own_timing)
				{
					pNewNotes->timing_data_ = stepsTiming;
				}
				step_tag.has_own_timing = false;
				pNewNotes->SetSMNoteData(params[1]);
				pNewNotes->TidyUpData();
				pNewNotes->SetFilename(sPath);
				out.AddSteps(pNewNotes);
			}
			else if (valueName == "STEPFILENAME")
			{
				state = GETTING_SONG_INFO;
				if (step_tag.has_own_timing)
				{
					pNewNotes->timing_data_ = stepsTiming;
				}
				step_tag.has_own_timing = false;
				pNewNotes->SetFilename(params[1]);
				out.AddSteps(pNewNotes);
			}
			else
			{
				// Silently ignore unrecognized tags, as was done before. -Kyz
			}
			break;
		}
		}
	}

	out.version = kStepFileVersionNumber;
	TidyUpData(out, false);

	// This part is not done in the original ITGm implementation, but placed here for simplicity.
	out.SetGSHashes();

	return false;
}