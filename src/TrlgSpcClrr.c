#include "far/plugin.hpp"
#include "common.h"

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
// #include <wchar.h> // fwprintf

static const wchar_t* TITLE = L"TrlgSpcClrr";
static const wchar_t* DESCRIPTION = L"Colors trailing spacecharacters.";
static const VersionInfo PLUGIN_VERSION = {1, 0, 0, 1, VS_RELEASE};

static const unsigned int PLUGIN_COLORING_PRIORITY = 3;
static const FarColor PLUGIN_COLOR_USED = {FCF_4BITMASK, {0}, {4}, {0, 0}};

/*
DLL_EXPORT WINAPI void GetPluginInfoW(PluginInfo* info) {
	info->StructSize = sizeof(PluginInfo);
	info->Flags = PF_DISABLEPANELS;
	info->CommandPrefix = NULL;
}

 DLL_EXPORT WINAPI HANDLE OpenW(const OpenInfo* info) {
	MessageBoxW(0, L"OpenW", NULL, 0);
	EXPLICITLY_UNUSED(info);
	return NULL;
}
*/

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
	
	RegisterValue endPos = lineLength - 1;
	if (!isSpaceCharacter(line[endPos])) {
		return;
	}
	RegisterValue beginPos = endPos;
	for (; beginPos >= 1 && isSpaceCharacter(line[beginPos - 1]); --beginPos);
	
	// Если красить весь отрезок сразу, то не будет работать с плагином TabColorer. А если так, то
	// будет.
	for (RegisterValue pos = beginPos; pos <= endPos; ++pos) {
		colorCharacter(lineNumber, pos, PLUGIN_COLOR_USED, ECF_NONE);
	}
}