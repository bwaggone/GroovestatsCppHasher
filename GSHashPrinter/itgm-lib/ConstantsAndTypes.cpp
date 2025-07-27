#include "ConstantsAndTypes.h"


Difficulty OldStyleStringToDifficulty(const std::string& sDC)
{
	std::string s2 = sDC;

	std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
	auto diff =
		OldStyleStringToDifficulty_converter.conversion_map.find(s2);
	if (diff != OldStyleStringToDifficulty_converter.conversion_map.end())
	{
		return diff->second;
	}
	return Difficulty_Invalid;
}