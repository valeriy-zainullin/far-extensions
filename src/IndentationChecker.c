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
static const FarColor PLUGIN_COLOR_USED = {FCF_4BITMASK, {0}, {14}};

static bool isSpaceCharacter(const wchar_t character) {
	return character == L' ' || character == L'\t';
}

static void colorLine(const RegisterValue lineNumber) {
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
		EditorColor editorColor = {
			sizeof(EditorColor),
			(intptr_t) lineNumber,
			(intptr_t) 0,
			(intptr_t) pos,
			(intptr_t) pos,
			(uintptr_t) PLUGIN_COLORING_PRIORITY,
			ECF_AUTODELETE,
			PLUGIN_COLOR_USED,
			pluginGUID
		};
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