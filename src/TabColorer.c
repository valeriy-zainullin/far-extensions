#include "far/plugin.hpp"
#include "common.h"

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

static const wchar_t* TITLE = L"TabColorer";
static const wchar_t* DESCRIPTION = L"Colors tabs that are not leading spacecharacters.";
static const VersionInfo PLUGIN_VERSION = {1, 0, 0, 1, VS_RELEASE};

static const unsigned int PLUGIN_COLORING_PRIORITY = 4;
static const FarColor PLUGIN_COLOR_USED = {FCF_4BITMASK, {0}, {14}, {0, 0}};

static void newColoring() {}

static bool isSpaceCharacter(const wchar_t character) {
	return character == L' ' || character == L'\t';
}

static void colorLine(const RegisterValue lineNumber) {
	const wchar_t* line;
	RegisterValue lineLength;
	getLine(lineNumber, &line, &lineLength);
	if (lineLength == 0) {
		return;
	}
	
	// RegisterValue beginPos = 0; [beginPos, end)
	RegisterValue end = 0;
	for (; end < lineLength && isSpaceCharacter(line[end]); ++end);
	
	RegisterValue firstPosToProcess = end;
	if (end == lineLength) {
		firstPosToProcess = 0;
	}
	
	for (RegisterValue pos = firstPosToProcess; pos < lineLength; ++pos) {
		if (line[pos] != L'\t') {
			continue;
		}
		
		colorCharacter(lineNumber, pos, PLUGIN_COLOR_USED, ECF_NONE);
	}
}