#include <regex>

#include "Steps.h"
#include "RageUtil.h"

std::string Steps::MinimizeChartString() {

	// We can potentially minimize the chart to get the most compressed
	// form of the actual chart data.
	// NOTE(teejusb): This can be more compressed than the data actually
	// generated by StepMania. This is okay because the charts would still
	// be considered equivalent.
	// E.g. 0000                      0000
	// 		0000  -- minimized to -->
	// 		0000
	// 		0000
	// 		StepMania will always generate the former since quarter notes are
	// 		the smallest quantization.

	std::regex anyNote("[^0]");

	std::vector<std::string> measures;
	std::string minimizedNoteData = "";
	util::split(raw_chart_, ",", measures, true);
	for (unsigned m = 0; m < measures.size(); m++)
	{

		util::Trim(measures[m]);
		bool isEmpty = true;
		bool allZeroes = true;
		bool minimal = false;
		std::vector<std::string> lines;
		util::split(measures[m], "\n", lines, true);
		while (!minimal && lines.size() % 2 == 0)
		{
			// If every other line is all 0s, we can minimize the measure
			for (unsigned i = 1; i < lines.size(); i += 2)
			{
				util::Trim(lines[i]);
				std::cmatch m;
				if (std::regex_search(lines[i].c_str(), m, anyNote))
				{
					allZeroes = false;
					break;
				}
			}

			if (allZeroes)
			{
				// Iterate through lines, removing every other item.
				// Note that we're removing the odd indices, so we
				// call `++it;` and then erase the following line
				auto it = lines.begin();
				while (it != lines.end())
				{
					++it;
					if (it != lines.end())
					{
						it = lines.erase(it);
					}
				}
			}
			else
			{
				minimal = true;
			}
		}

		// Once the measure has been minimized, make sure all of the lines are
		// actually trimmed.
		// (for some reason, the chart returned by GetSMNoteData() have a lot
		//  of extra newlines)
		for (unsigned l = 0; l < lines.size(); l++)
		{
			util::Trim(lines[l]);
		}

		// Then, rejoin the lines together to make a measure,
		// and add it to minimizedNoteData.
		minimizedNoteData += util::join("\n", lines);
		if (m < measures.size() - 1)
		{
			minimizedNoteData += "\n,\n";
		}
	}
	return minimizedNoteData;
}


void Steps::CalculateAndSetGSHash(std::string bpm_string) {
	std::string chart_and_bpms = MinimizeChartString() + bpm_string;
	SetGSHash(util::BinaryToHex(util::GetSHA1ForString(chart_and_bpms)).substr(0, 16));
}
