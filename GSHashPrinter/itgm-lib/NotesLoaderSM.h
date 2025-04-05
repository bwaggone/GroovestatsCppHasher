#ifndef NotesLoaderSM_H
#define NotesLoaderSM_H

#include "Attack.h"
#include "BackgroundUtil.h"
#include "MsdFile.h" // we require the struct from here.
#include "TimingData.h"

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
	// SetSongTitle and GetSongTitle changed to public to allow the functions
	// used by the parser helper to access them. -Kyz
	/**
	 * @brief Set the song title.
	 * @param t the song title. */
	virtual void SetSongTitle(const std::string& title);

	/**
	 * @brief Get the song title.
	 * @return the song title. */
	virtual std::string GetSongTitle() const;

	float RowToBeat(std::string line, const int rowsPerBeat);

	void ProcessInstrumentTracks(Song& out, const std::string& sParam);
	void ProcessBGChanges(Song& out, const std::string& sValueName, const std::string& sPath, const std::string& sParam);
	void ParseBGChangesString(const std::string& _sChanges, std::vector<std::vector<std::string> >& vvsAddTo, const std::string& sSongDir);
	std::vector<std::string> GetSongDirFiles(const std::string& sSongDir);
	bool LoadFromBGChangesVector(BackgroundChange& change, std::vector<std::string> aBGChangeValues);
	void ProcessAttackString(std::vector<std::string>& attacks, MsdFile::value_t params);
	void ProcessAttacks(AttackArray& attacks, MsdFile::value_t params);
	void ProcessDelays(TimingData& out, std::string line, int rowsPerBeat = -1);
	void ProcessTimeSignatures(TimingData& out, const std::string line, const int rowsPerBeat = -1);
	void ProcessTickcounts(TimingData& out, const std::string line, const int rowsPerBeat = -1);
	virtual void ProcessSpeeds(TimingData& out, const std::string line, const int rowsPerBeat = -1);
	virtual void ProcessCombos(TimingData&,const std::string line, const int rowsPerBeat = -1) {}
	virtual void ProcessFakes(TimingData& out, const std::string line, const int rowsPerBeat = -1);
	void TidyUpData(Song& song, bool bFromCache);

private:
	/** @brief The file extension in use. */
	const std::string fileExt;
	/** @brief The song title that is being processed. */
	std::string songTitle;

	std::vector<std::string> m_SongDirFiles;
};

#endif
