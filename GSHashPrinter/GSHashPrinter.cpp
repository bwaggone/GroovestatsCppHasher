#include <filesystem>
#include <vector>
#include <filesystem>

#include "GSHashPrinter.h"

#include "itgm-lib/NotesLoader.h"
#include "itgm-lib/RageUtil.h"

using namespace std;
using namespace std::filesystem;


void GetGrooveStatsHashes(Song& song)
{
	for (Steps& step : song.GetSteps()) {
		std::cout << song.GetPath() << "\nStyle: " << step.GetStepsType() << "\tDifficulty: " << step.GetDifficulty() << "\tHash : " << step.GetGSHash() << "\n";
	}
}

int main(int argc, char **argv)
{
    std::cout << "Beginning Recursive Read through: " << argv[1] << "\n";
    std::vector<std::string> files_to_hash;
    for (const auto& pack : std::filesystem::directory_iterator(argv[1])) {
		if (pack.is_directory()) {
			for (const auto& song_dir : std::filesystem::directory_iterator(pack.path())) {
				if (song_dir.is_directory()) {
					std::string chosen_file = "";
					for (const auto& entry : std::filesystem::directory_iterator(song_dir)) {
						if (std::filesystem::path(entry).extension() == ".ssc") {
							chosen_file = entry.path().generic_string();
						}
						else if (std::filesystem::path(entry).extension() == ".sm" && chosen_file.empty()) {
							chosen_file = entry.path().generic_string();
						}
					}
					if (!chosen_file.empty()) {
						files_to_hash.push_back(chosen_file);
					}
				}
			}
		}
	}
	for (const auto& f : files_to_hash) {
		Song out(f);
		NotesLoader::LoadFromDir(f, out);
		GetGrooveStatsHashes(out);
	}
	return 0;
}
