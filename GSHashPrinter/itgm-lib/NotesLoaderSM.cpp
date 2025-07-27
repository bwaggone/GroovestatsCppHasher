#include <algorithm>

#include "Attack.h"
#include "BackgroundUtil.h"
#include "NotesLoaderSM.h"
#include "NoteTypes.h"
#include "Song.h"
#include "RageUtil.h"

struct SMSongTagInfo
{
	SMLoader* loader;
	Song* song;
	const MsdFile::value_t* params;
	const std::string& path;
	std::vector<std::pair<float, float>> BPMChanges, Stops;
	SMSongTagInfo(SMLoader* l, Song* s, const std::string& p)
		:loader(l), song(s), path(p)
	{}
};

typedef void (*song_tag_func_t)(SMSongTagInfo& info);

typedef std::map<std::string, song_tag_func_t> song_handler_map_t;


// Functions for song tags go below this line. -Kyz
/****************************************************************/
void SMSetTitle(SMSongTagInfo& info)
{
	info.song->main_title = (*info.params)[1];
	info.loader->SetSongTitle((*info.params)[1]);
}
void SMSetSubtitle(SMSongTagInfo& info)
{
	info.song->subtitle = (*info.params)[1];
}
void SMSetArtist(SMSongTagInfo& info)
{
	info.song->artist = (*info.params)[1];
}
void SMSetTitleTranslit(SMSongTagInfo& info)
{
	info.song->main_title_transliteration = (*info.params)[1];
}
void SMSetSubtitleTranslit(SMSongTagInfo& info)
{
	info.song->subtitle_transliteration = (*info.params)[1];
}
void SMSetArtistTranslit(SMSongTagInfo& info)
{
	info.song->artist_transliteration = (*info.params)[1];
}
void SMSetGenre(SMSongTagInfo& info)
{
	info.song->genre = (*info.params)[1];
}
void SMSetCredit(SMSongTagInfo& info)
{
	info.song->credit = (*info.params)[1];
};
void SMSetBanner(SMSongTagInfo& info)
{
	info.song->banner_file = (*info.params)[1];
}
void SMSetBackground(SMSongTagInfo& info)
{
	info.song->background_file = (*info.params)[1];
}
void SMSetLyricsPath(SMSongTagInfo& info)
{
	info.song->lyrics_file = (*info.params)[1];
}
void SMSetCDTitle(SMSongTagInfo& info)
{
	info.song->cd_title_file = (*info.params)[1];
}
void SMSetMusic(SMSongTagInfo& info)
{
	info.song->music_file = (*info.params)[1];
}
void SMSetOffset(SMSongTagInfo& info)
{
	info.song->timing_data_.beat0OffsetInSeconds_ = std::stof((*info.params)[1]);
}
void SMSetBPMs(SMSongTagInfo& info)
{
	info.BPMChanges.clear();
	info.loader->ParseBPMs(info.BPMChanges, (*info.params)[1]);
}
void SMSetStops(SMSongTagInfo& info)
{
	info.Stops.clear();
	info.loader->ParseStops(info.Stops, (*info.params)[1]);
}
void SMSetDelays(SMSongTagInfo& info)
{
	info.loader->ProcessDelays(info.song->timing_data_, (*info.params)[1]);
}
void SMSetTimeSignatures(SMSongTagInfo& info)
{
	info.loader->ProcessTimeSignatures(info.song->timing_data_, (*info.params)[1]);
}
void SMSetTickCounts(SMSongTagInfo& info)
{
	info.loader->ProcessTickcounts(info.song->timing_data_, (*info.params)[1]);
}
void SMSetInstrumentTrack(SMSongTagInfo& info)
{
	info.loader->ProcessInstrumentTracks(*info.song, (*info.params)[1]);
}
void SMSetSampleStart(SMSongTagInfo& info)
{
	info.song->music_sample_start_seconds = util::HHMMSSToSeconds((*info.params)[1]);
}
void SMSetSampleLength(SMSongTagInfo& info)
{
	info.song->music_sample_length_seconds = util::HHMMSSToSeconds((*info.params)[1]);
}
void SMSetDisplayBPM(SMSongTagInfo& info)
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
			info.song->SetMinBPM(info.song->GetMinBPM());
		}
		else
		{
			info.song->SetMaxBPM(std::stof((*info.params)[2]));
		}
	}
}
void SMSetSelectable(SMSongTagInfo& info)
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
	// ROULETTE from 3.9. It was removed since UnlockManager can serve
	// the same purpose somehow. This, of course, assumes you're using
	// unlocks. -aj
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
	else if (std::stoi((*info.params)[1]) > 0)
	{
		info.song->selection_display = info.song->SHOW_ALWAYS;
	}
	else
	{
		std::cerr << "Song file " << info.path << " has an unknown #SELECTABLE value, \"%s\"; ignored." << (*info.params)[1].c_str() << "\n";
	}
}

struct sm_parser_helper_t
{
	song_handler_map_t song_tag_handlers;
	// Unless signed, the comments in this tag list are not by me.  They were
	// moved here when converting from the else if chain. -Kyz
	sm_parser_helper_t()
	{
		song_tag_handlers["TITLE"] = &SMSetTitle;
		song_tag_handlers["SUBTITLE"] = &SMSetSubtitle;
		song_tag_handlers["ARTIST"] = &SMSetArtist;
		song_tag_handlers["TITLETRANSLIT"] = &SMSetTitleTranslit;
		song_tag_handlers["SUBTITLETRANSLIT"] = &SMSetSubtitleTranslit;
		song_tag_handlers["ARTISTTRANSLIT"] = &SMSetArtistTranslit;
		song_tag_handlers["GENRE"] = &SMSetGenre;
		song_tag_handlers["CREDIT"] = &SMSetCredit;
		song_tag_handlers["BANNER"] = &SMSetBanner;
		song_tag_handlers["BACKGROUND"] = &SMSetBackground;
		// Save "#LYRICS" for later, so we can add an internal lyrics tag.
		song_tag_handlers["LYRICSPATH"] = &SMSetLyricsPath;
		song_tag_handlers["CDTITLE"] = &SMSetCDTitle;
		song_tag_handlers["MUSIC"] = &SMSetMusic;
		song_tag_handlers["OFFSET"] = &SMSetOffset;
		song_tag_handlers["BPMS"] = &SMSetBPMs;
		song_tag_handlers["STOPS"] = &SMSetStops;
		song_tag_handlers["FREEZES"] = &SMSetStops;
		song_tag_handlers["DELAYS"] = &SMSetDelays;
		song_tag_handlers["TIMESIGNATURES"] = &SMSetTimeSignatures;
		song_tag_handlers["TICKCOUNTS"] = &SMSetTickCounts;
		song_tag_handlers["INSTRUMENTTRACK"] = &SMSetInstrumentTrack;
		song_tag_handlers["SAMPLESTART"] = &SMSetSampleStart;
		song_tag_handlers["SAMPLELENGTH"] = &SMSetSampleLength;
		song_tag_handlers["DISPLAYBPM"] = &SMSetDisplayBPM;
		song_tag_handlers["SELECTABLE"] = &SMSetSelectable;
		// It's a bit odd to have the tag that exists for backwards compatibility
		// in this list and not the replacement, but the BGCHANGES tag has a
		// number on the end, allowing up to NUM_BackgroundLayer tags, so it
		// can't fit in the map. -Kyz
		/*song_tag_handlers["ANIMATIONS"] = &SMSetBGChanges;
		song_tag_handlers["FGCHANGES"] = &SMSetFGChanges;
		song_tag_handlers["KEYSOUNDS"] = &SMSetKeysounds;
		// Attacks loaded from file
		song_tag_handlers["ATTACKS"] = &SMSetAttacks;
		/* Tags that no longer exist, listed for posterity.  May their names
		 * never be forgotten for their service to Stepmania. -Kyz
		 * LASTBEATHINT: // unable to identify at this point: ignore
		 * MUSICBYTES: // ignore
		 * FIRSTBEAT: // cache tags from older SM files: ignore.
		 * LASTBEAT: // cache tags from older SM files: ignore.
		 * SONGFILENAME: // cache tags from older SM files: ignore.
		 * HASMUSIC: // cache tags from older SM files: ignore.
		 * HASBANNER: // cache tags from older SM files: ignore.
		 * SAMPLEPATH: // SamplePath was used when the song has a separate preview clip. -aj
		 * LEADTRACK: // XXX: Does anyone know what LEADTRACK is for? -Wolfman2000
		 * MUSICLENGTH: // Loaded from the cache now. -Kyz
		 */
	}
};
sm_parser_helper_t sm_parser_helper;
// End sm_parser_helper related functions. -Kyz
/****************************************************************/

void SMLoader::SetSongTitle(const std::string& title)
{
	this->songTitle = title;
}

std::string SMLoader::GetSongTitle() const
{
	return this->songTitle;
}

void SMLoader::ProcessInstrumentTracks(Song& out, const std::string &sParam)
{
	std::vector<std::string> vs1;
	util::split(sParam, ",", vs1);
	for (auto const& s : vs1)
	{
		std::vector<std::string> vs2;
		util::split(s, "=", vs2);
		if (vs2.size() >= 2)
		{
			InstrumentTrack it = StringToInstrumentTrack(vs2[0]);
			if (it != InstrumentTrack_Invalid)
				out.instrument_track_files[it] = vs2[1];
		}
	}
}

void SMLoader::ProcessTimeSignatures(TimingData& out, const std::string line, const int rowsPerBeat)
{
	std::vector<std::string> vs1;
	std::vector<TimeSignatureSegment> segments;
	util::split(line, ",", vs1);

	for (std::string const& s1 : vs1)
	{
		std::vector<std::string> vs2;
		util::split(s1, "=", vs2);

		if (vs2.size() < 3)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has an invalid time signature change with " << vs2.size() << "values\n";
			continue;
		}

		const float fBeat = RowToBeat(vs2[0], rowsPerBeat);
		const int iNumerator = std::stoi(vs2[1]);
		const int iDenominator = std::stoi(vs2[2]);

		if (fBeat < 0)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has an invalid time signature change with beat " << fBeat << "\n";
			continue;
		}

		if (iNumerator < 1)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has an invalid time signature change with beat " << fBeat << " numerator " << iNumerator << "\n";
			continue;
		}

		if (iDenominator < 1)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has an invalid time signature change with beat " << fBeat << " denominator " << iDenominator << "\n";
			continue;
		}
		segments.push_back(TimeSignatureSegment(BeatToNoteRow(fBeat), iNumerator, iDenominator));
	}

	// If there are any time signatures defined, but there isn't one
	// for the very first beat of the song, then add one.
	// Without it, calls to functions like TimingData::NoteRowToMeasureAndBeat
	// can fail for charts that are otherwise valid.
	if (segments.size() > 0 && segments[0].GetRow() > 0)
	{
		out.AddSegment(TimeSignatureSegment(0, 4, 4));
	}

	for (TimeSignatureSegment segment : segments)
	{
		out.AddSegment(segment);
	}
}


void SMLoader::ProcessTickcounts(TimingData& out, const std::string line, const int rowsPerBeat)
{
	std::vector<std::string> arrayTickcountExpressions;
	util::split(line, ",", arrayTickcountExpressions);

	for (unsigned f = 0; f < arrayTickcountExpressions.size(); f++)
	{
		std::vector<std::string> arrayTickcountValues;
		util::split(arrayTickcountExpressions[f], "=", arrayTickcountValues);
		if (arrayTickcountValues.size() != 2)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has an invalid #TICKCOUNTS value " << arrayTickcountValues[f] << "\n";
			continue;
		}

		const float fTickcountBeat = RowToBeat(arrayTickcountValues[0], rowsPerBeat);
		int iTicks = std::clamp(atoi(arrayTickcountValues[1].c_str()), 0, ROWS_PER_BEAT);

		out.AddSegment(TickcountSegment(BeatToNoteRow(fTickcountBeat), iTicks));
	}
}

void SMLoader::ProcessSpeeds(TimingData& out, const std::string line, const int rowsPerBeat)
{
	std::vector<std::string> vs1;
	util::split(line, ",", vs1);

	for (std::string const& s1 : vs1)
	{
		std::vector<std::string> vs2;
		util::split(s1, "=", vs2);

		if (vs2[0].c_str() == 0 && vs2.size() == 2) // First one always seems to have 2.
		{
			vs2.push_back("0");
		}

		if (vs2.size() == 3) // use beats by default.
		{
			vs2.push_back("0");
		}

		if (vs2.size() < 4)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has a speed change with " << vs2.size() << " values\n";
			continue;
		}

		const float fBeat = RowToBeat(vs2[0], rowsPerBeat);
		const float fRatio = std::stof(vs2[1]);
		const float fDelay = std::stof(vs2[2]);

		// XXX: ugly...
		int iUnit = std::stoi(vs2[3]);
		SpeedSegment::BaseUnit unit = (iUnit == 0) ?
			SpeedSegment::UNIT_BEATS : SpeedSegment::UNIT_SECONDS;

		if (fBeat < 0)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has a speed change with beat " << fBeat << "\n";
			continue;
		}

		if (fDelay < 0)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has a speed change with beat " << fBeat << " length " << fDelay << "\n";
			continue;
		}

		out.AddSegment(SpeedSegment(BeatToNoteRow(fBeat), fRatio, fDelay, unit));
	}
}

void SMLoader::ProcessFakes(TimingData& out, const std::string line, const int rowsPerBeat)
{
	std::vector<std::string> arrayFakeExpressions;
	util::split(line, ",", arrayFakeExpressions);

	for (unsigned b = 0; b < arrayFakeExpressions.size(); b++)
	{
		std::vector<std::string> arrayFakeValues;
		util::split(arrayFakeExpressions[b], "=", arrayFakeValues);
		if (arrayFakeValues.size() != 2)
		{
			std::cout << "Song file " << this->GetSongTitle() << " has an invalid #FAKES value " << arrayFakeExpressions[b] << "\n";
			continue;
		}

		const float fBeat = RowToBeat(arrayFakeValues[0], rowsPerBeat);
		const float fSkippedBeats = std::stof(arrayFakeValues[1]);

		if (fSkippedBeats > 0)
			out.AddSegment(FakeSegment(BeatToNoteRow(fBeat), fSkippedBeats));
		else
		{
			std::cout << "Song file " << this->GetSongTitle() << " has an invalid Fake beat " << fBeat << " beats to skip " << fSkippedBeats << "\n";
		}
	}
}


void SMLoader::ParseBPMs(std::vector<std::pair<float, float>>& out, const std::string line, const int rowsPerBeat)
{
	std::vector<std::string> arrayBPMChangeExpressions;
	util::split(line, ",", arrayBPMChangeExpressions);

	for (unsigned b = 0; b < arrayBPMChangeExpressions.size(); b++)
	{
		std::vector<std::string> arrayBPMChangeValues;
		util::Trim(arrayBPMChangeExpressions[b]);
		if (arrayBPMChangeExpressions[b].empty()) {
			continue;
		}
		util::split(arrayBPMChangeExpressions[b], "=", arrayBPMChangeValues);
		if (arrayBPMChangeValues.size() != 2)
		{
			std::cerr << "Song file " <<
				this->GetSongTitle() <<
				"has an invalid #BPMs value \"%s\" (must have exactly one '='), ignored. " <<
				arrayBPMChangeExpressions[b].c_str();
			continue;
		}

		const float fBeat = RowToBeat(arrayBPMChangeValues[0], rowsPerBeat);
		const float fNewBPM = std::stof(arrayBPMChangeValues[1]);
		if (fNewBPM == 0) {
			std::cerr << "Song file " << this->GetSongTitle() <<
				" has a zero BPM; ignored.\n";
			continue;
		}

		out.push_back(std::make_pair(fBeat, fNewBPM));
	}
}

void SMLoader::ParseStops(std::vector<std::pair<float, float>>& out, const std::string line, const int rowsPerBeat)
{
	std::vector<std::string> arrayFreezeExpressions;
	util::split(line, ",", arrayFreezeExpressions);

	for (unsigned f = 0; f < arrayFreezeExpressions.size(); f++)
	{
		std::vector<std::string> arrayFreezeValues;
		util::Trim(arrayFreezeExpressions[f]);
		if (arrayFreezeExpressions[f].empty()) {
			continue;
		}
		util::split(arrayFreezeExpressions[f], "=", arrayFreezeValues);
		if (arrayFreezeValues.size() != 2)
		{
			std::cerr << "Song file " <<
				this->GetSongTitle() <<
				"has an invalid #STOPS value \"%s\" (must have exactly one '='), ignored. "<<
				arrayFreezeExpressions[f].c_str() << "\n";
			continue;
		}

		const float fFreezeBeat = RowToBeat(arrayFreezeValues[0], rowsPerBeat);
		const float fFreezeSeconds = std::stof(arrayFreezeValues[1]);
		if (fFreezeSeconds == 0) {
			std::cerr << "Song file " << this->GetSongTitle() <<
				" has a zero-length stop; ignored.\n";
			continue;
		}

		out.push_back(std::make_pair(fFreezeBeat, fFreezeSeconds));
	}
}


void SMLoader::LoadFromTokens(
	std::string sStepsType,
	std::string sDescription,
	std::string sDifficulty,
	std::string sMeter,
	std::string sRadarValues,
	std::string sNoteData,
	Steps& out
)
{

	util::Trim(sStepsType);
	util::Trim(sDescription);
	util::Trim(sDifficulty);
	util::Trim(sNoteData);

	out.SetStepsTypeStr(sStepsType);
	out.SetStepsType(sStepsType);
	out.SetDescription(sDescription);
	out.SetCredit(sDescription); // this is often used for both.
	out.SetChartName(sDescription); // yeah, one more for good measure.
	out.SetDifficulty(OldStyleStringToDifficulty(sDifficulty));

	std::string desc_lower = sDescription;
	std::transform(desc_lower.begin(), desc_lower.end(), desc_lower.begin(), ::tolower);

	// Handle hacks that originated back when StepMania didn't have
	// Difficulty_Challenge. (At least v1.64, possibly v3.0 final...)
	if (out.GetDifficulty() == Difficulty_Hard)
	{
		// HACK: SMANIAC used to be Difficulty_Hard with a special description.
		if (desc_lower == "smaniac")
			out.SetDifficulty(Difficulty_Challenge);

		// HACK: CHALLENGE used to be Difficulty_Hard with a special description.
		if (desc_lower == "challenge")
			out.SetDifficulty(Difficulty_Challenge);
	}

	if (sMeter.empty())
	{
		// some simfiles (e.g. X-SPECIALs from Zenius-I-Vanisher) don't
		// have a meter on certain steps. Make the meter 1 in these instances.
		sMeter = "1";
	}
	out.SetMeter(std::stoi(sMeter));

	out.SetSMNoteData(sNoteData);

	out.TidyUpData();
}

void SMLoader::ProcessBGChanges(Song& out, const std::string& sValueName, const std::string& sPath, const std::string& sParam)
{
	/*BackgroundLayer iLayer = BACKGROUND_LAYER_1;
	if (sscanf(sValueName.c_str(), "BGCHANGES%d", &*ConvertValue<int>(&iLayer)) == 1)
		enum_add(iLayer, -1);	// #BGCHANGES2 = BACKGROUND_LAYER_2

	bool bValid = iLayer >= 0 && iLayer < NUM_BackgroundLayer;
	if (!bValid)
	{
		// BGChanges out of range.
	}
	else
	{
		std::vector<std::vector<std::string> > aBGChanges;
		ParseBGChangesString(sParam, aBGChanges, out.GetPath());

		for (const auto& b : aBGChanges)
		{
			BackgroundChange change;
			if (LoadFromBGChangesVector(change, b))
				out.AddBackgroundChange(iLayer, change);
		}
	}*/
}

bool SMLoader::LoadFromBGChangesVector(BackgroundChange& change, std::vector<std::string> aBGChangeValues)
{
	/*aBGChangeValues.resize(std::min((int)aBGChangeValues.size(), 11));

	switch (aBGChangeValues.size())
	{
	case 11:
		change.m_def.m_sColor2 = aBGChangeValues[10];
		change.m_def.m_sColor2.Replace('^', ',');
		change.m_def.m_sColor2 = RageColor::NormalizeColostd::string(change.m_def.m_sColor2);
		[[fallthrough]];
	case 10:
		change.m_def.m_sColor1 = aBGChangeValues[9];
		change.m_def.m_sColor1.Replace('^', ',');
		change.m_def.m_sColor1 = RageColor::NormalizeColostd::string(change.m_def.m_sColor1);
		[[fallthrough]];
	case 9:
		change.m_sTransition = aBGChangeValues[8];
		[[fallthrough]];
	case 8:
	{
		std::string tmp = aBGChangeValues[7];
		tmp.MakeLower();
		if ((tmp.find(".ini") != std::string::npos || tmp.find(".xml") != std::string::npos)
			&& !PREFSMAN->m_bQuirksMode)
		{
			return false;
		}
		change.m_def.m_sFile2 = aBGChangeValues[7];
		[[fallthrough]];
	}
	case 7:
		change.m_def.m_sEffect = aBGChangeValues[6];
		[[fallthrough]];
	case 6:
		// param 7 overrides this.
		// Backward compatibility:
		if (change.m_def.m_sEffect.empty())
		{
			bool bLoop = std::stoi(aBGChangeValues[5]) != 0;
			if (!bLoop)
				change.m_def.m_sEffect = SBE_StretchNoLoop;
		}
		[[fallthrough]];
	case 5:
		// param 7 overrides this.
		// Backward compatibility:
		if (change.m_def.m_sEffect.empty())
		{
			bool bRewindMovie = std::stoi(aBGChangeValues[4]) != 0;
			if (bRewindMovie)
				change.m_def.m_sEffect = SBE_StretchRewind;
		}
		[[fallthrough]];
	case 4:
		// param 9 overrides this.
		// Backward compatibility:
		if (change.m_sTransition.empty())
			change.m_sTransition = (std::stoi(aBGChangeValues[3]) != 0) ? "CrossFade" : "";
		[[fallthrough]];
	case 3:
		change.m_fRate = std::stof(aBGChangeValues[2]);
		[[fallthrough]];
	case 2:
	{
		std::string tmp = aBGChangeValues[1];
		tmp.MakeLower();
		if ((tmp.find(".ini") != std::string::npos || tmp.find(".xml") != std::string::npos)
			&& !PREFSMAN->m_bQuirksMode)
		{
			return false;
		}
		change.m_def.m_sFile1 = aBGChangeValues[1];
		[[fallthrough]];
	}
	case 1:
		change.m_fStartBeat = std::stof(aBGChangeValues[0]);
	}

	return aBGChangeValues.size() >= 2;*/
	return 0;
}


std::vector<std::string> SMLoader::GetSongDirFiles(const std::string& sSongDir)
{
	if (!m_SongDirFiles.empty())
		return m_SongDirFiles;

	std::vector<std::string> vsDirs;
	vsDirs.push_back(sSongDir);

	while (!vsDirs.empty())
	{
		std::string d = vsDirs.back();
		vsDirs.pop_back();

		std::vector<std::string> vsFiles;
		//TODO bwaggone: This and below
		//GetDirListing(d + "*", vsFiles, false, true);

		for (const std::string& f : vsFiles)
		{
			//if (IsADirectory(f))
			//	vsDirs.push_back(f + "/");

			m_SongDirFiles.push_back(f.substr(sSongDir.size()));
		}
	}

	return m_SongDirFiles;
}

void SMLoader::ParseBGChangesString(const std::string& _sChanges, std::vector<std::vector<std::string> >& vvsAddTo, const std::string& sSongDir)
{
	// short circuit: empty string
	if (_sChanges.empty())
		return;

	// strip newlines (basically operates as both split and join at the same time)
	std::string sChanges;
	size_t start = 0;
	do {
		size_t pos = _sChanges.find_first_of("\r\n", start);
		if (std::string::npos == pos)
			pos = _sChanges.size();

		if (pos - start > 0) {
			if ((start == 0) && (pos == _sChanges.size()))
				sChanges = _sChanges;
			else
				sChanges += _sChanges.substr(start, pos - start);
		}
		start = pos + 1;
	} while (start <= _sChanges.size());

	// after removing newlines, do we have anything?
	if (sChanges.empty())
		return;

	// get the list of possible files/directories for the file parameters
	std::vector<std::string> vsFiles = GetSongDirFiles(sSongDir);

	start = 0;
	int pnum = 0;
	do {
		switch (pnum) {
			// parameters 1 and 7 can be files or folder names
		case 1:
		case 7:
		{
			// see if one of the files in the song directory are listed.
			std::string found;
			for (const auto& f : vsFiles)
			{
				// there aren't enough characters for this to match
				if ((sChanges.size() - start) < f.size())
					continue;

				// the string itself matches
				if (util::upper(f) == util::upper(sChanges.substr(start, f.size())))
				{
					size_t nextpos = start + f.size();

					// is this name followed by end-of-string, equals, or comma?
					if ((nextpos == sChanges.size()) || (sChanges[nextpos] == '=') || (sChanges[nextpos] == ','))
					{
						found = f;
						break;
					}
				}
			}
			// yes. use that as this parameter, even if it has commas or equals signs in it
			if (!found.empty())
			{
				vvsAddTo.back().push_back(found);
				start += found.size();
				// the next character should be a comma or equals. skip it
				if (start < sChanges.size())
				{
					if (sChanges[start] == '=')
						++pnum;
					else
					{
						//ASSERT(sChanges[start] == ',');
						pnum = 0;
					}
					start += 1;
				}
				// move to the next parameter
				break;
			}
			// deliberate fall-through if not found. treat it as a normal string like before
			[[fallthrough]];
		}
		// everything else should be safe
		default:
			if (0 == pnum) vvsAddTo.push_back(std::vector<std::string>()); // first value of this set. create our vector

			{
				size_t eqpos = sChanges.find('=', start);
				size_t compos = sChanges.find(',', start);

				if ((eqpos == std::string::npos) && (compos == std::string::npos))
				{
					// neither = nor , were found in the remainder of the string. consume the rest of the string.
					vvsAddTo.back().push_back(sChanges.substr(start));
					start = sChanges.size() + 1;
				}
				else if ((eqpos != std::string::npos) && (compos != std::string::npos))
				{
					// both were found. which came first?
					if (eqpos < compos)
					{
						// equals. consume value and move to next value
						vvsAddTo.back().push_back(sChanges.substr(start, eqpos - start));
						start = eqpos + 1;
						++pnum;
					}
					else
					{
						// comma. consume value and move to next set
						vvsAddTo.back().push_back(sChanges.substr(start, compos - start));
						start = compos + 1;
						pnum = 0;
					}
				}
				else if (eqpos != std::string::npos)
				{
					// found only equals. consume and move on.
					vvsAddTo.back().push_back(sChanges.substr(start, eqpos - start));
					start = eqpos + 1;
					++pnum;
				}
				else
				{
					// only foudn comma. consume and move on.
					vvsAddTo.back().push_back(sChanges.substr(start, compos - start));
					start = compos + 1;
					pnum = 0;
				}
				break;
			}
		}
	} while (start <= sChanges.size());
}

void SMLoader::ProcessAttackString(std::vector<std::string>& attacks, MsdFile::value_t params)
{
	for (unsigned s = 1; s < params.params.size(); ++s)
	{
		std::string tmp = params[s];
		util::Trim(tmp);
		if (tmp.size() > 0)
			attacks.push_back(tmp);
	}
}

void SMLoader::ProcessAttacks(AttackArray& attacks, MsdFile::value_t params)
{
	Attack attack;
	float end = -9999;

	for (unsigned j = 1; j < params.params.size(); ++j)
	{
		std::vector<std::string> sBits;
		util::split(params[j], "=", sBits, false);

		// Need an identifer and a value for this to work
		if (sBits.size() < 2)
			continue;

		util::Trim(sBits[0]);

		// TODO bwaggone this
		/*if (!sBits[0].CompareNoCase("TIME"))
			attack.fStartSecond = strtof(sBits[1], nullptr);
		else if (!sBits[0].CompareNoCase("LEN"))
			attack.fSecsRemaining = strtof(sBits[1], nullptr);
		else if (!sBits[0].CompareNoCase("END"))
			end = strtof(sBits[1], nullptr);
		else if (!sBits[0].CompareNoCase("MODS"))
		{
			util::Trim(sBits[1]);
			attack.sModifiers = sBits[1];

			if (end != -9999)
			{
				attack.fSecsRemaining = end - attack.fStartSecond;
				end = -9999;
			}

			if (attack.fSecsRemaining < 0.0f)
				attack.fSecsRemaining = 0.0f;

			attacks.push_back(attack);
		}*/
	}
}

float SMLoader::RowToBeat(std::string line, int rowsPerBeat)
{
	std::string backup = line;
	util::Trim(line, "r");
	util::Trim(line, "R");
	if (backup != line)
	{
		return std::stof(line) / rowsPerBeat;
	}
	else
	{
		return std::stof(line);
	}
}

void SMLoader::ProcessDelays(TimingData& out, std::string line, int rowsPerBeat)
{
	std::vector<std::string> arrayDelayExpressions;
	util::split(line, ",", arrayDelayExpressions);

	for (unsigned f = 0; f < arrayDelayExpressions.size(); f++)
	{
		std::vector<std::string> arrayDelayValues;
		util::split(arrayDelayExpressions[f], "=", arrayDelayValues);
		if (arrayDelayValues.size() != 2)
		{
			// Invalid delays
			continue;
		}
		float fFreezeBeat = RowToBeat(arrayDelayValues[0], rowsPerBeat);
		float fFreezeSeconds = std::stof(arrayDelayValues[1]);
		// LOG->Trace( "Adding a delay segment: beat: %f, seconds = %f", new_seg.m_fStartBeat, new_seg.m_fStopSeconds );

		if (fFreezeSeconds > 0.0f)
			out.AddSegment(DelaySegment(BeatToNoteRow(fFreezeBeat), fFreezeSeconds));
	}
}

void SMLoader::TidyUpData(Song& song, bool bFromCache)
{
	// TODO(bwaggone): Most of this deals with BGChanges, which are ignored
	// in this script for now. If we get it working, then implement this.
	/*
	* Hack: if the song has any changes at all (so it won't use a random BGA)
	* and doesn't end with "-nosongbg-", add a song background BGC.  Remove
	* "-nosongbg-" if it exists.
	*
	* This way, songs that were created earlier, when we added the song BG
	* at the end by default, will still behave as expected; all new songs will
	* have to add an explicit song BG tag if they want it.  This is really a
	* formatting hack only; nothing outside of SMLoader ever sees "-nosongbg-".
	*/
	/*std::vector<BackgroundChange>& bg = song.GetBackgroundChanges(BACKGROUND_LAYER_1);
	if (!bg.empty())
	{
		bool bHasNoSongBgTag = false;

		for (unsigned i = 0; !bHasNoSongBgTag && i < bg.size(); ++i)
		{
			if (!bg[i].m_def.m_sFile1.CompareNoCase(NO_SONG_BG_FILE))
			{
				bg.erase(bg.begin() + i);
				bHasNoSongBgTag = true;
			}
		}

		// If there's no -nosongbg- tag, add the song BG.
		if (!bHasNoSongBgTag) do
		{
			if (bFromCache)
				break;

			float lastBeat = song.GetLastBeat();
			if (!bg.empty() && bg.back().m_fStartBeat - 0.0001f >= lastBeat)
				break;

			// If the last BGA is already the song BGA, don't add a duplicate.
			if (!bg.empty() && !bg.back().m_def.m_sFile1.CompareNoCase(song.m_sBackgroundFile))
				break;

			if (!IsAFile(song.GetBackgroundPath()))
				break;

			bg.push_back(BackgroundChange(lastBeat, song.m_sBackgroundFile));
		} while (0);
	}
	if (bFromCache)
	{
		song.TidyUpData(bFromCache, true);
	}*/
}

// Utility function for sorting timing change data
namespace {
	bool compare_first(std::pair<float, float> a, std::pair<float, float> b) {
		return a.first < b.first;
	}
}

// Precondition: no BPM change or stop has 0 for its value (change.second).
//     (The ParseBPMs and ParseStops functions make sure of this.)
// Postcondition: all BPM changes, stops, and warps are added to the out
//     parameter, already sorted by beat.
void SMLoader::ProcessBPMsAndStops(TimingData& out,
	std::vector<std::pair<float, float>>& vBPMs,
	std::vector<std::pair<float, float>>& vStops)
{
	std::vector<std::pair<float, float>>::const_iterator ibpm, ibpmend;
	std::vector<std::pair<float, float>>::const_iterator istop, istopend;

	// Current BPM (positive or negative)
	float bpm = 0;
	// Beat at which the previous timing change occurred
	float prevbeat = 0;
	// Start/end of current warp (-1 if not currently warping)
	float warpstart = -1;
	float warpend = -1;
	// BPM prior to current warp, to detect if it has changed
	float prewarpbpm = 0;
	// How far off we have gotten due to negative changes
	float timeofs = 0;

	// Sort BPM changes and stops by beat.  Order matters.
	// TODO: Make sorted lists a precondition rather than sorting them here.
	// The caller may know that the lists are sorted already (e.g. if
	// loaded from cache).
	stable_sort(vBPMs.begin(), vBPMs.end(), compare_first);
	stable_sort(vStops.begin(), vStops.end(), compare_first);

	// Convert stops that come before beat 0.  All these really do is affect
	// where the arrows are with respect to the music, i.e. the song offset.
	// Positive stops subtract from the offset, and negative add to it.
	istop = vStops.begin();
	istopend = vStops.end();
	for (/* istop */; istop != istopend && istop->first < 0; istop++)
	{
		out.beat0OffsetInSeconds_ -= istop->second;
	}

	// Get rid of BPM changes that come before beat 0.  Positive BPMs before
	// the chart don't really do anything, so we just ignore them.  Negative
	// BPMs cause unpredictable behavior, so ignore them as well and issue a
	// warning.
	ibpm = vBPMs.begin();
	ibpmend = vBPMs.end();
	for (/* ibpm */; ibpm != ibpmend && ibpm->first <= 0; ibpm++)
	{
		bpm = ibpm->second;
		if (bpm < 0 && ibpm->first < 0)
		{
			std::cerr << "Song file" << this->GetSongTitle() <<
				"has a negative BPM prior to beat 0.  " <<
				"These cause problems; ignoring.\n";
		}
	}

	// It's beat 0.  Do you know where your BPMs are?
	if (bpm == 0)
	{
		// Nope.  Can we just use the next BPM value?
		if (ibpm == ibpmend)
		{
			// Nope.
			bpm = 60;
			std::cerr << "Song file " << this->GetSongTitle() <<
				"has no valid BPMs.  Defaulting to 60.\n";
		}
		else
		{
			// Yep.  Get the next BPM.
			ibpm++;
			bpm = ibpm->second;
			std::cerr << "Song file" << this->GetSongTitle() <<
				"does not establish a BPM before beat 0.  "
				"Using the value from the next BPM change.\n";
		}
	}
	// We always want to have an initial BPM.  If we start out warping, this
	// BPM will be added later.  If we start with a regular BPM, add it now.
	if (bpm > 0 && bpm <= FAST_BPM_WARP)
	{
		out.AddSegment(BPMSegment(BeatToNoteRow(0), bpm));
	}

	// Iterate over all BPMs and stops in tandem
	while (ibpm != ibpmend || istop != istopend)
	{
		// Get the next change in order, with BPMs taking precedence
		// when they fall on the same beat.
		bool changeIsBpm = istop == istopend || (ibpm != ibpmend && ibpm->first <= istop->first);
		const std::pair<float, float>& change = changeIsBpm ? *ibpm : *istop;

		// Calculate the effects of time at the current BPM.  "Infinite"
		// BPMs (SM4 warps) imply that zero time passes, so skip this
		// step in that case.
		if (bpm <= FAST_BPM_WARP)
		{
			timeofs += (change.first - prevbeat) * 60 / bpm;

			// If we were in a warp and it finished during this
			// timeframe, create the warp segment.
			if (warpstart >= 0 && bpm > 0 && timeofs > 0)
			{
				// timeofs represents how far past the end we are
				warpend = change.first - (timeofs * bpm / 60);
				out.AddSegment(WarpSegment(BeatToNoteRow(warpstart),
					warpend - warpstart));

				// If the BPM changed during the warp, put that
				// change at the beginning of the warp.
				if (bpm != prewarpbpm)
				{
					out.AddSegment(BPMSegment(BeatToNoteRow(warpstart), bpm));
				}
				// No longer warping
				warpstart = -1;
			}
		}

		// Save the current beat for the next round of calculations
		prevbeat = change.first;

		// Now handle the timing changes themselves
		if (changeIsBpm)
		{
			// Does this BPM change start a new warp?
			if (warpstart < 0 && (change.second < 0 || change.second > FAST_BPM_WARP))
			{
				// Yes.
				warpstart = change.first;
				prewarpbpm = bpm;
				timeofs = 0;
			}
			else if (warpstart < 0)
			{
				// No, and we aren't currently warping either.
				// Just a normal BPM change.
				out.AddSegment(BPMSegment(BeatToNoteRow(change.first), change.second));
			}
			bpm = change.second;
			ibpm++;
		}
		else
		{
			// Does this stop start a new warp?
			if (warpstart < 0 && change.second < 0)
			{
				// Yes.
				warpstart = change.first;
				prewarpbpm = bpm;
				timeofs = change.second;
			}
			else if (warpstart < 0)
			{
				// No, and we aren't currently warping either.
				// Just a normal stop.
				out.AddSegment(StopSegment(BeatToNoteRow(change.first), change.second));
			}
			else
			{
				// We're warping already.  Stops affect the time
				// offset directly.
				timeofs += change.second;

				// If a stop overcompensates for the time
				// deficit, the warp ends and we stop for the
				// amount it goes over.
				if (change.second > 0 && timeofs > 0)
				{
					warpend = change.first;
					out.AddSegment(WarpSegment(BeatToNoteRow(warpstart),
						warpend - warpstart));
					out.AddSegment(StopSegment(BeatToNoteRow(change.first), timeofs));

					// Now, are we still warping because of
					// the BPM value?
					if (bpm < 0 || bpm > FAST_BPM_WARP)
					{
						// Yep.
						warpstart = change.first;
						// prewarpbpm remains the same
						timeofs = 0;
					}
					else
					{
						// Nope, warp is done.  Add any
						// BPM change that happened in
						// the meantime.
						if (bpm != prewarpbpm)
						{
							out.AddSegment(BPMSegment(BeatToNoteRow(warpstart), bpm));
						}
						warpstart = -1;
					}
				}
			}
			istop++;
		}
	}

	// If we are still warping, we now have to consider the time remaining
	// after the last timing change.
	if (warpstart >= 0)
	{
		// Will this warp ever end?
		if (bpm < 0 || bpm > FAST_BPM_WARP)
		{
			// No, so it ends the entire chart immediately.
			// XXX There must be a less hacky and more accurate way
			// to do this.
			warpend = 99999999.0f;
		}
		else
		{
			// Yes.  Figure out when it will end.
			warpend = prevbeat - (timeofs * bpm / 60);
		}
		out.AddSegment(WarpSegment(BeatToNoteRow(warpstart),
			warpend - warpstart));

		// As usual, record any BPM change that happened during the warp
		if (bpm != prewarpbpm)
		{
			out.AddSegment(BPMSegment(BeatToNoteRow(warpstart), bpm));
		}
	}
}

bool SMLoader::LoadFromSimfile(const std::string& sPath, Song& out, bool bFromCache)
{

	MsdFile msd;
	if (!msd.ReadFile(sPath, true))  // unescape
	{
		std::cerr << "Song file" << sPath << "couldn't be opened: " << msd.GetError().c_str();
		return false;
	}

	SMSongTagInfo reused_song_info(&*this, &out, sPath);

	for (unsigned i = 0; i < msd.GetNumValues(); i++)
	{
		int iNumParams = msd.GetNumParams(i);
		const MsdFile::value_t& sParams = msd.GetValue(i);
		std::string sValueName = sParams[0];
		util::upper(sValueName);

		reused_song_info.params = &sParams;
		song_handler_map_t::iterator handler =
			sm_parser_helper.song_tag_handlers.find(sValueName);
		if (handler != sm_parser_helper.song_tag_handlers.end())
		{
			/* Don't use GetMainAndSubTitlesFromFullTitle; that's only for heuristically
			 * splitting other formats that *don't* natively support #SUBTITLE. */
			handler->second(reused_song_info);
		}
		// TODO: BGChanges needs more work if desired
		//else if (Left(sValueName, strlen("BGCHANGES")) == "BGCHANGES")
		//{
		//	SMSetBGChanges(reused_song_info);
		//}
		else if (sValueName == "NOTES" || sValueName == "NOTES2")
		{
			if (iNumParams < 7)
			{
				std::cerr << "Song file" << sPath << "has " << iNumParams << " fields in a #NOTES tag, but should have at least 7.\n";
				continue;
			}

			Steps* pNewNotes = out.CreateSteps();
			LoadFromTokens(
				sParams[1],
				sParams[2],
				sParams[3],
				sParams[4],
				sParams[5],
				sParams[6],
				*pNewNotes);

			pNewNotes->SetFilename(sPath);
			out.AddSteps(pNewNotes);
		}
		else
		{
			std::cerr << "Song file " << sPath << " has an unexpected value named " << sValueName.c_str() << "\n";
		}
	}

	// Turn negative time changes into warps
	ProcessBPMsAndStops(out.timing_data_, reused_song_info.BPMChanges, reused_song_info.Stops);

	TidyUpData(out, bFromCache);
	// This part is not done in the original ITGm implementation, but placed here for simplicity.
	out.SetGSHashes();
	return true;
}