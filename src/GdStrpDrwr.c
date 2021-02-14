#include "far/plugin.hpp"
#include "common.h"

#include <stddef.h>
#include <assert.h>

static const wchar_t* TITLE = L"GdStrpDrwr";
static const wchar_t* DESCRIPTION = L"Draws guidestripe.";
static const VersionInfo PLUGIN_VERSION = {1, 0, 0, 1, VS_RELEASE};

static const unsigned int PLUGIN_COLORING_PRIORITY = 1;
static const FarColor PLUGIN_COLOR_USED = {FCF_4BITMASK, {0}, {7}, {0, 0}};
static const unsigned int GUIDESTRIPE_OFFSET = 100;

static void colorLine(const RegisterValue lineNumber) {
	EditorConvertPos editorConvertPos;
	editorConvertPos.StructSize = sizeof(EditorConvertPos);
	editorConvertPos.StringNumber = lineNumber;
	editorConvertPos.SrcPos = GUIDESTRIPE_OFFSET;
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
	
	EditorColor editorColor;
	editorColor.StructSize = sizeof(EditorColor);
	editorColor.StringNumber = (intptr_t) lineNumber;
	editorColor.ColorItem = (intptr_t) 0;
	editorColor.StartPos = (intptr_t) realPos;
	editorColor.EndPos = (intptr_t) realPos;
	editorColor.Priority = (uintptr_t) PLUGIN_COLORING_PRIORITY;
	editorColor.Flags = ECF_AUTODELETE | ECF_TABMARKFIRST; // TODO: проверить "ECF_TABMARKFIRST" без "TabColorer"!
	editorColor.Color = PLUGIN_COLOR_USED;
	editorColor.Owner = pluginGUID;
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