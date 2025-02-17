/*
 * The original MSD format is simply:
 * 
 * #PARAM0:PARAM1:PARAM2:PARAM3;
 * #NEXTPARAM0:PARAM1:PARAM2:PARAM3;
 * 
 * (The first field is typically an identifier, but doesn't have to be.)
 *
 * The semicolon is not optional, though if we hit a # on a new line, eg:
 * #VALUE:PARAM1
 * #VALUE2:PARAM2
 * we'll recover.
 */

#include "MsdFile.h"

#include <string>
#include <fstream>

void MsdFile::AddParam( const char *buf, int len )
{
	values.back().params.push_back( std::string(buf, len) );
}

void MsdFile::AddValue() /* (no extra charge) */
{
	values.push_back( value_t() );
	values.back().params.reserve( 32 );
}

void MsdFile::ReadBuf( const char *buf, int len, bool bUnescape )
{
	values.reserve( 64 );

	bool ReadingValue=false;
	int i = 0;
	char *cProcessed = new char[len];
	int iProcessedLen = -1;
	while( i < len )
	{
		if( i+1 < len && buf[i] == '/' && buf[i+1] == '/' )
		{
			/* Skip a comment entirely; don't copy the comment to the value/parameter */
			do
			{
				i++;
			} while( i < len && buf[i] != '\n' );

			continue;
		}

		if( ReadingValue && buf[i] == '#' )
		{
			/* Unfortunately, many of these files are missing ;'s.
			 * If we get a # when we thought we were inside a value, assume we
			 * missed the ;.  Back up and end the value. */
			// Make sure this # is the first non-whitespace character on the line.
			bool FirstChar = true;
			int j = iProcessedLen;
			while( j > 0 && cProcessed[j - 1] != '\r' && cProcessed[j - 1] != '\n' )
			{
				if( cProcessed[j - 1] == ' ' || cProcessed[j - 1] == '\t' )
				{
					--j;
					continue;
				}

				FirstChar = false;
				break;
			}

			if( !FirstChar )
			{
				/* We're not the first char on a line.  Treat it as if it were a normal character. */
				cProcessed[iProcessedLen++] = buf[i++];
				continue;
			}

			/* Skip newlines and whitespace before adding the value. */
			iProcessedLen = j;
			while( iProcessedLen > 0 &&
			       ( cProcessed[iProcessedLen - 1] == '\r' || cProcessed[iProcessedLen - 1] == '\n' ||
			         cProcessed[iProcessedLen - 1] == ' ' || cProcessed[iProcessedLen - 1] == '\t' ) )
				--iProcessedLen;

			AddParam( cProcessed, iProcessedLen );
			iProcessedLen = 0;
			ReadingValue=false;
		}

		/* # starts a new value. */
		if( !ReadingValue && buf[i] == '#' )
		{
			AddValue();
			ReadingValue=true;
		}

		if( !ReadingValue )
		{
			if( bUnescape && buf[i] == '\\' )
				i += 2;
			else
				++i;
			continue; /* nothing else is meaningful outside of a value */
		}

		/* : and ; end the current param, if any. */
		if( iProcessedLen != -1 && (buf[i] == ':' || buf[i] == ';') )
			AddParam( cProcessed, iProcessedLen );

		/* # and : begin new params. */
		if( buf[i] == '#' || buf[i] == ':' )
		{
			++i;
			iProcessedLen = 0;
			continue;
		}

		/* ; ends the current value. */
		if( buf[i] == ';' )
		{
			ReadingValue=false;
			++i;
			continue;
		}

		/* We've gone through all the control characters.  All that is left is either an escaped character, 
		 * ie \#, \\, \:, etc., or a regular character. */
		if(buf[i] == '\\' && i < len)
		{
			// If we're escaping the next character, skip the '\\'
			if(bUnescape)
			{
				++i;
			}
			// Otherwise, add the '\\' to cProcessed here, so that
			// whatever character is coming next stays escaped in
			// the resulting value/parameter string 
			// (and most importantly, it doesn't get parsed as a control character)
			// on the next iteration
			else 
			{
				cProcessed[iProcessedLen++] = buf[i++];	
			}
		}
		
		if( i < len )
		{
			cProcessed[iProcessedLen++] = buf[i++];
		}
	}

	/* Add any unterminated value at the very end. */
	if( ReadingValue )
		AddParam( cProcessed, iProcessedLen );

	delete [] cProcessed;
}

// returns true if successful, false otherwise
bool MsdFile::ReadFile( std::string sNewPath, bool bUnescape )
{
	error = "";

	std::ifstream f(sNewPath);
	/* Open a file. */
	if( f.fail() )
	{
		return false;
	}

	// NOTE from bwaggone: This differs from ITGm by using a built in ifstream
	// rather than the rage file manager system.
	std::string FileString((std::istreambuf_iterator<char>(f)),
		std::istreambuf_iterator<char>());

	ReadBuf( FileString.c_str(), FileString.size(), bUnescape);

	return true;
}

void MsdFile::ReadFromString( const std::string &sString, bool bUnescape )
{
	ReadBuf( sString.c_str(), sString.size(), bUnescape );
}

std::string MsdFile::GetParam(unsigned val, unsigned par) const
{
	if( val >= GetNumValues() || par >= GetNumParams(val) )
		return std::string();

	return values[val].params[par];
}
