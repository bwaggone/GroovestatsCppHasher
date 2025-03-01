#ifndef NotesLoaderSM_H
#define NotesLoaderSM_H

#include "MsdFile.h" // we require the struct from here.

#include <vector>


class Song;
class Steps;
//class TimingData;

/**
 * @brief The highest allowable speed before Warps come in.
 *
 * This was brought in from StepMania 4's recent betas. */
const float FAST_BPM_WARP = 9999999.f;

/** @brief The maximum file size for edits. */
const int MAX_EDIT_STEPS_SIZE_BYTES = 60 * 1024;	// 60KB

/** @brief Reads a Song from an .SM file. */
struct SMLoader
{
	SMLoader() : fileExt(".sm"), songTitle() {}

	SMLoader(std::string ext) : fileExt(ext), songTitle() {}

	virtual ~SMLoader() {}

protected:

	/**
	 * @brief Retrieve the file extension associated with this loader.
	 * @return the file extension. */
	std::string GetFileExtension() const { return fileExt; }

public:

private:
	/** @brief The file extension in use. */
	const std::string fileExt;
	/** @brief The song title that is being processed. */
	std::string songTitle;

	std::vector<std::string> m_SongDirFiles;
};

#endif
