#include "EnginePCH.h"
#include "FileHelper.h"

void GetFileData(std::string const &sFile, void *&pData, size_t &nFileSize)
{
	FILE *f;
	fopen_s(&f, sFile.c_str(), "rb");

	if (!f)
		return;

	fseek(f, 0, SEEK_END);
	size_t nSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (!nSize)
	{
		fclose(f);

		return;
	}

	pData = malloc(nSize);
	fread_s(pData, nSize, nSize, 1, f);

	nFileSize = nSize;

	fclose(f);
}

bool IsFileExists(std::string const &sFile)
{
	std::ifstream File(sFile.c_str());

	return File.is_open();
}