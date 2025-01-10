// This is a file that contains a bunch of utilities specific to ITGm.
// IMO lots of these should be replaced with standard library functions, but that's a chore for another day
#include <string>
#include <sstream>
#include <tomcrypt.h>
#include <vector>

#include "RageUtil.h"


// Some jank custom stringy code

namespace util {

	template <class S>
	static int DelimitorLength(const S& Delimitor)
	{
		return Delimitor.size();
	}

	static int DelimitorLength(char Delimitor)
	{
		return 1;
	}

	static int DelimitorLength(wchar_t Delimitor)
	{
		return 1;
	}

	void Trim(std::string& sStr, const char* s)
	{
		int b = 0, e = sStr.size();
		while (b < e && strchr(s, sStr[b]))
			++b;
		while (b < e && strchr(s, sStr[e - 1]))
			--e;
		sStr.assign(sStr.substr(b, e - b));
	}

	std::string join(const std::string& sDeliminator, const std::vector<std::string>& sSource)
	{
		if (sSource.empty())
			return std::string();

		std::string sTmp;
		std::size_t final_size = 0;
		std::size_t delim_size = sDeliminator.size();
		for (std::size_t n = 0; n < sSource.size() - 1; ++n)
		{
			final_size += sSource[n].size() + delim_size;
		}
		final_size += sSource.back().size();
		sTmp.reserve(final_size);

		for (unsigned iNum = 0; iNum < sSource.size() - 1; iNum++)
		{
			sTmp += sSource[iNum];
			sTmp += sDeliminator;
		}
		sTmp += sSource.back();
		return sTmp;
	}

	template <class S, class C>
	void do_split(const S& Source, const C Delimitor, std::vector<S>& AddIt, const bool bIgnoreEmpty)
	{
		/* Short-circuit if the source is empty; we want to return an empty vector if
		 * the string is empty, even if bIgnoreEmpty is true. */
		if (Source.empty())
			return;

		std::size_t startpos = 0;

		do {
			std::size_t pos;
			pos = Source.find(Delimitor, startpos);
			if (pos == Source.npos)
				pos = Source.size();

			if (pos - startpos > 0 || !bIgnoreEmpty)
			{
				/* Optimization: if we're copying the whole string, avoid substr; this
				 * allows this copy to be refcounted, which is much faster. */
				if (startpos == 0 && pos - startpos == Source.size())
					AddIt.push_back(Source);
				else
				{
					const S AddRString = Source.substr(startpos, pos - startpos);
					AddIt.push_back(AddRString);
				}
			}

			startpos = pos + DelimitorLength(Delimitor);
		} while (startpos <= Source.size());
	}

	void split(const std::string& sSource, const std::string& sDelimitor, std::vector<std::string>& asAddIt, const bool bIgnoreEmpty) {
		if (sDelimitor.size() == 1)
			do_split(sSource, sDelimitor[0], asAddIt, bIgnoreEmpty);
		else
			do_split(sSource, sDelimitor, asAddIt, bIgnoreEmpty);
	}

	std::string BinaryToHex(const void* pData_, std::size_t iNumBytes)
	{
		const unsigned char* pData = (const unsigned char*)pData_;
		std::string s;
		unsigned total = 0;
		std::stringstream stream;
		for (std::size_t i = 0; i < iNumBytes; i++)
		{
			unsigned val = pData[i];
			stream << std::hex << val;
		}
		std::string result(stream.str());
		return result;
	}

	std::string BinaryToHex(const std::string& sString) {
		return BinaryToHex(sString.data(), sString.size());
	}

	std::string GetSHA1ForString(std::string sData)
	{
		unsigned char digest[20];

		int iHash = register_hash(&sha1_desc);

		hash_state hash;
		hash_descriptor[iHash].init(&hash);
		hash_descriptor[iHash].process(&hash, (const unsigned char*)sData.data(), sData.size());
		hash_descriptor[iHash].done(&hash, digest);

		return std::string((const char*)digest, sizeof(digest));
	}

}