#pragma once

#include "EnginePCH.h"

void GetFileData(std::string const &sFile, void *&pData, size_t &nFileSize);

bool IsFileExists(std::string const &sFile);