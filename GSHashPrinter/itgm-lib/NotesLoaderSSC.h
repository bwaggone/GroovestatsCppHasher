// @brief SSCLoader - Reads a Song and its Steps from a .SSC file.
#ifndef NotesLoaderSSC_H
#define NotesLoaderSSC_H

#include "ConstantsAndTypes.h"
#include "NotesLoaderSM.h"

class MsdFile;
class Song;
class Steps;
class TimingData;

enum SSCLoadingStates
{
	GETTING_SONG_INFO,
	GETTING_STEP_INFO,
	NUM_SSCLoadingStates
};

const float VERSION_RADAR_FAKE = 0.53f;
const float VERSION_WARP_SEGMENT = 0.56f;
const float VERSION_SPLIT_TIMING = 0.7f;
const float VERSION_OFFSET_BEFORE_ATTACK = 0.72f;
const float VERSION_CHART_NAME_TAG = 0.74f;
const float VERSION_CACHE_SWITCH_TAG = 0.77f;
const float VERSION_RADAR_NOTECOUNT = 0.83f;

struct SSCLoader : public SMLoader
{
	SSCLoader() : SMLoader(".ssc") {}
	virtual bool LoadFromSimfile(const std::string& sPath, Song& out);
	//virtual bool LoadNoteDataFromSimfile(const std::string& cachePath, Steps& out);

	void ProcessBPMs(TimingData& out, const std::string sParam);
	void ProcessStops(TimingData&, const std::string);
	void ProcessWarps(TimingData&, const std::string, const float);
	void ProcessLabels(TimingData&, const std::string);
	virtual void ProcessCombos(TimingData&, const std::string, const int = -1);
	void ProcessScrolls(TimingData&, const std::string);
};

#endif
