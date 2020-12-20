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

static void colorLine(const unsigned long long lineNumber) {
	EditorColor editorColor = {
		sizeof(EditorColor),
		(intptr_t) (MemoryAddress) lineNumber,
		(intptr_t) (MemoryAddress) 0,
		(intptr_t) GUIDESTRIPE_OFFSET,
		(intptr_t) GUIDESTRIPE_OFFSET,
		(uintptr_t) PLUGIN_COLORING_PRIORITY,
		ECF_AUTODELETE,
		PLUGIN_COLOR_USED,
		pluginGUID
	};
	intptr_t editorControlResult = farAPI.EditorControl(
		CURRENT_EDITOR,
		ECTL_ADDCOLOR,
		(intptr_t) 0,
		&editorColor
	);
	assert((MemoryAddress) editorControlResult == 1);
	EXPLICITLY_UNUSED(editorControlResult);
}