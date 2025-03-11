// This is a file that contains a bunch of utilities specific to ITGm.
// IMO lots of these should be replaced with standard library functions, but that's a chore for another day
#ifndef RAGE_UTIL_H
#define RAGE_UTIL_H

class RageUtil {
public:
	// Safely delete pointers.
	template <typename T>
	inline static void SafeDelete(T*& p) noexcept
	{
		delete p;
		p = nullptr;
	}

	// Safely delete array pointers.
	template <typename T>
	inline static void SafeDeleteArray(T*& p) noexcept
	{
		delete[] p;
		p = nullptr;
	}
};

template<typename TO, typename FROM>
struct ConvertValueHelper
{
	explicit ConvertValueHelper(FROM* pVal) : m_pFromValue(pVal)
	{
		m_ToValue = static_cast<TO>(*m_pFromValue);
	}

	~ConvertValueHelper()
	{
		*m_pFromValue = static_cast<FROM>(m_ToValue);
	}

	TO& operator *() { return m_ToValue; }
	operator TO* () { return &m_ToValue; }

private:
	FROM* m_pFromValue;
	TO m_ToValue;
};

template<typename TO, typename FROM>
ConvertValueHelper<TO, FROM> ConvertValue(FROM* pValue)
{
	return ConvertValueHelper<TO, FROM>(pValue);
}


template<typename T>
static inline void enum_add(T& val, int iAmt)
{
	val = static_cast<T>(val + iAmt);
}

namespace util {

	void Trim(std::string& sStr, const char* s = "\r\n\t ");

	std::string join(const std::string& sDeliminator, const std::vector<std::string>& sSource);

	std::string upper(std::string in);

	template <class S, class C>
	void do_split(const S& Source, const C Delimitor, std::vector<S>& AddIt, const bool bIgnoreEmpty);

	void split(const std::string& sSource, const std::string& sDelimitor, std::vector<std::string>& asAddIt, const bool bIgnoreEmpty = true);

	std::string BinaryToHex(const void* pData_, std::size_t iNumBytes);

	std::string BinaryToHex(const std::string& sString);

	std::string GetSHA1ForString(std::string sData);

	float HHMMSSToSeconds(const std::string& sHHMMSS);

	void TrimRight(std::string& sStr, const char* szTrim = "\r\n\t ");
}

#endif