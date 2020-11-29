#include "far/plugin.hpp"
#include "common.h"

#include <windows.h>
#include <guiddef.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#define DLL_EXPORT __declspec(dllexport)
#define EXPLICITLY_UNUSED(variable) ((void) variable)

typedef struct FarColor FarColor;
typedef struct VersionInfo VersionInfo;
typedef struct EditorGetString EditorGetString;
typedef struct EditorInfo EditorInfo;
typedef struct PluginStartupInfo PluginStartupInfo;
typedef struct GlobalInfo GlobalInfo;
typedef struct PluginInfo PluginInfo;
typedef struct OpenInfo OpenInfo;
typedef struct EditorColor EditorColor;
typedef struct ProcessEditorEventInfo ProcessEditorEventInfo;


static const wchar_t* TITLE = L"TrlgSpcClrr";
static const wchar_t* DESCRIPTION = L"Colors trailing spacecharacters.";
static const wchar_t* AUTHOR = L"Valeriy Zainullin";

static const unsigned int PLUGIN_COLORING_PRIORITY = 2;
static const FarColor PLUGIN_COLOR_USED = {FCF_BG_4BIT, {(COLORREF) 0}, {(COLORREF) 0x0000FF00}};

static const VersionInfo MIN_FAR_VERSION = {3, 0, 0, 0, VS_RELEASE};
static const VersionInfo PLUGIN_VERSION = {1, 0, 0, 1, VS_RELEASE};


static bool farAPIWasInitialized = false;
static PluginStartupInfo farAPI;

static bool pluginGUIDInitialized = false;
static GUID pluginGUID;

DLL_EXPORT WINAPI void GetGlobalInfoW(GlobalInfo* info) {
	info->StructSize = sizeof(GlobalInfo);
	info->MinFarVersion = MIN_FAR_VERSION;
	info->Version = PLUGIN_VERSION;
	
	if (!pluginGUIDInitialized) {
		HRESULT createGuidResult = CoCreateGuid(&pluginGUID);
		assert(createGuidResult == S_OK);
		EXPLICITLY_UNUSED(createGuidResult);
		pluginGUIDInitialized = true;
	}
	
	info->Guid = pluginGUID;
	info->Title = TITLE;
	info->Description = DESCRIPTION;
	info->Author = AUTHOR;
}

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

DLL_EXPORT WINAPI void SetStartupInfo(const PluginStartupInfo* info) {
	if (info->StructSize < sizeof(PluginStartupInfo)) {
		return;
	}
	farAPI = *info;
	farAPIWasInitialized = true;
}

static bool isSpaceCharacter(const wchar_t character) {
	return character == L' ' || character == L'\t';
}

static void colorLine(const unsigned long long lineNumber) {
	EditorGetString editorString = {sizeof(EditorGetString)};
	intptr_t editorControlResult = farAPI.EditorControl(
		CURRENT_EDITOR,
		ECTL_GETSTRING,
		(intptr_t) (MemoryAddress) lineNumber,
		&editorString
	);
	assert((MemoryAddress) editorControlResult == 1);
	EXPLICITLY_UNUSED(editorControlResult);
	
	const wchar_t* currentLine = editorString.StringText;
	const wchar_t* currentEOL = editorString.StringEOL;
	unsigned long long currentLineLength = (MemoryAddress) editorString.StringLength;
	if (*currentEOL == L'\r') {
		currentLineLength -= 2;
	} else if (*currentEOL == L'\n') {
		currentLineLength -= 1;
	}
	unsigned long long endPos = currentLineLength - 1;
	if (!isSpaceCharacter(currentLine[endPos])) {
		return;
	}
	unsigned long long beginPos = endPos;
	for (; beginPos >= 1 && isSpaceCharacter(currentLine[beginPos]); --beginPos);
	
	EditorColor editorColor = {
		sizeof(EditorColor),
		(intptr_t) lineNumber,
		(intptr_t) (MemoryAddress) 0,
		(intptr_t) beginPos,
		(intptr_t) endPos,
		(uintptr_t) PLUGIN_COLORING_PRIORITY,
		ECF_AUTODELETE,
		PLUGIN_COLOR_USED,
		pluginGUID
	};
	editorControlResult = farAPI.EditorControl(CURRENT_EDITOR, ECTL_ADDCOLOR, (intptr_t) 0, &editorColor);
	assert((MemoryAddress) editorControlResult == 1);
	EXPLICITLY_UNUSED(editorControlResult);
}

DLL_EXPORT WINAPI intptr_t ProcessEditorEventW(const ProcessEditorEventInfo* info) {
	if (
		info->StructSize < sizeof(ProcessEditorEventInfo) ||
		!farAPIWasInitialized ||
		(MemoryAddress) info->Event != EE_REDRAW
	) {
		return (intptr_t) 0;
	}
	
	EditorInfo editorInfo;
	intptr_t editorControlResult = farAPI.EditorControl(CURRENT_EDITOR, ECTL_GETINFO, (intptr_t) 0, &editorInfo);
	assert((MemoryAddress) editorControlResult == 1);
	EXPLICITLY_UNUSED(editorControlResult);
	
	for (
		unsigned long long visibleLineNumber = 0;
		visibleLineNumber < (MemoryAddress) editorInfo.WindowSizeY;
		++visibleLineNumber
	) {
		colorLine(((MemoryAddress) editorInfo.TopScreenLine) + visibleLineNumber);
	}
	
	return (intptr_t) 0;
}

DLL_EXPORT BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	EXPLICITLY_UNUSED(hinstDLL);
	EXPLICITLY_UNUSED(fdwReason);
	EXPLICITLY_UNUSED(lpReserved);
	return TRUE;
}