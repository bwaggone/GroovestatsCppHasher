#include "NotesLoader.h"
#include "NotesLoaderSSC.h"
#include "MsdFile.h"
#include "RageUtil.h"

#include <cstddef>
#include <vector>
#include <iostream>
#include <algorithm>

void NotesLoader::GetMainAndSubTitlesFromFullTitle( const std::string &sFullTitle, std::string &sMainTitleOut, std::string &sSubTitleOut )
{
	static const std::string_view sLeftSeps[] = { "\t", " -", " ~", " (", " [" };
	size_t fullTitleSize = sFullTitle.size();

	for (const auto& sep : sLeftSeps)
	{
		size_t iBeginIndex = sFullTitle.find(sep);
		if (iBeginIndex != std::string::npos)
		{
			sMainTitleOut = sFullTitle.substr(0, static_cast<int>(iBeginIndex));
			sSubTitleOut = sFullTitle.substr(iBeginIndex + sep.size(), fullTitleSize - iBeginIndex - sep.size());
			return;
		}
	}
	sMainTitleOut = sFullTitle;
	sSubTitleOut = "";
}

// bwaggone note: The original NotesLoaderSM has some hacks to preserve legacy
// loading behavior (smaniac -> challenge, descriptions on hard to call it challenge, etc).
// Most of these hacks are omitted.
bool NotesLoader::LoadFromSM(MsdFile& in, Song& out) {
	const unsigned values = in.GetNumValues();

	// Cribbed from NotesLoaderSM, this loads in order of the file.
	std::string difficulty = "";
	for (unsigned i = 0; i < values; i++)
	{
		const MsdFile::value_t& params = in.GetValue(i);
		std::string valueName = params[0];
		std::string matcher = params[1];
		std::transform(valueName.begin(), valueName.end(), valueName.begin(), ::toupper);
		util::Trim(matcher);

		// At this point, ITGm usually calls particular parsing functions
		// for each value. Skip that and only record what we care about for
		// GSHash calculation.
		if (valueName == "NOTES" || valueName == "NOTES2") {
			// SM Files contain a bunch of extra information in the notes, this has
			// to be parsed more carefully.
			int num_params = in.GetNumParams(i);
			if (num_params < 7) {
				std::cout << "Error, there should be at least 7 fields in a #NOTES tag\n";
				return false;
			}
			std::string steps_type = params[1];
			std::string difficulty = params[3];
			std::string note_data = params[6];
			util::Trim(steps_type);
			util::Trim(difficulty);
			util::Trim(note_data);
			//out.AddSteps(note_data, difficulty, steps_type);
		}
		if (valueName == "BPMS") {
			out.SetBpms(matcher);
		}
	}

	// After iteration, calculate the hashes based on what we found.
	out.SetGSHashes();

	return false;
}

bool NotesLoader::LoadFromDir(const std::string &sPath, Song &out)
{
	// bwaggone note: This function has been greatly simplified, it usually grabs an appropriate loader,
	// and parses everything into rage data structures. Instead, just do the chart loading

	MsdFile msd;
	if (!msd.ReadFile(sPath, true))
	{
		return false;
	}
	// TODO: Check if sm or scc file, and change loading appropriately.
	if (sPath.find(".ssc") != std::string::npos) {
		SSCLoader loader;
		loader.LoadFromSimfile(sPath, out);
	}
	else if (sPath.find(".sm") != std::string::npos) {
		LoadFromSM(msd, out);
	}

	return false;
}
