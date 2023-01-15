#include "defFileCrector.h"
#include <fstream>

int writeDefFile (const char* szFilename)
{
	int nRet = 0;
	std::ofstream os(szFilename);
	do {
		if (!os) {
			nRet = 1;
			break;
		}
		const char* szCon = R"(LIBRARY
EXPORTS
	afterLoad	@1
	beforeUnload	@2)";
		os<<szCon;
	} while (0);

	return nRet;
}
