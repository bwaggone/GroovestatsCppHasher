#ifndef BACKGROUND_UTIL_H
#define BACKGROUND_UTIL_H

#include <string>

struct BackgroundDef
{
	bool operator<(const BackgroundDef& other) const;
	bool operator==(const BackgroundDef& other) const;
	bool IsEmpty() const { return m_sFile1.empty() && m_sFile2.empty(); }
	std::string	m_sEffect;	// "" == automatically choose
	std::string m_sFile1;	// must not be ""
	std::string m_sFile2;	// may be ""
	std::string m_sColor1;	// "" == use default
	std::string m_sColor2;	// "" == use default

	/** @brief Set up the BackgroundDef with default values. */
	BackgroundDef() : m_sEffect(""), m_sFile1(""), m_sFile2(""),
		m_sColor1(""), m_sColor2("") {}

	/**
	 * @brief Set up the BackgroundDef with some defined values.
	 * @param effect the intended effect.
	 * @param f1 the primary filename for the definition.
	 * @param f2 the secondary filename (optional). */
	BackgroundDef(std::string effect, std::string f1, std::string f2) :
		m_sEffect(effect), m_sFile1(f1), m_sFile2(f2),
		m_sColor1(""), m_sColor2("") {}
};

struct BackgroundChange
{
	BackgroundChange() : m_def(), m_fStartBeat(-1), m_fRate(1),
		m_sTransition("") {}

	BackgroundChange(
		float s,
		std::string f1,
		std::string f2 = "",
		float r = 1.f,
		std::string e = "Centered",
		std::string t = std::string()
	) :
		m_def(e, f1, f2), m_fStartBeat(s),
		m_fRate(r), m_sTransition(t) {}

	BackgroundDef m_def;
	float m_fStartBeat;
	float m_fRate;
	std::string m_sTransition;
};

#endif