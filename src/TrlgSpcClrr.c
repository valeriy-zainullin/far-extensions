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

static bool isSpaceCharacter(const wchar_t character) {
	return character == L' ' || character == L'\t';
}

static void colorLine(const RegisterValue lineNumber) {
	EditorGetString editorString;
	editorString.StructSize = sizeof(EditorGetString);
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
	RegisterValue lineLength = (MemoryAddress) editorString.StringLength;
	if (lineLength == 0) {
		/*
		DEBUG_PRINTF("Line %llu: ", lineNumber);
		DEBUG_PRINTF("length is %llu.\n", lineLength);
		*/
		return;
	}
	
	RegisterValue endPos = lineLength - 1;
	if (!isSpaceCharacter(line[endPos])) {
		/*
		DEBUG_PRINTF("Line %llu: ", lineNumber);
		DEBUG_PRINTF("length is %llu, no trailing spacecharacters. ", lineLength);
		// fwprintf(stderr, line);
		DEBUG_PRINTF("line[endPos] = %d.\n", (int) line[endPos]);
		*/
		return;
	}
	RegisterValue beginPos = endPos;
	for (; beginPos >= 1 && isSpaceCharacter(line[beginPos - 1]); --beginPos);
	
	/*
	DEBUG_PRINTF("Line %llu: ", lineNumber);
	DEBUG_PRINTF("length is %llu, ", lineLength);
	DEBUG_PRINTF("trailing spacecharacters start at %llu and ", beginPos);
	DEBUG_PRINTF("end at %llu.\n", endPos);
	*/
	// Если красить весь отрезок сразу, то не будет работать с плагином TabColorer. А если так, то
	// будет.
	for (RegisterValue pos = beginPos; pos <= endPos; ++pos) {
		EditorColor editorColor;
		editorColor.StructSize = sizeof(EditorColor);
		editorColor.StringNumber = (intptr_t) lineNumber;
		editorColor.ColorItem = (intptr_t) 0;
		editorColor.StartPos = (intptr_t) pos;
		editorColor.EndPos = (intptr_t) pos;
		editorColor.Priority = (uintptr_t) PLUGIN_COLORING_PRIORITY;
		editorColor.Flags = ECF_AUTODELETE;
		editorColor.Color = PLUGIN_COLOR_USED;
		editorColor.Owner = pluginGUID;
		// editorControlResult =
		farAPI.EditorControl(
			CURRENT_EDITOR,
			ECTL_ADDCOLOR,
			(intptr_t) 0,
			&editorColor
		);
		// assert((MemoryAddress) editorControlResult == 1);
		// EXPLICITLY_UNUSED(editorControlResult);
	}
}