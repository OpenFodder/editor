#include "stdafx.hpp"

std::string local_PathGenerate( const std::string& pFile, const std::string& pPath, bool pData = true ) {
	std::stringstream	 filePathFinal;

	if (pData)
		filePathFinal << "Data/";
	else
		filePathFinal << "Saves/";

	if (pPath.size())
		filePathFinal << pPath << "/";

	filePathFinal << pFile;

	return filePathFinal.str();
}

uint8 *local_FileRead( const std::string& pFile, const std::string& pPath, size_t& pFileSize, bool pFullPath ) {
	std::ifstream*		fileStream;
	uint8*				fileBuffer = 0;
	std::string			finalPath;

	if (pFullPath)
		finalPath = pFile;
	else
		finalPath = local_PathGenerate( pFile, pPath );

	// Attempt to open the file
	fileStream = new std::ifstream( finalPath.c_str(), std::ios::binary );
	if (fileStream->is_open() != false) {

		// Get file size
		fileStream->seekg( 0, std::ios::end );
		pFileSize = (size_t)fileStream->tellg();
		fileStream->seekg( std::ios::beg );

		// Allocate buffer, and read the file into it
		fileBuffer = new uint8[pFileSize];
		fileStream->read( (char*)fileBuffer, pFileSize );
		if (!(*fileStream)) {
			delete fileBuffer;
			fileBuffer = 0;
		}

	}

	// Close the stream
	fileStream->close();
	delete fileStream;

	// All done ;)
	return fileBuffer;
}

void tool_EndianSwap( uint8 *pBuffer, size_t pSize ) {
	uint8 *pDest = pBuffer;

	pSize /= 2;

	while (pSize--) {
		uint8 al = *pBuffer++;
		uint8 ah = *pBuffer++;

		*pDest++ = ah;
		*pDest++ = al;
	}
}
