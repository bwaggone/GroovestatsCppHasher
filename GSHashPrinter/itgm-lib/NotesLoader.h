#ifndef NOTES_LOADER_H
#define NOTES_LOADER_H

#include <set>
#include <string>
#include "MsdFile.h"
#include "Song.h"

/** @brief Base class for step file loaders. */
namespace NotesLoader
{
	/**
	 * @brief Identify the main and sub titles from a full title.
	 * @param sFullTitle the full title.
	 * @param sMainTitleOut the eventual main title.
	 * @param sSubTitleOut the ventual sub title. */
	void GetMainAndSubTitlesFromFullTitle( const std::string &sFullTitle, 
					      std::string &sMainTitleOut, std::string &sSubTitleOut );

	/**
	 * @brief Attempt to load a Song from the given directory.
	 * @param sPath the path to the file.
	 * @param out the Song in question.
	 * @param BlacklistedImages images to exclude (DWI files only for some reason).
	 * @return its success or failure. */
	bool LoadFromDir( const std::string &sPath, Song &out );

	bool LoadFromSM(MsdFile& in, Song& out);

	enum SSCLoadingStates
	{
		GETTING_SONG_INFO, /**< Retrieving song information. */
		GETTING_STEP_INFO, /**< Retrieving step information. */
		NUM_SSCLoadingStates /**< The number of states used. */
	};
}

#endif
