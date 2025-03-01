#include "NotesLoaderSSC.h"
#include "RageUtil.h"
#include "Song.h"

#include <cstddef>
#include <vector>
#include <iostream>
#include <algorithm>

struct StepsTagInfo
{
	SSCLoader* loader;
	Song* song;
	Steps* steps;
	//TimingData* timing;
	const MsdFile::value_t* params;
	const std::string& path;
	StepsTagInfo(SSCLoader* l, Song* s, const std::string& p)
		: loader(l), song(s), path(p)
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
		}
	}

}

void SetDisplayBPM(SongTagInfo& info)
{
	// #DISPLAYBPM:[xxx][xxx:xxx]|[*];
	if ((*info.params)[1] == "*")
	{
		info.song->SetDisplayBPM(enums::DISPLAY_BPM_RANDOM);
	}
	else
	{
		info.song->SetDisplayBPM(enums::DISPLAY_BPM_SPECIFIED);
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
void SetSongBPMs(SongTagInfo& info)
{
	info.loader->ProcessBPMs(info.song->timing_data_, (*info.params)[1]);
}


bool SSCLoader::LoadFromSimfile(const std::string& sPath, Song& out) {
	MsdFile msd;
	if (!msd.ReadFile(sPath, true))
	{
		return false;
	}
	SSCLoadingStates state = GETTING_SONG_INFO;

	// Cribbed from NotesLoaderSSC, this loads in order of the file.
	std::string difficulty = "";
	std::string steps_type = "";
	const unsigned values = msd.GetNumValues();
	for (unsigned i = 0; i < values; i++)
	{
		const MsdFile::value_t& params = msd.GetValue(i);
		std::string valueName = params[0];
		std::string matcher = params[1];
		std::transform(valueName.begin(), valueName.end(), valueName.begin(), ::toupper);
		util::Trim(matcher);

		switch (state) {
		case GETTING_SONG_INFO:
		{
			SongTagInfo song_tag(&*this, &out, sPath);
			StepsTagInfo step_tag(&*this, &out, sPath);
			song_tag.params = &params;
			// TODO: Generic tags function and map
			// WriteGlobalTags(params, out);
			if (valueName == "DISPLAYBPM") {
				SetDisplayBPM(song_tag);
			}

			// This tag will get us to the next section.
			if (valueName == "NOTEDATA")
			{
				state = GETTING_STEP_INFO;
				Steps* new_notes = out.CreateSteps();
				//stepsTiming = TimingData(out.m_SongTiming.m_fBeat0OffsetInSeconds);
				//reused_steps_info.has_own_timing = false;
				//reused_steps_info.steps = pNewNotes;
				//reused_steps_info.timing = &stepsTiming;
			}
			else
			{
				// Silently ignore unrecognized tags, as was done before. -Kyz
			}
			break;
		}
		case GETTING_STEP_INFO:
		{
			break;
		}
		}

		// At this point, ITGm usually calls particular parsing functions
		// for each value. Skip that and only record what we care about for
		// GSHash calculation.
		if (valueName == "DIFFICULTY") {
			// Stateful.
			difficulty = matcher;
		}
		if (valueName == "STEPSTYPE") {
			steps_type = matcher;
		}
		if (valueName == "NOTES") {
			out.AddSteps(matcher, difficulty, steps_type);
		}
		if (valueName == "BPMS") {
			out.SetBpms(matcher);
		}
	}

	// After iteration, calculate the hashes based on what we found.
	out.SetGSHashes();

	return false;
}