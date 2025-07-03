#include <filesystem>
#include <vector>
#include <iostream>
#include <filesystem>

#include "itgm-lib/NotesLoader.h"

using namespace std;
using namespace std::filesystem;


void GetGrooveStatsHashes(Song& song)
{
	for (Steps* step : song.GetSteps()) {
		std::cout << song.GetPath() << "\nStyle: " << step->GetStepsTypeString() << "\tDifficulty: " << kDifficultyToString[step->GetDifficulty()] << "\tHash: " << step->GetGSHash() << "\n";
	}
}

// Function to recursively find files with specified extensions in a directory
// Parameters:
//   directory_path: The starting directory to search within.
//   extensions: A vector of strings representing the desired file extensions (e.g., {".ssc", ".sm"}).
// Returns:
//   A vector containing all files found within the directory that match the file extension.
std::vector<std::string> findFilesByExtension(const std::filesystem::path& directory_path, const std::vector<std::string>& extensions) {
    std::vector<std::string> found_files;
    
    // A map from base path to file. Required to prioritize ssc over sm.
    std::unordered_map<std::string, std::string> files_by_path;
    
    // Check if the provided path is a valid directory
    if (!std::filesystem::exists(directory_path) || !std::filesystem::is_directory(directory_path)) {
        std::cerr << "Error: Directory not found or is not a directory: " << directory_path << "\n";
        return found_files;
    }

    // Iterate through all entries (files and subdirectories) in the given directory
    // std::filesystem::recursive_directory_iterator is used for recursive traversal
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
        // Check if the current entry is a regular file
        if (std::filesystem::is_regular_file(entry.status())) {
            // Get the extension of the current file
            std::filesystem::path current_path = entry.path();
            std::string file_extension = current_path.extension().string();

            // Convert the file extension to lowercase for case-insensitive comparison
            // This ensures that ".SSC" matches ".ssc", for example.
            std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(),
                [](unsigned char c) { return std::tolower(c); });

            // Check if the file's extension matches any of the desired extensions
            for (const std::string& ext : extensions) {
                // Convert the target extension to lowercase for consistent comparison
                std::string lower_ext = ext;

                std::transform(lower_ext.begin(), lower_ext.end(), lower_ext.begin(),
                    [](unsigned char c) { return std::tolower(c); });

                if (file_extension == lower_ext) {
                    std::string base_name = current_path.stem().string(); // Filename without extension
                    std::string parent_dir = current_path.parent_path().string();

                    std::string unique_key = parent_dir + "/" + base_name;

                    if (file_extension == ".ssc" || files_by_path[unique_key].empty()) {
                        files_by_path[unique_key] = current_path.string();
                        // If a match is found, print the full path of the file
                        found_files.push_back(entry.path().string());
                    }
                    else {
                        std::cout << "Found file " << entry.path().string() << ", but .ssc was already added.\n";
                    }
                    break; // No need to check other extensions for this file
                }
            }
        }
    }
    return found_files;
}

int main(int argc, char **argv)
{
    std::cout << "Beginning Recursive Read through: " << argv[1] << "\n";
    std::vector<std::string> extensions{ ".ssc", ".sm" };
    std::vector<std::string> files_to_hash = findFilesByExtension(argv[1], extensions);

	for (const auto& f : files_to_hash) {
		Song out(f);
		NotesLoader::LoadFromDir(f, out);
		GetGrooveStatsHashes(out);
	}
	return 0;
}
