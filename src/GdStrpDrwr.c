#include "far/plugin.hpp"
#include "common.h"

#include <stddef.h>
#include <assert.h>

static const wchar_t* TITLE = L"GdStrpDrwr";
static const wchar_t* DESCRIPTION = L"Draws guidestripe.";
static const VersionInfo PLUGIN_VERSION = {1, 0, 0, 1, VS_RELEASE};

static const unsigned int PLUGIN_COLORING_PRIORITY = 1;
static const FarColor PLUGIN_COLOR_USED = {FCF_4BITMASK, {0}, {7}};
static const unsigned int GUIDESTRIPE_OFFSET = 100;

static void colorLine(const RegisterValue lineNumber) {
	EditorConvertPos editorConvertPos = {sizeof(EditorConvertPos), lineNumber, GUIDESTRIPE_OFFSET};
	intptr_t editorControlResult = farAPI.EditorControl(
		CURRENT_EDITOR,
		ECTL_TABTOREAL,
		(intptr_t) 0,
		&editorConvertPos
	);
	if ((MemoryAddress) editorControlResult != 1) {
		return;
	}
	RegisterValue realPos = (MemoryAddress) editorConvertPos.DestPos;
	
	EditorColor editorColor = {
		sizeof(EditorColor),
		(intptr_t) lineNumber,
		(intptr_t) 0,
		(intptr_t) realPos,
		(intptr_t) realPos,
		(uintptr_t) PLUGIN_COLORING_PRIORITY,
		ECF_AUTODELETE,
		PLUGIN_COLOR_USED,
		pluginGUID
	};
	//intptr_t editorControlResult = 
	farAPI.EditorControl(
		CURRENT_EDITOR,
		ECTL_ADDCOLOR,
		(intptr_t) 0,
		&editorColor
	);
	// assert((MemoryAddress) editorControlResult == 1);
	// EXPLICITLY_UNUSED(editorControlResult);
}