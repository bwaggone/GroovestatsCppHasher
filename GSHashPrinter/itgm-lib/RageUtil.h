// This is a file that contains a bunch of utilities specific to ITGm.
// IMO lots of these should be replaced with standard library functions, but that's a chore for another day

namespace util {
	void Trim(std::string& sStr, const char* s = "\r\n\t ");

	std::string join(const std::string& sDeliminator, const std::vector<std::string>& sSource);

	template <class S, class C>
	void do_split(const S& Source, const C Delimitor, std::vector<S>& AddIt, const bool bIgnoreEmpty);

	void split(const std::string& sSource, const std::string& sDelimitor, std::vector<std::string>& asAddIt, const bool bIgnoreEmpty = true);

	std::string BinaryToHex(const void* pData_, std::size_t iNumBytes);

	std::string BinaryToHex(const std::string& sString);

	std::string GetSHA1ForString(std::string sData);
}