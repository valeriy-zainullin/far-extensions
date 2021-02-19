#ifndef COMMON_H
#define COMMON_H

#ifndef TARGET
#error "TARGET" macro is not defined!
#endif

#include <assert.h>
#include <stdbool.h>
#include <windows.h>

#if TARGET == X86
typedef unsigned int MemoryAddress;
typedef unsigned int RegisterValue;
#elif TARGET == X86-64
typedef unsigned long long MemoryAddress;
typedef unsigned long long RegisterValue;
#else
#error Target is not supported!
#endif

#ifndef NDEBUG
#include <stdio.h>
#include <assert.h>
#define DEBUG_PRINTF(format, arg) assert(fprintf(stderr, format, arg) >= 0)
#define DEBUG_PRINT(format) assert(fprintf(stderr, format))
#else
#define DEBUG_PRINTF(format, arg)
#define DEBUG_PRINT(format)
#endif


#define DLL_EXPORT __declspec(dllexport)
#define EXPLICITLY_UNUSED(variable) ((void) variable)

typedef struct FarColor FarColor;
typedef struct VersionInfo VersionInfo;
typedef struct EditorGetString EditorGetString;
typedef struct EditorConvertPos EditorConvertPos;
typedef struct EditorInfo EditorInfo;
typedef struct PluginStartupInfo PluginStartupInfo;
typedef struct GlobalInfo GlobalInfo;
typedef struct PluginInfo PluginInfo;
typedef struct OpenInfo OpenInfo;
typedef struct EditorColor EditorColor;
typedef struct ProcessEditorEventInfo ProcessEditorEventInfo;

DLL_EXPORT BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	EXPLICITLY_UNUSED(hinstDLL);
	EXPLICITLY_UNUSED(fdwReason);
	EXPLICITLY_UNUSED(lpReserved);
	return TRUE;
}

static const wchar_t* TITLE;
static const wchar_t* DESCRIPTION;
static const wchar_t* AUTHOR = L"Valeriy Zainullin";
static const VersionInfo MIN_FAR_VERSION = {3, 0, 0, 0, VS_RELEASE};
static const VersionInfo PLUGIN_VERSION;
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

static PluginStartupInfo farAPI;
DLL_EXPORT WINAPI void SetStartupInfoW(const PluginStartupInfo* info) {
	farAPI = *info;
}


static RegisterValue minForRegisterValues(RegisterValue first, RegisterValue second) {
	if (first <= second) {
		return first;
	} else {
		return second;
	}
}

static void newColoring();
static void colorLine(const RegisterValue lineNumber);
DLL_EXPORT WINAPI intptr_t ProcessEditorEventW(const ProcessEditorEventInfo* info) {
	if ((MemoryAddress) info->Event != EE_REDRAW) {
		return (intptr_t) 0;
	}
	
	EditorInfo editorInfo;
	intptr_t editorControlResult = farAPI.EditorControl(CURRENT_EDITOR, ECTL_GETINFO, (intptr_t) 0, &editorInfo);
	if ((MemoryAddress) editorControlResult != 1) {
		return (intptr_t) 0;
	}
	// assert((MemoryAddress) editorControlResult == 1);
	// EXPLICITLY_UNUSED(editorControlResult);
	
	RegisterValue begin = editorInfo.TopScreenLine;
	RegisterValue end = minForRegisterValues(
		(RegisterValue) (editorInfo.TopScreenLine + editorInfo.WindowSizeY),
		editorInfo.TotalLines
	);
	newColoring();
	for (RegisterValue lineNumber = begin; lineNumber < end; ++lineNumber) {
		colorLine(lineNumber);
	}
	
	return (intptr_t) 0;
}

static void getLine(RegisterValue lineNumber, const wchar_t** line, RegisterValue* lineLength) {
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
	
	*line = editorString.StringText;
	*lineLength = editorString.StringLength;
}

static const unsigned int PLUGIN_COLORING_PRIORITY;
static void colorCharacter(
	RegisterValue lineNumber,
	RegisterValue position,
	FarColor color,
	EDITORCOLORFLAGS flags
) {
	EditorColor editorColor;
	editorColor.StructSize = sizeof(EditorColor);
	editorColor.StringNumber = (intptr_t) lineNumber;
	editorColor.ColorItem = (intptr_t) 0;
	editorColor.StartPos = (intptr_t) position;
	editorColor.EndPos = (intptr_t) position;
	editorColor.Priority = (uintptr_t) PLUGIN_COLORING_PRIORITY;
	editorColor.Flags = ECF_AUTODELETE | flags;
	editorColor.Color = color;
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

#endif