#include "far/plugin.hpp"
#include "common.h"

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

static const wchar_t* TITLE = L"IndentChecker";
static const wchar_t* DESCRIPTION = L"Checks indentation. Colors tabs if both spaces and tabs are "
	"used.";
static const VersionInfo PLUGIN_VERSION = {1, 0, 0, 1, VS_RELEASE};

static const unsigned int PLUGIN_COLORING_PRIORITY = 2;
static const FarColor PLUGIN_COLOR_USED = {FCF_4BITMASK, {0}, {14}, {0, 0}};

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
		return;
	}
	
	RegisterValue beginPos = 0;
	RegisterValue endPos = beginPos;
	for (; endPos + 1 < lineLength && isSpaceCharacter(line[endPos + 1]); ++endPos);
	
	if (endPos + 1 == lineLength) {
		return;
	}
	
	bool containsSpaces = false;
	for (RegisterValue pos = beginPos; pos <= endPos; ++pos) {
		if (line[pos] == L' ') {
			containsSpaces = true;
			break;
		}
	}
	if (!containsSpaces) {
		return;
	}
	
	for (RegisterValue pos = beginPos; pos <= endPos; ++pos) {
		if (line[pos] != L'\t') {
			continue;
		}

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