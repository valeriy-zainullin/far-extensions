#include "far/plugin.hpp"
#include "common.h"

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
// #include <wchar.h> // fwprintf

static const wchar_t* TITLE = L"TrlgSpcClrr";
static const wchar_t* DESCRIPTION = L"Colors trailing spacecharacters.";
static const VersionInfo PLUGIN_VERSION = {1, 0, 0, 1, VS_RELEASE};

static const unsigned int PLUGIN_COLORING_PRIORITY = 2;
static const FarColor PLUGIN_COLOR_USED = {FCF_4BITMASK, {0}, {4}};

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

static bool isSpaceCharacter(const wchar_t character) {
	return character == L' ' || character == L'\t';
}

static void colorLine(const unsigned long long lineNumber) {
	EditorGetString editorString = {sizeof(EditorGetString)};
	editorString.StringNumber = lineNumber;
	intptr_t editorControlResult = farAPI.EditorControl(
		CURRENT_EDITOR,
		ECTL_GETSTRING,
		(intptr_t) 0,
		&editorString
	);
	assert((MemoryAddress) editorControlResult == 1);
	EXPLICITLY_UNUSED(editorControlResult);
	
	const wchar_t* line = editorString.StringText;
	unsigned long long lineLength = (MemoryAddress) editorString.StringLength;
	if (lineLength == 0) {
		/*
		DEBUG_PRINTF("Line %llu: ", lineNumber);
		DEBUG_PRINTF("length is %llu.\n", lineLength);
		*/
		return;
	}
	
	unsigned long long endPos = lineLength - 1;
	if (!isSpaceCharacter(line[endPos])) {
		/*
		DEBUG_PRINTF("Line %llu: ", lineNumber);
		DEBUG_PRINTF("length is %llu, no trailing spacecharacters. ", lineLength);
		// fwprintf(stderr, line);
		DEBUG_PRINTF("line[endPos] = %d.\n", (int) line[endPos]);
		*/
		return;
	}
	unsigned long long beginPos = endPos;
	for (; beginPos >= 1 && isSpaceCharacter(line[beginPos - 1]); --beginPos);
	
	/*
	DEBUG_PRINTF("Line %llu: ", lineNumber);
	DEBUG_PRINTF("length is %llu, ", lineLength);
	DEBUG_PRINTF("trailing spacecharacters start at %llu and ", beginPos);
	DEBUG_PRINTF("end at %llu.\n", endPos);
	*/
	
	EditorColor editorColor = {
		sizeof(EditorColor),
		(intptr_t) (MemoryAddress) lineNumber,
		(intptr_t) (MemoryAddress) 0,
		(intptr_t) beginPos,
		(intptr_t) endPos,
		(uintptr_t) PLUGIN_COLORING_PRIORITY,
		ECF_AUTODELETE,
		PLUGIN_COLOR_USED,
		pluginGUID
	};
	editorControlResult = farAPI.EditorControl(
		CURRENT_EDITOR,
		ECTL_ADDCOLOR,
		(intptr_t) 0,
		&editorColor
	);
	assert((MemoryAddress) editorControlResult == 1);
	EXPLICITLY_UNUSED(editorControlResult);
}