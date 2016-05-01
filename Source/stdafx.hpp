#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <SDL.h>

#include "Types.hpp"
#include "Singleton.hpp"

#include "Resources.hpp"
#include "Resource_PC_CD.hpp"
#include "Surface.hpp"
#include "Sprites.hpp"
#include "OFED.hpp"
#include "ofedApp.h"

uint8*		local_FileRead( const std::string& pFile, const std::string& pPath, size_t& pFileSize, bool pFullPath = false );
std::string local_PathGenerate( const std::string& pFile, const std::string& pPath, bool pData );

void		tool_EndianSwap( uint8* pBuffer, size_t pSize );

// Read a BE word from the buffer
inline uint16 readBEWord( const void *buffer ) {
	const uint8* bytes = (const uint8*)buffer;

	return uint16( (bytes[0] << 8) + bytes[1] );
}

inline void writeBEWord( const void *buffer, uint16 pValue ) {
	uint8* bytes = (uint8*)buffer;

	bytes[0] = (uint8)(pValue >> 8);
	bytes[1] = (uint8)(pValue & 0xFF);
}


inline uint32 readBEDWord( const void *buffer ) {
	const uint8* bytes = (const uint8*)buffer;

	return uint32( (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + (bytes[3]) );
}

// Read a word from the buffer
inline uint32 readLEDWord( const void *buffer ) {
	const uint32* wordBytes = (const uint32*)buffer;
	return *wordBytes;
}

// Read a word from the buffer
inline uint16 readLEWord( const void *buffer ) {
	const uint16* wordBytes = (const uint16*)buffer;
	return *wordBytes;
}

inline void writeLEWord( const void *buffer, uint16 pValue ) {
	uint16* wordBytes = (uint16 *)buffer;
	*wordBytes = pValue;
}


#define g_OFED	cOFED::GetSingleton()
#define g_Resource cResources::GetSingleton()
